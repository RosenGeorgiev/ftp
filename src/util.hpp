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
#include <exception>
#include <algorithm>

#include "logger.hpp"


namespace rs
{
namespace ftp
{

static std::regex const codes_regex{"(\\d{3})"};
static std::regex const ipv4_regex{R"###((\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3}))###"};
static std::regex const pasv_reply_regex{R"###(\((\d{1,3}),(\d{1,3}),(\d{1,3}),(\d{1,3}),(\d{1,3}),(\d{1,3})\))###"};
static std::regex const epsv_reply_regex{R"###(\(\|([12])?\|(.+)?\|([0-9]{1,5})\|\))###"};

inline auto parse_codes(
    std::string const& a_reply_str
) noexcept
-> std::vector<reply_code>
{
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
    std::string const& a_reply_str
)
-> void
{
    auto returned_codes = parse_codes(a_reply_str);

    if (returned_codes.empty())
    {
        throw std::runtime_error("No reply codes returned - invalid response");
    }

    std::vector<reply_code> matched;

    std::set_intersection(
        a_accepted_codes.begin(),
        a_accepted_codes.end(),
        returned_codes.begin(),
        returned_codes.end(),
        std::back_inserter(matched)
    );

    if (matched.empty())
    {
        throw std::runtime_error("No reply codes matched - operation failed");
    }
}

inline auto parse_ipv4(
    std::string const& a_ip_str
)
-> std::vector<int>
{
    std::smatch ip_match;

    if (!std::regex_match(a_ip_str, ip_match, ipv4_regex) || ip_match.size() < 4)
    {
        throw std::runtime_error("Failed to parse IP address");
    }

    logger::debug(ip_match[0]);

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
    std::string const& a_pasv_reply
)
-> std::tuple<std::vector<int>, unsigned short>
{
    std::smatch ip_port_match;

    if (!std::regex_search(a_pasv_reply, ip_port_match, pasv_reply_regex) ||
        ip_port_match.size() < 6)
    {
        throw std::runtime_error("Failed to parse PASV reply");
    }

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

struct epsv_reply
{
    // NOTE - Empty in EPSV responses.
    address_family family{address_family::AF_INET4};
    // NOTE - Empty in EPSV responses.
    std::string address{};
    unsigned short port{DEFAULT_DATA_CONNECTION_PORT};
};

inline auto parse_epsv_reply(
    std::string const& a_epsv_reply
)
-> epsv_reply
{
    std::smatch match;

    if (!std::regex_search(a_epsv_reply, match, epsv_reply_regex) || match.size() < 2)
    {
        throw std::runtime_error("Failed to parse EPSV reply");
    }

    epsv_reply reply;


    if (!match[1].str().empty())
    {

        reply.family = str_to_address_family(match[1].str());
    }

    reply.address = match[2];
    reply.port = std::stoi(match[3]);

    return reply;
}

}   // namespace ftp
}   // namespace rs

