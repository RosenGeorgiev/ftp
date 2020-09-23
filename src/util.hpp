/**
 * @file util.hpp
 */
#pragma once

#include <regex>
#include <tuple>
#include <string>
#include <vector>
#include <sstream>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <system_error>


namespace rs
{
namespace ftp
{

#define CHECK_EC(__std_error_code) if (__std_error_code)                        \
                                   {                                            \
                                       log_debug(__std_error_code.message());   \
                                       return {};                               \
                                   }
#define CHECK_EC_VOID(__std_error_code) if (__std_error_code)                       \
                                        {                                           \
                                            log_debug(__std_error_code.message());  \
                                            return;                                 \
                                        }

#define CHECK_CONVERT_EC(__boost_error_code, __std_error_code)      \
    if (__boost_error_code)                                         \
    {                                                               \
        __std_error_code = boost_to_std_ec(__boost_error_code);     \
        return {};                                                  \
    }
#define CHECK_CONVERT_EC_VOID(__boost_error_code, __std_error_code) \
    if (__boost_error_code)                                         \
    {                                                               \
        __std_error_code = boost_to_std_ec(__boost_error_code);     \
        return;                                                     \
    }


static std::regex const ipv4_regex{R"###((\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3}))###"};
static std::regex const pasv_reply_regex{R"###(\((\d{1,3}),(\d{1,3}),(\d{1,3}),(\d{1,3}),(\d{1,3}),(\d{1,3})\))###"};

inline auto boost_to_std_ec(boost::system::error_code& a_boost_ec) noexcept -> std::error_code
{
    return std::make_error_code(static_cast<std::errc>(a_boost_ec.value()));
}

inline auto log_debug(std::string const& a_log_str) noexcept -> void
{
#if 1
    std::cout << a_log_str << std::endl;
#endif
}

inline auto parse_codes(
    std::string const& a_reply_str
) noexcept
-> std::vector<reply_code>
{
    static std::regex const codes_regex{"(\\d{3})"};
    std::smatch codes_match;
    std::vector<reply_code> ret_codes;
    std::string search_str{a_reply_str};

    // NOTE - Even if we get an invalid reply_code nothing scary should happen.
    while (std::regex_search(search_str, codes_match, codes_regex))
    {
        ret_codes.push_back(static_cast<reply_code>(std::stoi(codes_match.str())));
        search_str = codes_match.suffix();
    }

    return ret_codes;
}

inline auto check_success(
    std::vector<reply_code> const& a_accepted_codes,
    std::string const& a_reply_str,
    std::error_code& a_ec
) noexcept
-> void
{
    auto returned_codes = parse_codes(a_reply_str);

    if (returned_codes.empty())
    {
        log_debug("No reply codes returned!");
        a_ec = std::make_error_code(std::errc::no_message);
        return;
    }

    std::vector<reply_code> matched;

    std::set_intersection(
        a_accepted_codes.begin(),
        a_accepted_codes.end(),
        returned_codes.begin(),
        returned_codes.end(),
        std::back_inserter(matched)
    );

    // TODO - Better errors.
    if (matched.empty())
    {
        a_ec = std::make_error_code(std::errc::bad_message);
    }
}

inline auto parse_ipv4(
    std::string const& a_ip_str,
    std::error_code& a_ec
) noexcept
-> std::vector<int>
{
    std::smatch ip_match;

    if (!std::regex_match(a_ip_str, ip_match, ipv4_regex) || ip_match.size() < 4)
    {
        log_debug("Failed to parse IP address!");
        a_ec = std::make_error_code(std::errc::address_not_available);
        return {};
    }

    log_debug(ip_match[0]);

    return {
        std::stoi(ip_match[1]),
        std::stoi(ip_match[2]),
        std::stoi(ip_match[3]),
        std::stoi(ip_match[4])
    };
}

inline auto port_to_network(unsigned short a_port) noexcept
-> std::tuple<uint8_t, uint8_t>
{
    return std::make_tuple(a_port >> 8, a_port);
}

inline auto parse_pasv_ipv4_port_reply(
    std::string const& a_pasv_reply,
    std::error_code& a_ec
) noexcept
-> std::tuple<std::vector<int>, unsigned short>
{
    std::smatch ip_port_match;

    if (!std::regex_search(a_pasv_reply, ip_port_match, pasv_reply_regex) ||
        ip_port_match.size() < 6)
    {
        log_debug("Failed to parse PASV reply!");
        a_ec = std::make_error_code(std::errc::address_not_available);
        return {};
    }

    log_debug(ip_port_match[0]);

    std::vector<int> address{
        std::stoi(ip_port_match[1]),
        std::stoi(ip_port_match[2]),
        std::stoi(ip_port_match[3]),
        std::stoi(ip_port_match[4])
    };
    unsigned short port = (256 * std::stoi(ip_port_match[5])) + std::stoi(ip_port_match[6]);

    return std::make_tuple(
        address,
        port
    );
}

inline auto ipv4_vec_to_str(std::vector<int> const& a_ip_vec) noexcept -> std::string
{
    assert(a_ip_vec.size() == 4 && "vector too short or too long");

    std::ostringstream ss;

    ss << a_ip_vec[0]
       << "."
       << a_ip_vec[1]
       << "."
       << a_ip_vec[2]
       << "."
       << a_ip_vec[3];

    return ss.str();
}

}   // namespace ftp
}   // namespace rs

