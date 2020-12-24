/**
 * @file ftp.hpp
 */
#pragma once

#include <memory>
#include <vector>
#include <chrono>
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
    // Unused due to the lack of active mode support
    // std::string data_connection_host{};
    // unsigned short data_connection_port{DEFAULT_DATA_CONNECTION_PORT};
    /**
     * Prints all the correspondence between the server and the client.
     *
     * @warning Affects all instances of the class.
     */
    bool debug_output{false};
    std::chrono::milliseconds timeout{60000};
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
            int a_port,
            std::chrono::milliseconds const& a_timeout
        )
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

        auto write(char const* a_buf, int a_buf_size)
        -> void;

        auto is_open() noexcept -> bool;

    private:
        struct impl;
        std::unique_ptr<impl> m_impl;
    };

public:
    client() =default;
    client(connection_options const& a_opts);

    /**
     * @brief
     *
     * @param[in] a_opts
     */
    auto set_connection_options(connection_options const& a_opts) noexcept -> void;
    /**
     * @brief
     *
     * @throws std::runtime_error If the server returns an unexpected response
     * @throws std::invalid_argument If invalid hostname or port is passed
     * @throws boost::system::system_error If host resolution or couldn't connect to the server
     */
    auto connect() -> void;
    /**
     * @brief
     *
     * @param[in] a_hostname
     * @param[in] a_port
     *
     * @throws std::runtime_error If the server returns an unexpected response
     * @throws std::invalid_argument If invalid hostname or port is passed
     * @throws boost::system::system_error If host resolution or couldn't connect to the server
     */
    auto connect(
        std::string const& a_hostname,
        int a_port
    )
    -> void;
    /**
     * @brief
     *
     * @throws boost::system::system_error
     */
    auto close() -> void;
    /**
     * @brief
     *
     * @throws std::runtime_error If the server returns an unexpected response
     * @throws boost::system::system_error If writing to the socket fails
     */
    auto login() -> void;
    /**
     * @brief
     *
     * @param[in] a_username
     * @param[in] a_password
     *
     * @throws std::runtime_error If the server returns an unexpected response
     * @throws boost::system::system_error If writing to the socket fails
     */
    auto login(
        std::string const& a_username,
        std::string const& a_password
    )
    -> void;
    /**
     * @brief
     *
     * @param[in] a_new_wd
     *
     * @throws std::runtime_error If the server returns an unexpected response
     * @throws boost::system::system_error If writing to the socket fails
     */
    auto cwd(std::string const& a_new_wd) -> void;
    /**
     * @brief
     *
     * @throws std::runtime_error If the server returns an unexpected response
     * @throws boost::system::system_error If writing to the socket fails
     */
    auto cdup() -> void;
    /**
     * @brief
     *
     * @param[in] a_filename
     *
     * @throws std::runtime_error If parsing the PASV response fails
     * @throws std::runtime_error If the server returns an unexpected response
     * @throws boost::system::system_error If reading/writing to the socket fails or data transfer
     * connection fails.
     *
     * @returns std::vector<char>
     */
    auto download(std::string const& a_filename)
    -> std::vector<char>;
    /**
     * @brief
     *
     * @param[in] a_filename
     * @param[out] a_ofstream
     *
     * @throws std::runtime_error If parsing the PASV response fails
     * @throws std::runtime_error If the server returns an unexpected response
     * @throws boost::system::system_error If reading/writing to the socket fails or data transfer
     * connection fails.
     */
    auto download(
        std::string const& a_filename,
        std::ofstream& a_ofstream
    )
    -> void;
    /**
     * @brief
     *
     * @param[in] a_filename
     * @param[in] a_istream
     *
     * @throws std::runtime_error If parsing the PASV response fails
     * @throws std::runtime_error If the server returns an unexpected response
     * @throws boost::system::system_error If reading/writing to the socket fails or data transfer
     * connection fails.
     */
    auto upload(
        std::string const& a_filename,
        std::istream& a_istream
    )
    -> void;
    /**
     * @brief
     *
     * @param[in] a_file_to_rename
     * @param[in] a_rename_to
     *
     * @throws std::runtime_error If the server returns an unexpected response
     * @throws boost::system::system_error If reading/writing to the socket fails
     */
    auto rename(
        std::string const& a_file_to_rename,
        std::string const& a_rename_to
    )
    -> void;
    /**
     * @brief
     *
     * @param[in] a_filepath
     *
     * @throws std::runtime_error If the server returns an unexpected response
     * @throws boost::system::system_error If reading/writing to the socket fails
     */
    auto remove_file(std::string const& a_filepath)
    -> void;
    /**
     * @brief
     *
     * @param[in] a_dirpath
     *
     * @throws std::runtime_error If the server returns an unexpected response
     * @throws boost::system::system_error If reading/writing to the socket fails
     */
    auto rmdir(std::string const& a_dirpath)
    -> void;
    /**
     * @brief
     *
     * @param[in] a_dirpath
     *
     * @throws std::runtime_error If the server returns an unexpected response
     * @throws boost::system::system_error If reading/writing to the socket fails
     */
    auto mkdir(std::string const& a_dirpath)
    -> void;
    /**
     * @brief
     *
     * @throws std::runtime_error If the server returns an unexpected response
     * @throws std::length_error If the server returns a malformed response
     * @throws boost::system::system_error If reading/writing to the socket fails
     *
     * @returns std::string
     */
    auto pwd() -> std::string;
    /**
     * @brief
     *
     * @throws std::runtime_error If the server returns an unexpected response
     * @throws boost::system::system_error If reading/writing to the socket fails or reading from
     * the data transfer connection fails
     *
     * @returns std::string
     */
    auto ls() -> std::string;
    /**
     * @brief
     *
     * @param[in] a_pathname
     *
     * @throws std::runtime_error If the server returns an unexpected response
     * @throws boost::system::system_error If reading/writing to the socket fails or reading from
     * the data transfer connection fails
     *
     * @returns std::string
     */
    auto ls(std::string const& a_pathname) -> std::string;
    /**
     * @brief
     *
     * @throws std::runtime_error If the server returns an unexpected response
     * @throws std::length_error If the server returns a malformed response
     * @throws boost::system::system_error If reading/writing to the socket fails
     *
     * @returns std::string
     */
    auto system_info() -> std::string;
    /**
     * @brief
     *
     * @throws std::runtime_error If the server returns an unexpected response
     * @throws std::length_error If the server returns a malformed response
     * @throws boost::system::system_error If reading/writing to the socket fails
     *
     * @returns std::string
     */
    auto progress() -> std::string;
    /**
     * @brief
     *
     * @throws std::runtime_error If the server returns an unexpected response
     * @throws boost::system::system_error If reading/writing to the socket fails
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

class timeout_error : public std::runtime_error
{
public:
    explicit timeout_error(std::string const& a_msg) :
        std::runtime_error(a_msg)
    { }

    explicit timeout_error(char const* a_msg) :
        std::runtime_error(a_msg)
    { }
};

}   // namespace ftp
}   // namespace rs

