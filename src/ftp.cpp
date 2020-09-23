#include <ftp/ftp.hpp>

#include <thread>
#include <cassert>
#include <algorithm>
#include <system_error>

#include <boost/asio.hpp>

#include "util.hpp"


namespace rs
{
namespace ftp
{

struct client::connection::impl
{
    boost::asio::io_context m_io_context;
    boost::asio::ip::tcp::socket m_socket;

    impl() :
        m_socket(m_io_context)
    { }

    ~impl() noexcept
    {
        if (is_open())
        {
            std::error_code ec;
            close(ec);
        }
    }

    auto connect(
        std::string const& a_hostname,
        int a_port,
        std::error_code& a_ec
    ) noexcept
    -> void
    {
        if (a_port < 0 || a_hostname.empty())
        {
            assert(false && "negative port number or empty hostname");
            a_ec = std::make_error_code(std::errc::invalid_argument);
            return;
        }

        if (m_socket.is_open())
        {
            assert(false && "already connected");
            a_ec = std::make_error_code(std::errc::already_connected);
            return;
        }

        boost::asio::ip::tcp::resolver resolver(m_io_context);
        boost::system::error_code ec;
        auto endpoints = resolver.resolve(a_hostname, std::to_string(a_port), ec);

        CHECK_CONVERT_EC_VOID(ec, a_ec);

        boost::asio::connect(m_socket, endpoints, ec);

        CHECK_CONVERT_EC_VOID(ec, a_ec);
    }

    auto accept_v4(
        int a_port,
        std::error_code& a_ec
    ) noexcept
    -> void
    {
        try
        {
            boost::asio::ip::tcp::acceptor acceptor(
                m_io_context,
                boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), a_port)
            );

            acceptor.accept(m_socket);
        } catch (std::exception const& e)
        {
            log_debug(e.what());
            a_ec = std::make_error_code(std::errc::operation_not_permitted);
        }
    }

    auto close(std::error_code& a_ec) noexcept -> void
    {
        if (!m_socket.is_open())
        {
            log_debug("Closing a non-opened socket!");
            return;
        }

        boost::system::error_code ec;
        m_socket.close(ec);

        CHECK_CONVERT_EC_VOID(ec, a_ec);
    }

    auto read(
        int a_max,
        std::error_code& a_ec
    ) noexcept
    -> std::vector<char>
    {
        if (!m_socket.is_open())
        {
            assert(false && "reading from socket that is not connected");
            a_ec = std::make_error_code(std::errc::not_connected);
            return {};
        }

        std::vector<char> buf(a_max);

        boost::system::error_code ec;

        m_socket.read_some(boost::asio::buffer(buf), ec);

        CHECK_CONVERT_EC(ec, a_ec);

        return buf;
    }

    auto read_until(
        std::string const& a_delimiter,
        std::error_code& a_ec
    ) noexcept
    -> std::string
    {
        if (!m_socket.is_open())
        {
            assert(false && "reading from socket that is not connected");
            a_ec = std::make_error_code(std::errc::not_connected);
            return {};
        }

        std::vector<char> buf;

        boost::system::error_code ec;

        boost::asio::read_until(
            m_socket,
            boost::asio::dynamic_buffer(buf),
            a_delimiter,
            ec
        );

        CHECK_CONVERT_EC(ec, a_ec);

        return std::string(buf.begin(), buf.end());
    }

    auto read_until(
        char a_delimiter,
        std::error_code& a_ec
    ) noexcept
    -> std::string
    {
        if (!m_socket.is_open())
        {
            assert(false && "reading from socket that is not connected");
            a_ec = std::make_error_code(std::errc::not_connected);
            return {};
        }

        std::vector<char> buf;

        boost::system::error_code ec;

        boost::asio::read_until(
            m_socket,
            boost::asio::dynamic_buffer(buf),
            a_delimiter,
            ec
        );

        CHECK_CONVERT_EC(ec, a_ec);

        return std::string(buf.begin(), buf.end());
    }

    auto write(
        std::string const& a_buf,
        std::error_code& a_ec
    ) noexcept -> void
    {
        if (!m_socket.is_open())
        {
            assert(false && "writing to socket that is not connected");
            a_ec = std::make_error_code(std::errc::not_connected);
            return;
        }

        boost::system::error_code ec;
        m_socket.write_some(boost::asio::buffer(a_buf), ec);

        CHECK_CONVERT_EC_VOID(ec, a_ec);
    }

