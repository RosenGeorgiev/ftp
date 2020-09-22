#include <ftp/ftp.hpp>

#include <vector>
#include <thread>
#include <cassert>
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
    -> std::string
    {
        if (!m_socket.is_open())
        {
            assert(false && "reading from socket that is not connected");
            a_ec = std::make_error_code(std::errc::not_connected);
            return {};
        }

        std::vector<char> buf;
        buf.reserve(a_max);

        boost::system::error_code ec;

        m_socket.read_some(boost::asio::buffer(buf), ec);

        CHECK_CONVERT_EC(ec, a_ec);

        return std::string(buf.begin(), buf.end());
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
-> std::string
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

auto client::prepare_ip_port(
    std::string& h1,
    std::string& h2,
    std::string& h3,
    std::string& h4,
    std::string& p1,
    std::string& p2,
    std::error_code& a_ec
) noexcept
-> void
{
    auto ip = resolver::resolve_v4(m_options.data_connection_host, a_ec);

    CHECK_EC_VOID(a_ec);

    static std::regex const ip_regex{R"###((\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3}))###"};
    std::smatch ip_match;

    if (!std::regex_match(ip, ip_match, ip_regex) || ip_match.size() < 4)
    {
        log_debug("Failed to parse IP address!");
        a_ec = std::make_error_code(std::errc::address_not_available);
        return;
    }

    log_debug(ip_match[0]);

    h1 = ip_match[1];
    h2 = ip_match[2];
    h3 = ip_match[3];
    h4 = ip_match[4];

    p1 = std::to_string(m_options.data_connection_port >> 8);
    p2 = std::to_string(m_options.data_connection_port);
}

auto client::download(
    std::string const& a_filename,
    std::error_code& a_ec
) noexcept -> void
{
    std::string h1, h2, h3, h4;
    std::string p1, p2;

    prepare_ip_port(
        h1,
        h2,
        h3,
        h4,
        p1,
        p2,
        a_ec
    );

    CHECK_EC_VOID(a_ec);

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

    CHECK_EC_VOID(a_ec);

    auto response = m_control_connection.read_until(CRLF, a_ec);

    CHECK_EC_VOID(a_ec);

    check_success({reply_code::OK_200}, response, a_ec);

    CHECK_EC_VOID(a_ec);

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
        return;
    }

    response = m_control_connection.read_until(CRLF, a_ec);

    if (a_ec)
    {
        a_ec = ec_future.get();
        log_debug(a_ec.message());
        th.join();
        return;
    }

    check_success({
        reply_code::DATA_CONNECTION_OPEN_TRANSFER_STARTING_125,
        reply_code::FILE_STATUS_OK_OPENING_DATA_CONNECTION_150},
        response, a_ec
    );

    a_ec = ec_future.get();
    th.join();

    CHECK_EC_VOID(a_ec);

    response = m_control_connection.read_until(CRLF, a_ec);
    check_success({
        reply_code::CLOSING_DATA_CONNECTION_226,
        reply_code::FILE_ACTION_COMPLETED_250},
        response, a_ec
    );
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

auto client::noop(std::error_code& a_ec) noexcept -> void
{
    m_control_connection.write(noop_command(), a_ec);
    CHECK_EC_VOID(a_ec);
    auto response = m_control_connection.read_until(CRLF, a_ec);
    check_success({reply_code::OK_200}, response, a_ec);
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

