/**
 * @file ftp.hpp
 */
#pragma once

#include <memory>
#include <future>
#include <system_error>

#include "codes.hpp"


namespace rs
{
namespace ftp
{

struct connection_options
{
    std::string username{};
    std::string password{};
    std::string server_hostname{};
    unsigned short server_port{DEFAULT_DATA_CONNECTION_PORT};
    std::string data_connection_host{};
    unsigned short data_connection_port{DEFAULT_DATA_CONNECTION_PORT};
    // @Unimplemented
    bool passive_mode{false};
    bool debug_output{false};
    // @Unimplemented
    data_type type{data_type::ASCII};
    // @Unimplemented
    transmission_mode mode{transmission_mode::BLOCK};
    // @Unimplemented
    file_structure structure{file_structure::FILE_STRUCTURE};
};

class client
{
    class connection
    {
    public:
        connection();
        ~connection() noexcept;

        auto connect(
            std::string const& a_hostname,
            int a_port,
            std::error_code& a_ec
        ) noexcept
        -> void;

        auto accept_v4(
            int a_port,
            std::error_code& a_ec
        ) noexcept
        -> void;

        auto close(std::error_code& a_ec) noexcept -> void;

        auto read(
            int a_max,
            std::error_code& a_ec
        ) noexcept
        -> std::string;

        auto read_until(
            std::string const& a_delimiter,
            std::error_code& a_ec
        ) noexcept
        -> std::string;

        auto read_until(
            char a_delimiter,
            std::error_code& a_ec
        ) noexcept
        -> std::string;

        auto write(
            std::string const& a_buf,
            std::error_code& a_ec
        ) noexcept -> void;

        auto is_open() noexcept -> bool;

    private:
        struct impl;
        std::unique_ptr<impl> m_impl;
    };

    class resolver
    {
    public:
        static auto resolve_v4(
            std::string const& a_hostname,
            std::error_code& a_ec
        ) noexcept
        -> std::string;
    };

public:
    client() =default;
    client(connection_options const& a_opts);

    auto set_connection_options(connection_options const& a_opts) noexcept -> void;

    auto connect(std::error_code& a_ec) noexcept -> void;
    /*
    auto connect(
        std::string const& a_hostname,
        int a_port,
        std::error_code& a_ec
    ) noexcept
    -> void;
    */
    auto close(std::error_code& a_ec) noexcept -> void;

    auto login(std::error_code& a_ec) noexcept -> void;
    /*
    auto login(
        std::string const& a_username,
        std::string const& a_password,
        std::error_code& a_ec
    ) noexcept
    -> void;
    */

    auto cwd(std::string const& a_new_wd, std::error_code& a_ec) noexcept -> void;
    auto cdup(std::error_code& a_ec) noexcept -> void;

    /**
     * @note Not supported by vsftpd.
     */
    auto smnt(std::string const& a_mount_point, std::error_code& a_ec) noexcept -> void;

    /**
     * @note Actually REIN. Logout is more descriptive of what it does. Not supported by vsftpd.
     */
    auto logout(std::error_code& a_ec) noexcept -> void;

    auto download(
        std::string const& a_filename,
        std::error_code& a_ec
    ) noexcept -> void;

    auto rename(
        std::string const& a_file_to_rename,
        std::string const& a_rename_to,
        std::error_code& a_ec
    ) noexcept
    -> void;

    auto remove_file(
        std::string const& a_filepath,
        std::error_code& a_ec
    ) noexcept
    -> void;

    auto rmdir(
        std::string const& a_dirpath,
        std::error_code& a_ec
    ) noexcept
    -> void;

    auto mkdir(
        std::string const& a_dirpath,
        std::error_code& a_ec
    ) noexcept
    -> void;

    auto noop(std::error_code& a_ec) noexcept -> void;

private:
    auto prepare_ip_port(
        std::string& h1,
        std::string& h2,
        std::string& h3,
        std::string& h4,
        std::string& p1,
        std::string& p2,
        std::error_code& a_ec
    ) noexcept
    -> void;
    auto accept_and_read(std::promise<std::error_code> a_ec) noexcept -> void;

private:
    connection_options m_options;
    connection m_control_connection;
};

}   // namespace ftp
}   // namespace rs