    auto is_open() noexcept -> bool
    {
        return m_socket.is_open();
    }
};

client::connection::connection() :
    m_impl(std::make_unique<client::connection::impl>())
{ }

client::connection::~connection() noexcept
{
    if (is_open())
    {
        std::error_code ec;
        close(ec);
    }
}

auto client::connection::connect(
    std::string const& a_host,
    int a_port,
    std::error_code& a_ec
) noexcept
-> void
{
    m_impl->connect(a_host, a_port, a_ec);
}

auto client::connection::accept_v4(
    int a_port,
    std::error_code& a_ec
) noexcept -> void
{
    m_impl->accept_v4(a_port, a_ec);
}

auto client::connection::close(std::error_code& a_ec) noexcept -> void
{
    m_impl->close(a_ec);
}

auto client::connection::read(
    int a_max,
    std::error_code& a_ec
) noexcept
-> std::vector<char>
{
    return m_impl->read(a_max, a_ec);
}

auto client::connection::read_until(
    std::string const& a_delimiter,
    std::error_code& a_ec
) noexcept
-> std::string
{
    auto result = m_impl->read_until(a_delimiter, a_ec);
    log_debug(result);
    return result;
}

auto client::connection::read_until(
    char a_delimiter,
    std::error_code& a_ec
) noexcept
-> std::string
{
    auto result = m_impl->read_until(a_delimiter, a_ec);
    log_debug(result);
    return result;
}

auto client::connection::write(
    std::string const& a_buf,
    std::error_code& a_ec
) noexcept -> void
{
    log_debug(a_buf);
    m_impl->write(a_buf, a_ec);
}

auto client::connection::is_open() noexcept -> bool
{
    return m_impl->is_open();
}

auto client::resolver::resolve_v4(
    std::string const& a_hostname,
    std::error_code& a_ec
) noexcept
-> std::string
{
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::system::error_code ec;

    auto endpoints = resolver.resolve(a_hostname, "ftp", ec);

    CHECK_CONVERT_EC(ec, a_ec);

    for (auto it = endpoints.begin(); it != endpoints.end(); ++it)
    {
        if (it->endpoint().address().is_v4())
        {
            return it->endpoint().address().to_string();
        }
    }

    a_ec = std::make_error_code(std::errc::address_not_available);
    return {};
}

client::client(connection_options const& a_opts) :
    m_options(a_opts)
{
    assert(!a_opts.server_hostname.empty() && "empty hostname");
    assert(a_opts.server_port > 0 && "negative server port");
}

auto client::set_connection_options(connection_options const& a_opts) noexcept -> void
{
    m_options = a_opts;
}

auto client::connect(std::error_code& a_ec) noexcept -> void
{
    m_control_connection.connect(m_options.server_hostname, m_options.server_port, a_ec);
    CHECK_EC_VOID(a_ec);
    auto response = m_control_connection.read_until(CRLF, a_ec);
    CHECK_EC_VOID(a_ec);
    check_success({reply_code::OK_200, reply_code::READY_FOR_NEW_USER_220}, response, a_ec);
}

auto client::close(std::error_code& a_ec) noexcept -> void
{
    if (m_control_connection.is_open())
    {
        m_control_connection.write(quit_command(), a_ec);
        CHECK_EC_VOID(a_ec);
        auto response = m_control_connection.read_until(CRLF, a_ec);
        CHECK_EC_VOID(a_ec);
        check_success({reply_code::CLOSING_CONTROL_CONNECTION_221}, response, a_ec);
        m_control_connection.close(a_ec);
    }
}

auto client::login(std::error_code& a_ec) noexcept -> void
{
    m_control_connection.write(user_command(m_options.username), a_ec);
    CHECK_EC_VOID(a_ec);
    auto response = m_control_connection.read_until(CRLF, a_ec);
    CHECK_EC_VOID(a_ec);
    check_success({
        reply_code::USER_LOGGED_IN_230,
        reply_code::USERNAME_OK_NEED_PASSWORD_331,
        reply_code::NEED_ACCOUNT_332},
        response, a_ec
    );
    CHECK_EC_VOID(a_ec);
    m_control_connection.write(password_command(m_options.password), a_ec);
    CHECK_EC_VOID(a_ec);
    response = m_control_connection.read_until(CRLF, a_ec);
    check_success({reply_code::USER_LOGGED_IN_230}, response, a_ec);
}

