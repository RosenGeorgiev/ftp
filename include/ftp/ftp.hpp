/**
 * @file ftp.hpp
 */
#pragma once

#include <memory>
#include <vector>
#include <fstream>
#include <exception>
#include <functional>

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
    bool passive_mode{true};
    bool debug_output{false};
    // @Unimplemented
    data_type type{data_type::ASCII};
    // @Unimplemented
    format_control control{format_control::NON_PRINT};
    // @Unimplemented
    transmission_mode mode{transmission_mode::STREAM};
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
            int a_port
        )
        -> void;

        auto accept_v4(int a_port)
        -> void;

        auto close() -> void;

        auto read(int a_max)
        -> std::vector<char>;

        auto read_until(std::string const& a_delimiter)
        -> std::string;

        auto read_until(char a_delimiter)
        -> std::string;

        auto write(std::string const& a_buf)
        -> void;

        auto is_open() noexcept -> bool;

    private:
        struct impl;
        std::unique_ptr<impl> m_impl;
    };

    class resolver
    {
    public:
        static auto resolve_v4(std::string const& a_hostname)
        -> std::string;
    };

public:
    client() =default;
    client(connection_options const& a_opts);

    /**
     * @brief
     */
    auto set_connection_options(connection_options const& a_opts) noexcept -> void;

    /**
     * @brief
     */
    auto connect() -> void;
    /*
    auto connect(
        std::string const& a_hostname,
        int a_port,
        std::error_code& a_ec
    ) noexcept
    -> void;
    */
    /**
     * @brief
     */
    auto close() -> void;
    /**
     * @brief
     */
    auto login() -> void;
    /*
    auto login(
        std::string const& a_username,
        std::string const& a_password,
        std::error_code& a_ec
    ) noexcept
    -> void;
    */
    /**
     * @brief
     */
    auto cwd(std::string const& a_new_wd) -> void;
    /**
     * @brief
     */
    auto cdup() -> void;

    /**
     * @note Not supported by vsftpd.
     */
    auto smnt(std::string const& a_mount_point) -> void;

    /**
     * @note Actually REIN. Logout is more descriptive of what it does. Not supported by vsftpd.
     */
    auto logout() -> void;
    /**
     * @brief
     */
    auto download(std::string const& a_filename)
    -> std::vector<char>;
    /**
     * @brief
     */
    auto download(
        std::string const& a_filename,
        std::ofstream& a_ofstream
    )
    -> void;
    /**
     * @brief
     */
    auto upload(
        std::string const& a_filename,
        std::istream& a_istream
    )
    -> void;
    /**
     * @brief
     */
    auto rename(
        std::string const& a_file_to_rename,
        std::string const& a_rename_to
    )
    -> void;
    /**
     * @brief
     */
    auto remove_file(std::string const& a_filepath)
    -> void;
    /**
     * @brief
     */
    auto rmdir(std::string const& a_dirpath)
    -> void;
    /**
     * @brief
     */
    auto mkdir(std::string const& a_dirpath)
    -> void;
    /**
     * @brief
     */
    auto pwd() -> std::string;
    /**
     * @brief
     */
    auto ls() -> std::string;
    /**
     * @brief
     */
    auto ls(std::string const& a_pathname) -> std::string;
    /**
     * @brief
     */
    auto system_info() -> std::string;
    /**
     * @brief
     */
    auto progress() -> std::string;
    /**
     * @brief
     */
    auto noop() -> void;

private:
    /**
     * @brief
     */
    auto download_active(
        std::string const& a_filename,
        std::function<void(std::vector<char> const&)> a_data_callback
    )
    -> void;
    /**
     * @brief
     */
    auto download_passive(
        std::string const& a_filename,
        std::function<void(std::vector<char> const&)> a_data_callback
    )
    -> void;
    /**
     * @brief
     */
    auto enter_passive_mode(connection& a_data_transfer_connection)
    -> void;

private:
    connection_options m_options;
    connection m_control_connection;
};

class end_of_file_error : public std::runtime_error
{
public:
    explicit end_of_file_error(std::string const& a_msg) :
        std::runtime_error(a_msg)
    { }

    explicit end_of_file_error(char const* a_msg) :
        std::runtime_error(a_msg)
    { }
};

}   // namespace ftp
}   // namespace rs

