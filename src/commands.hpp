/**
 * @file commands.hpp
 */
#pragma once

#include <sstream>

#include <ftp/codes.hpp>


namespace rs
{
namespace ftp
{

inline auto quit_command() noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::QUIT)
       << CRLF;
    return ss.str();
}

inline auto user_command(std::string const& a_username) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::USER)
       << SP
       << a_username
       << CRLF;

    return ss.str();
}

inline auto password_command(std::string const& a_password) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::PASS)
       << SP
       << a_password
       << CRLF;

    return ss.str();
}

inline auto cwd_command(std::string const& a_new_wd) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::CWD)
       << SP
       << a_new_wd
       << CRLF;

    return ss.str();
}

inline auto cdup_command() noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::CDUP)
       << CRLF;

    return ss.str();
}

inline auto smnt_command(std::string const& a_mount_point) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::SMNT)
       << SP
       << a_mount_point
       << CRLF;

    return ss.str();
}

inline auto rein_command() noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::REIN)
       << CRLF;

    return ss.str();
}

inline auto port_command(
    std::string const& a_h1,
    std::string const& a_h2,
    std::string const& a_h3,
    std::string const& a_h4,
    std::string const& a_p1,
    std::string const& a_p2
) noexcept
-> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::PORT)
       << SP
       << a_h1
       << COMMA
       << a_h2
       << COMMA
       << a_h3
       << COMMA
       << a_h4
       << COMMA
       << a_p1
       << COMMA
       << a_p2
       << CRLF;

    return ss.str();
}

inline auto pasv_command() noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::PASV)
       << CRLF;

    return ss.str();
}

inline auto type_command(data_type a_data_type) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::TYPE)
       << SP
       << data_type_to_str(a_data_type)
       << CRLF;

    return ss.str();
}

inline auto stru_command(file_structure a_structure) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::STRU)
       << SP
       << file_structure_to_str(a_structure)
       << CRLF;

    return ss.str();
}

inline auto mode_command(transmission_mode a_mode) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::MODE)
       << SP
       << transmission_mode_to_str(a_mode)
       << CRLF;

    return ss.str();
}

inline auto retr_command(std::string const& a_filename) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::RETR)
       << SP
       << a_filename
       << CRLF;

    return ss.str();
}

inline auto stor_command(std::string const& a_filename) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::STOR)
       << SP
       << a_filename
       << CRLF;

    return ss.str();
}

inline auto appe_command(std::string const& a_filename) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::APPE)
       << SP
       << a_filename
       << CRLF;

    return ss.str();
}

inline auto allo_command(int a_bytes_to_reserve) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::ALLO)
       << SP
       << a_bytes_to_reserve
       << CRLF;

    return ss.str();
}

inline auto allo_command(int a_bytes_to_reserve, int a_max_record_or_page_size) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::ALLO)
       << SP
       << a_bytes_to_reserve
       << SP
       << 'R'
       << SP
       << a_max_record_or_page_size
       << CRLF;

    return ss.str();
}

inline auto rnfr_command(std::string const& a_file_to_rename) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::RNFR)
       << SP
       << a_file_to_rename
       << CRLF;

    return ss.str();
}

inline auto rnto_command(std::string const& a_rename_to) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::RNTO)
       << SP
       << a_rename_to
       << CRLF;

    return ss.str();
}

inline auto dele_command(std::string const& a_filepath) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::DELE)
       << SP
       << a_filepath
       << CRLF;

    return ss.str();
}

inline auto rmd_command(std::string const& a_dirpath) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::RMD)
       << SP
       << a_dirpath
       << CRLF;

    return ss.str();
}

inline auto mkd_command(std::string const& a_dirpath) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::MKD)
       << SP
       << a_dirpath
       << CRLF;

    return ss.str();
}

inline auto pwd_command() noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::PWD)
       << CRLF;

    return ss.str();
}

inline auto list_command(std::string const& a_pathname = {}) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::LIST);

    if (!a_pathname.empty())
    {
        ss << SP;
        ss << a_pathname;
    }

    ss << CRLF;

    return ss.str();
}

inline auto nlst_command(std::string const& a_pathname = {}) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::NLST);

    if (!a_pathname.empty())
    {
        ss << SP;
        ss << a_pathname;
    }

    ss << CRLF;

    return ss.str();
}

inline auto syst_command() noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::SYST)
       << CRLF;

    return ss.str();
}

inline auto stat_command(std::string const& a_pathname = {}) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::STAT);

    if (!a_pathname.empty())
    {
        ss << SP;
        ss << a_pathname;
    }

    ss << CRLF;

    return ss.str();
}

inline auto noop_command() noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::NOOP)
       << CRLF;

    return ss.str();
}

inline auto auth_command(authentication_method a_auth_method) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::AUTH)
       << SP
       << authentication_method_to_str(a_auth_method)
       << CRLF;

    return ss.str();
}

inline auto adat_command(std::string const& a_data) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::ADAT)
       << SP
       << a_data
       << CRLF;

    return ss.str();
}

inline auto pbsz_command(int a_size) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::PBSZ)
       << SP
       << a_size
       << CRLF;

    return ss.str();
}

inline auto ccc_command() noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::CCC)
       << CRLF;

    return ss.str();
}

inline auto prot_command(data_channel_protection_level a_protection_level) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::PROT)
       << SP
       << data_channel_protection_level_to_str(a_protection_level)
       << CRLF;

    return ss.str();
}

inline auto mic_command(std::string const& a_data) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::MIC)
       << SP
       << a_data
       << CRLF;

    return ss.str();
}

inline auto conf_command(std::string const& a_data) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::CONF)
       << SP
       << a_data
       << CRLF;

    return ss.str();
}

inline auto enc_command(std::string const& a_data) noexcept -> std::string
{
    std::ostringstream ss;

    ss << ftp_command_to_str(ftp_command::ENC)
       << SP
       << a_data
       << CRLF;

    return ss.str();
}

}   // namespace ftp
}   // namespace rs