auto client::cwd(std::string const& a_new_wd, std::error_code& a_ec) noexcept -> void
{
    m_control_connection.write(cwd_command(a_new_wd), a_ec);
    CHECK_EC_VOID(a_ec);
    auto response = m_control_connection.read_until(CRLF, a_ec);
    CHECK_EC_VOID(a_ec);
    check_success({reply_code::FILE_ACTION_COMPLETED_250}, response, a_ec);
}

auto client::cdup(std::error_code& a_ec) noexcept -> void
{
    m_control_connection.write(cdup_command(), a_ec);
    CHECK_EC_VOID(a_ec);
    auto response = m_control_connection.read_until(CRLF, a_ec);
    CHECK_EC_VOID(a_ec);
    check_success({reply_code::OK_200, reply_code::FILE_ACTION_COMPLETED_250}, response, a_ec);
}

auto client::smnt(std::string const& a_mount_point, std::error_code& a_ec) noexcept -> void
{
    m_control_connection.write(smnt_command(a_mount_point), a_ec);
    CHECK_EC_VOID(a_ec);
    auto response = m_control_connection.read_until(CRLF, a_ec);
    CHECK_EC_VOID(a_ec);
    check_success({reply_code::NOT_IMPLEMENTED_202, reply_code::FILE_ACTION_COMPLETED_250}, response, a_ec);
}

auto client::logout(std::error_code& a_ec) noexcept -> void
{
    m_control_connection.write(rein_command(), a_ec);
    CHECK_EC_VOID(a_ec);
    auto response = m_control_connection.read_until(CRLF, a_ec);
    CHECK_EC_VOID(a_ec);
    check_success({reply_code::READY_FOR_NEW_USER_220}, response, a_ec);
}

auto client::download(
    std::string const& a_filename,
    std::error_code& a_ec
) noexcept -> std::vector<char>
{
    if (m_options.passive_mode)
    {
        return download_active(a_filename, a_ec);
    } else
    {
        return download_passive(a_filename, a_ec);
    }
}

auto client::rename(
    std::string const& a_file_to_rename,
    std::string const& a_rename_to,
    std::error_code& a_ec
) noexcept
-> void
{
    m_control_connection.write(rnfr_command(a_file_to_rename), a_ec);

    CHECK_EC_VOID(a_ec);

    auto response = m_control_connection.read_until(CRLF, a_ec);

    CHECK_EC_VOID(a_ec);

    check_success({reply_code::REQUESTED_FILE_ACTION_INFO_PENDING_350}, response, a_ec);

    m_control_connection.write(rnto_command(a_rename_to), a_ec);

    CHECK_EC_VOID(a_ec);

    response = m_control_connection.read_until(CRLF, a_ec);

    CHECK_EC_VOID(a_ec);

    check_success({reply_code::FILE_ACTION_COMPLETED_250}, response, a_ec);
}

auto client::remove_file(
    std::string const& a_filepath,
    std::error_code& a_ec
) noexcept
-> void
{
    m_control_connection.write(dele_command(a_filepath), a_ec);

    CHECK_EC_VOID(a_ec);

    auto response = m_control_connection.read_until(CRLF, a_ec);

    CHECK_EC_VOID(a_ec);

    check_success({reply_code::FILE_ACTION_COMPLETED_250}, response, a_ec);
}

auto client::rmdir(
    std::string const& a_dirpath,
    std::error_code& a_ec
) noexcept
-> void
{
    m_control_connection.write(rmd_command(a_dirpath), a_ec);

    CHECK_EC_VOID(a_ec);

    auto response = m_control_connection.read_until(CRLF, a_ec);

    CHECK_EC_VOID(a_ec);

    check_success({reply_code::FILE_ACTION_COMPLETED_250}, response, a_ec);
}

