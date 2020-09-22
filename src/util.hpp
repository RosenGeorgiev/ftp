/**
 * @file util.hpp
 */
#pragma once

#include <regex>
#include <string>
#include <vector>
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

#define TRYB(__boost_error_code, __std_error_code) if (__boost_error_code)                                      \
                                                   {                                                            \
                                                       __std_error_code = boost_to_std_ec(__boost_error_code);  \
                                                       return {};                                               \
                                                   }
#define TRYB_VOID(__boost_error_code, __std_error_code) if (__boost_error_code)                                     \
                                                        {                                                           \
                                                            __std_error_code = boost_to_std_ec(__boost_error_code); \
                                                            return;                                                 \
                                                        }

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

}   // namespace ftp
}   // namespace rs