auto client::mkdir(
    std::string const& a_dirpath,
    std::error_code& a_ec
) noexcept
-> void
{
    m_control_connection.write(mkd_command(a_dirpath), a_ec);

    CHECK_EC_VOID(a_ec);

    auto response = m_control_connection.read_until(CRLF, a_ec);

    CHECK_EC_VOID(a_ec);

    check_success({reply_code::PATHNAME_CREATED_257}, response, a_ec);
}

auto client::pwd(std::error_code& a_ec) noexcept -> std::string
{
    m_control_connection.write(pwd_command(), a_ec);

    CHECK_EC(a_ec);

    auto response = m_control_connection.read_until(CRLF, a_ec);

    CHECK_EC(a_ec);

    check_success({reply_code::PATHNAME_CREATED_257}, response, a_ec);

    CHECK_EC(a_ec);

    if (response.size() > 4)
    {
        auto ret{response.substr(4)};
        log_debug(ret);
        return ret;
    }

    log_debug("Server returned malformed response");
    a_ec = std::make_error_code(std::errc:: bad_message);

    return {};
}

// TODO - Finish
// TODO - Read on the DTP
auto client::ls(std::error_code& a_ec) noexcept -> std::string
{
    m_control_connection.write(nlst_command(), a_ec);

    CHECK_EC(a_ec);

    auto response = m_control_connection.read_until(CRLF, a_ec);

    CHECK_EC(a_ec);

    return {};
}

// TODO - Finish
// TODO - Read on the DTP
auto client::ls(std::string const& a_pathname, std::error_code& a_ec) noexcept -> std::string
{
    m_control_connection.write(stat_command(a_pathname), a_ec);

    CHECK_EC(a_ec);

    auto response = m_control_connection.read_until(CRLF, a_ec);

    CHECK_EC(a_ec);

    return {};
}

auto client::system_info(std::error_code& a_ec) noexcept -> std::string
{
    m_control_connection.write(syst_command(), a_ec);

    CHECK_EC(a_ec);

    auto response = m_control_connection.read_until(CRLF, a_ec);

    CHECK_EC(a_ec);

    check_success({reply_code::X_SYSTEM_TYPE_215}, response, a_ec);

    CHECK_EC(a_ec);

    if (response.size() > 4)
    {
        auto ret{response.substr(4)};
        log_debug(ret);
        return ret;
    }

    log_debug("Server returned malformed response");
    a_ec = std::make_error_code(std::errc:: bad_message);

    return {};
}

// TODO - Validate server response
auto client::progress(std::error_code& a_ec) noexcept -> std::string
{
    m_control_connection.write(stat_command(), a_ec);

    CHECK_EC(a_ec);

    auto response = m_control_connection.read_until(CRLF, a_ec);

    CHECK_EC(a_ec);

    check_success({
            reply_code::DIRECTORY_STATUS_212,
            reply_code::FILE_STATUS_213
        },
        response, a_ec
    );

    if (response.size() > 4)
    {
        auto ret{response.substr(4)};
        log_debug(ret);
        return ret;
    }

    log_debug("Server returned malformed response");
    a_ec = std::make_error_code(std::errc:: bad_message);

    return {};
}

auto client::noop(std::error_code& a_ec) noexcept -> void
{
    m_control_connection.write(noop_command(), a_ec);
    CHECK_EC_VOID(a_ec);
    auto response = m_control_connection.read_until(CRLF, a_ec);
    check_success({reply_code::OK_200}, response, a_ec);
}

auto client::download_active(
    std::string const& a_filename,
    std::error_code& a_ec
) noexcept
-> std::vector<char>
{
    m_control_connection.write(pasv_command(), a_ec);

    CHECK_EC(a_ec);

    auto response = m_control_connection.read_until(CRLF, a_ec);

    CHECK_EC(a_ec);

    check_success({reply_code::OK_200, reply_code::ENTERING_PASSIVE_MODE_227}, response, a_ec);

    CHECK_EC(a_ec);

    auto [ip_vec, port] = parse_pasv_ipv4_port_reply(response, a_ec);

    CHECK_EC(a_ec);

    connection data_transfer_connection;
    data_transfer_connection.connect(ipv4_vec_to_str(ip_vec), port, a_ec);

    CHECK_EC(a_ec);

    m_control_connection.write(retr_command(a_filename), a_ec);

    CHECK_EC(a_ec);

    response = m_control_connection.read_until(CRLF, a_ec);

    CHECK_EC(a_ec);

    check_success({
            reply_code::DATA_CONNECTION_OPEN_TRANSFER_STARTING_125,
            reply_code::FILE_STATUS_OK_OPENING_DATA_CONNECTION_150
        },
        response, a_ec
    );

    CHECK_EC(a_ec);

    std::vector<char> file_buf;
    // TODO - Handle more transfer modes - default stream. When the server closes the
    //        connection - the transfer is done.
    while (true)
    {
        auto data = data_transfer_connection.read(65536, a_ec);

        if (a_ec)
        {
            break;
        }

        std::copy(data.begin(), data.end(), std::back_inserter(file_buf));
    }

    if (a_ec && a_ec != std::errc::no_such_file_or_directory)
    {
        log_debug(a_ec.message());
        return {};
    } else  // NOTE - Probably caused by the server closing the connection - ignore.
    {
        a_ec = std::error_code{};
    }

    response = m_control_connection.read_until(CRLF, a_ec);

    CHECK_EC(a_ec);

    check_success({reply_code::CLOSING_DATA_CONNECTION_226}, response, a_ec);

    CHECK_EC(a_ec);

    return file_buf;
}

auto client::download_passive(
    std::string const& a_filename,
    std::error_code& a_ec
) noexcept
-> std::vector<char>
{
    auto ip = resolver::resolve_v4(m_options.data_connection_host, a_ec);

    CHECK_EC(a_ec);

    auto ip_nums = parse_ipv4(ip, a_ec);

    CHECK_EC(a_ec);

    std::string h1 = std::to_string(ip_nums[0]);
    std::string h2 = std::to_string(ip_nums[1]);
    std::string h3 = std::to_string(ip_nums[2]);
    std::string h4 = std::to_string(ip_nums[3]);

    auto network_port_repr = port_to_network(m_options.data_connection_port);

    std::string p1 = std::to_string(std::get<0>(network_port_repr));
    std::string p2 = std::to_string(std::get<1>(network_port_repr));

    m_control_connection.write(
        port_command(
            h1,
            h2,
            h3,
            h4,
            p1,
            p2
        ),
        a_ec
    );

    CHECK_EC(a_ec);

    auto response = m_control_connection.read_until(CRLF, a_ec);

    CHECK_EC(a_ec);

    check_success({reply_code::OK_200}, response, a_ec);

    CHECK_EC(a_ec);

    std::promise<std::error_code> ec_promise;
    std::future<std::error_code> ec_future = ec_promise.get_future();
    std::thread th(&client::accept_and_read, this, std::move(ec_promise));
    m_control_connection.write(retr_command(a_filename), a_ec);

    // FIXME - If this fails, the accepting thread will just hang and we have no way to stop it.
    if (a_ec)
    {
        a_ec = ec_future.get();
        log_debug(a_ec.message());
        th.join();
        return {};
    }

    response = m_control_connection.read_until(CRLF, a_ec);

    if (a_ec)
    {
        a_ec = ec_future.get();
        log_debug(a_ec.message());
        th.join();
        return {};
    }

    check_success({
        reply_code::DATA_CONNECTION_OPEN_TRANSFER_STARTING_125,
        reply_code::FILE_STATUS_OK_OPENING_DATA_CONNECTION_150},
        response, a_ec
    );

    a_ec = ec_future.get();
    th.join();

    CHECK_EC(a_ec);

    response = m_control_connection.read_until(CRLF, a_ec);
    check_success({
        reply_code::CLOSING_DATA_CONNECTION_226,
        reply_code::FILE_ACTION_COMPLETED_250},
        response, a_ec
    );

    return {};
}

auto client::accept_and_read(std::promise<std::error_code> a_ec) noexcept -> void
{
    connection data_connection;
    std::error_code ec;
    data_connection.accept_v4(m_options.data_connection_port, ec);

    if (ec)
    {
        a_ec.set_value(ec);
        return;
    }

    data_connection.read_until(EOF, ec);
    a_ec.set_value(ec);
}

}   // namespace ftp
}   // namespace rs

