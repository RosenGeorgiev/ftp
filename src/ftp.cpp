#include <ftp/ftp.hpp>

#include <cassert>
#include <algorithm>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "util.hpp"
#include "logger.hpp"
#include "commands.hpp"


namespace rs
{
namespace ftp
{

struct client::connection::impl
{
    boost::asio::io_context m_io_context;
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::deadline_timer m_timer;
    boost::system::error_code m_ec;
    std::chrono::milliseconds m_timeout;

    impl() :
        m_socket(m_io_context),
        m_timer(m_io_context),
        m_timeout(60000)
    { }

    ~impl() noexcept
    {
        if (is_open())
        {
            try
            {
                close();
            } catch (std::exception const& e)
            {
                logger::error(e.what());
            }
        }
    }

    auto run_event_loop() -> void
    {
        m_io_context.reset();
        m_io_context.run();
    }

    auto handle_error() -> void
    {
        if (m_ec)
        {
            auto ec = m_ec;

            m_ec = boost::system::error_code();

            if (ec == boost::asio::error::timed_out)
            {
                throw timeout_error(ec.message());
            } else if (ec == boost::asio::error::eof)
            {
                throw end_of_file_error(ec.message());
            } else
            {
                throw std::runtime_error(ec.message());
            }
        }
    }

    auto start_timer() -> void
    {
        m_timer.cancel();
        m_timer.expires_from_now(boost::posix_time::milliseconds(m_timeout.count()));
        m_timer.async_wait([this](boost::system::error_code const& a_ec) -> void
        {
            // NOTE - Timer timed out.
            if (!a_ec)
            {
                boost::system::error_code ignored_ec;
                m_socket.cancel(ignored_ec);
                m_ec = boost::asio::error::timed_out;
            } else if (a_ec && a_ec != boost::asio::error::operation_aborted)
            {
                m_ec = a_ec;
            }

            // NOTE - Timer got closed by operation completed in time/error in operation.
        });
    }

    auto connect(
        std::string const& a_hostname,
        int a_port,
        std::chrono::milliseconds const& a_timeout
    )
    -> void
    {
        m_timeout = a_timeout;

        if (a_port < 0 || a_hostname.empty())
        {
            assert(false && "negative port number or empty hostname");
            throw std::invalid_argument("Negative port number or empty hostname");
        }

        if (m_socket.is_open())
        {
            assert(false && "already connected");
            throw std::invalid_argument("Already connected");
        }

        boost::asio::ip::tcp::resolver resolver(m_io_context);
        boost::asio::ip::tcp::resolver::query query(
            a_hostname,
            std::to_string(a_port),
            boost::asio::ip::tcp::resolver::query::numeric_service
        );
        resolver.async_resolve(
            query,
            [this](
                boost::system::error_code const& a_ec,
                boost::asio::ip::tcp::resolver::results_type a_results
            ) -> void
            {
                if (a_ec && a_ec != boost::asio::error::operation_aborted)
                {
                    boost::system::error_code ignored_ec;
                    m_timer.cancel(ignored_ec);
                    m_ec = a_ec;
                    return;
                }

                boost::asio::async_connect(
                    m_socket,
                    a_results,
                    [this](
                        boost::system::error_code const& a_ec,
                        [[ maybe_unused ]] auto& a_endpoint
                    )
                    {
                        boost::system::error_code ignored_ec;
                        m_timer.cancel(ignored_ec);
                        // NOTE - We have an error and it was not a timeout.
                        if (a_ec && a_ec != boost::asio::error::operation_aborted)
                        {
                            m_ec = a_ec;
                        }
                    }
                );
            }
        );

        start_timer();
        run_event_loop();
        handle_error();
    }

    auto close() -> void
    {
        if (!m_socket.is_open())
        {
            logger::error("Closing a non-opened socket!");
            return;
        }

        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_type::shutdown_both);
        m_socket.close();
    }

    auto read(int a_max)
    -> std::vector<char>
    {
        if (!m_socket.is_open())
        {
            throw std::logic_error("Reading from socket that is not connected");
        }

        std::vector<char> buf(a_max);

        m_socket.async_read_some(
            boost::asio::buffer(buf, a_max),
            [this, &buf](
                boost::system::error_code const& a_ec,
                size_t a_bytes_transferred
            ) -> void
            {
                boost::system::error_code ignored_ec;
                m_timer.cancel(ignored_ec);

                if (a_ec && a_ec != boost::asio::error::operation_aborted)
                {
                    m_ec = a_ec;
                    return;
                }

                buf.resize(a_bytes_transferred);
            }
        );

        start_timer();
        run_event_loop();
        handle_error();

        return buf;
    }

    auto read_until(std::string const& a_delimiter)
    -> std::string
    {
        if (!m_socket.is_open())
        {
            throw std::logic_error("Reading from socket that is not connected");
        }

        std::vector<char> buf;

        boost::asio::async_read_until(
            m_socket,
            boost::asio::dynamic_buffer(buf),
            a_delimiter,
            [this](
                boost::system::error_code const& a_ec,
                [[ maybe_unused ]] size_t a_bytes_transferred
            ) -> void
            {
                boost::system::error_code ignored_ec;
                m_timer.cancel(ignored_ec);

                if (a_ec && a_ec != boost::asio::error::operation_aborted)
                {
                    m_ec = a_ec;
                }
            }
        );

        start_timer();
        run_event_loop();
        handle_error();

        return std::string(buf.begin(), buf.end());
    }

    auto read_until(char a_delimiter)
    -> std::string
    {
        if (!m_socket.is_open())
        {
            throw std::logic_error("Reading from socket that is not connected");
        }

        std::vector<char> buf;

        boost::asio::async_read_until(
            m_socket,
            boost::asio::dynamic_buffer(buf),
            a_delimiter,
            [this](
                boost::system::error_code const& a_ec,
                [[ maybe_unused ]] size_t a_bytes_transferred
            ) -> void
            {
                boost::system::error_code ignored_ec;
                m_timer.cancel(ignored_ec);

                if (a_ec && a_ec != boost::asio::error::operation_aborted)
                {
                    m_ec = a_ec;
                }
            }
        );

        start_timer();
        run_event_loop();
        handle_error();

        return std::string(buf.begin(), buf.end());
    }

    auto write(std::string const& a_buf)
    -> void
    {
        if (!m_socket.is_open())
        {
            throw std::logic_error("Writing to socket that is not connected");
        }

        boost::asio::async_write(
            m_socket,
            boost::asio::buffer(a_buf),
            [this](
                boost::system::error_code const& a_ec,
                [[ maybe_unused ]] size_t a_bytes_transferred
            ) -> void
            {
                boost::system::error_code ignored_ec;
                m_timer.cancel(ignored_ec);

                if (a_ec && a_ec != boost::asio::error::operation_aborted)
                {
                    m_ec = a_ec;
                }
            }
        );

        start_timer();
        run_event_loop();
        handle_error();
    }

    auto write(char const* a_buf, int a_buf_size)
    -> void
    {
        if (!m_socket.is_open())
        {
            throw std::logic_error("Writing to socket that is not connected");
        }

        boost::asio::async_write(
            m_socket,
            boost::asio::buffer(a_buf, a_buf_size),
            [this](
                boost::system::error_code const& a_ec,
                [[ maybe_unused ]] size_t a_bytes_transferred
            ) -> void
            {
                boost::system::error_code ignored_ec;
                m_timer.cancel(ignored_ec);

                if (a_ec && a_ec != boost::asio::error::operation_aborted)
                {
                    m_ec = a_ec;
                }
            }
        );

        start_timer();
        run_event_loop();
        handle_error();
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
        try
        {
            close();
        } catch (std::exception const& e)
        {
            logger::error(e.what());
        }
    }
}

auto client::connection::connect(
    std::string const& a_host,
    int a_port,
    std::chrono::milliseconds const& a_timeout
)
-> void
{
    m_impl->connect(a_host, a_port, a_timeout);
}

auto client::connection::close() -> void
{
    m_impl->close();
}

auto client::connection::read(int a_max)
-> std::vector<char>
{
    return m_impl->read(a_max);
}

auto client::connection::read_until(std::string const& a_delimiter)
-> std::string
{
    auto result = m_impl->read_until(a_delimiter);
    logger::debug(result);
    return result;
}

auto client::connection::read_until(char a_delimiter)
-> std::string
{
    auto result = m_impl->read_until(a_delimiter);
    logger::debug(result);
    return result;
}

auto client::connection::write(std::string const& a_buf)
-> void
{
    logger::debug(a_buf);
    m_impl->write(a_buf);
}

auto client::connection::write(char const* a_buf, int a_buf_size)
-> void
{
    m_impl->write(a_buf, a_buf_size);
}

auto client::connection::is_open() noexcept
-> bool
{
    return m_impl->is_open();
}

static auto set_log_level(bool a_debug) -> void
{
    if (a_debug)
    {
        logger::set_log_level(log_level::DEBUG);
    } else
    {
        logger::set_log_level(log_level::ERROR);
    }
}

client::client(connection_options const& a_opts) :
    m_options(a_opts)
{
    assert(!a_opts.server_hostname.empty() && "empty hostname");
    assert(a_opts.server_port > 0 && "negative server port");

    set_log_level(a_opts.debug_output);
}

auto client::set_connection_options(connection_options const& a_opts) noexcept
-> void
{
    m_options = a_opts;
    set_log_level(a_opts.debug_output);
}

auto client::connect()
-> void
{
    m_control_connection.connect(
        m_options.server_hostname,
        m_options.server_port,
        m_options.timeout
    );

    check_success(
        {
            reply_code::OK_200,
            reply_code::READY_FOR_NEW_USER_220
        },
        m_control_connection.read_until(CRLF)
    );
}

auto client::connect(
    std::string const& a_hostname,
    int a_port
)
-> void
{
    m_control_connection.connect(
        a_hostname,
        a_port,
        m_options.timeout
    );

    check_success(
        {
            reply_code::OK_200,
            reply_code::READY_FOR_NEW_USER_220
        },
        m_control_connection.read_until(CRLF)
    );
}

auto client::close()
-> void
{
    if (m_control_connection.is_open())
    {
        m_control_connection.write(quit_command());
        check_success(
            {reply_code::CLOSING_CONTROL_CONNECTION_221},
            m_control_connection.read_until(CRLF)
        );
        m_control_connection.close();
    }
}

auto client::login()
-> void
{
    login(m_options.username, m_options.password);
}

auto client::login(
    std::string const& a_username,
    std::string const& a_password
)
-> void
{
    if (m_options.use_ssl)
    {
        m_control_connection.write(auth_command(authentication_method::TLS));
        check_success(
            {
                reply_code::SECURITY_DATA_EXCHANGE_COMPLETE_234
            },
            m_control_connection.read_until(CRLF)
        );

        m_control_connection.write(pbsz_command(0));
        check_success(
            {
                reply_code::OK_200
            },
            m_control_connection.read_until(CRLF)
        );

        m_control_connection.write(prot_command(data_channel_protection_level::PRIVATE));
        check_success(
            {
                reply_code::OK_200
            },
            m_control_connection.read_until(CRLF)
        );
    }

    m_control_connection.write(user_command(a_username));
    auto codes = parse_codes(m_control_connection.read_until(CRLF));

    if (contains_any_of_codes(codes, {reply_code::USER_LOGGED_IN_230, reply_code::USER_LOGGED_IN_232}))
    {
        return;
    }

    if (!contains_any_of_codes(codes, {reply_code::USERNAME_OK_NEED_PASSWORD_331}))
    {
        throw std::runtime_error("No reply codes matched - operation failed");
    }

    m_control_connection.write(password_command(a_password));
    check_success(
        {
            reply_code::USER_LOGGED_IN_230,
            reply_code::USER_LOGGED_IN_232
        },
        m_control_connection.read_until(CRLF)
    );
}

auto client::cwd(std::string const& a_new_wd)
-> void
{
    m_control_connection.write(cwd_command(a_new_wd));
    check_success(
        {reply_code::FILE_ACTION_COMPLETED_250},
        m_control_connection.read_until(CRLF)
    );
}

auto client::cdup()
-> void
{
    m_control_connection.write(cdup_command());
    check_success(
        {
            reply_code::OK_200,
            reply_code::FILE_ACTION_COMPLETED_250
        },
        m_control_connection.read_until(CRLF)
    );
}

auto client::download(std::string const& a_filename)
-> std::vector<char>
{
    std::vector<char> ret_data;

    auto data_callback = [&ret_data](std::vector<char> const& a_data) -> void
    {
        std::copy(a_data.begin(), a_data.end(), std::back_inserter(ret_data));
    };

    download_passive(a_filename, data_callback);

    return ret_data;
}

auto client::download(
    std::string const& a_filename,
    std::ofstream& a_ofstream
)
-> void
{
    auto data_callback = [&a_ofstream](std::vector<char> const& a_data) -> void
    {
        a_ofstream.write(reinterpret_cast<char const*>(a_data.data()), a_data.size());
    };

    download_passive(a_filename, data_callback);
}

auto client::upload(
    std::string const& a_filename,
    std::istream& a_istream
)
-> void
{
    connection data_transfer_connection;
    enter_passive_mode(data_transfer_connection);
    m_control_connection.write(stor_command(a_filename));
    check_success(
        {
            reply_code::DATA_CONNECTION_OPEN_TRANSFER_STARTING_125,
            reply_code::FILE_STATUS_OK_OPENING_DATA_CONNECTION_150
        },
        m_control_connection.read_until(CRLF)
    );

    char buf[8192];
    while (!a_istream.eof())
    {
        a_istream.read(buf, 8192);
        data_transfer_connection.write(buf, a_istream.gcount());
    }

    data_transfer_connection.close();

    check_success(
        {
            reply_code::CLOSING_DATA_CONNECTION_226,
            reply_code::FILE_ACTION_COMPLETED_250
        },
        m_control_connection.read_until(CRLF)
    );
}

auto client::rename(
    std::string const& a_file_to_rename,
    std::string const& a_rename_to
)
-> void
{
    m_control_connection.write(rnfr_command(a_file_to_rename));
    check_success(
        {reply_code::REQUESTED_FILE_ACTION_INFO_PENDING_350},
        m_control_connection.read_until(CRLF)
    );
    m_control_connection.write(rnto_command(a_rename_to));
    check_success(
        {reply_code::FILE_ACTION_COMPLETED_250},
        m_control_connection.read_until(CRLF)
    );
}

auto client::remove_file(std::string const& a_filepath)
-> void
{
    m_control_connection.write(dele_command(a_filepath));
    check_success(
        {reply_code::FILE_ACTION_COMPLETED_250},
        m_control_connection.read_until(CRLF)
    );
}

auto client::rmdir(std::string const& a_dirpath)
-> void
{
    m_control_connection.write(rmd_command(a_dirpath));
    check_success(
        {reply_code::FILE_ACTION_COMPLETED_250},
        m_control_connection.read_until(CRLF)
    );
}

auto client::mkdir(std::string const& a_dirpath)
-> void
{
    m_control_connection.write(mkd_command(a_dirpath));
    check_success(
        {reply_code::PATHNAME_CREATED_257},
        m_control_connection.read_until(CRLF)
    );
}

auto client::pwd()
-> std::string
{
    m_control_connection.write(pwd_command());
    auto response = m_control_connection.read_until(CRLF);
    check_success(
        {reply_code::PATHNAME_CREATED_257},
        response
    );

    if (response.size() > 4)
    {
        auto ret{response.substr(4)};
        logger::debug(ret);
        return ret;
    }

    throw std::length_error("Server returned malformed response");
}

auto client::ls()
-> std::string
{
    connection data_transfer_connection;
    enter_passive_mode(data_transfer_connection);
    m_control_connection.write(nlst_command());
    check_success(
        {
            reply_code::DATA_CONNECTION_OPEN_TRANSFER_STARTING_125,
            reply_code::FILE_STATUS_OK_OPENING_DATA_CONNECTION_150
        },
        m_control_connection.read_until(CRLF)
    );
    auto response = data_transfer_connection.read_until(CRLF);
    check_success(
        {
            reply_code::CLOSING_DATA_CONNECTION_226,
            reply_code::FILE_ACTION_COMPLETED_250
        },
        m_control_connection.read_until(CRLF)
    );
    return response;
}

auto client::ls(std::string const& a_pathname)
-> std::string
{
    connection data_transfer_connection;
    enter_passive_mode(data_transfer_connection);
    m_control_connection.write(nlst_command(a_pathname));
    check_success(
        {
            reply_code::DATA_CONNECTION_OPEN_TRANSFER_STARTING_125,
            reply_code::FILE_STATUS_OK_OPENING_DATA_CONNECTION_150
        },
        m_control_connection.read_until(CRLF)
    );
    auto response = data_transfer_connection.read_until(CRLF);
    check_success(
        {
            reply_code::CLOSING_DATA_CONNECTION_226,
            reply_code::FILE_ACTION_COMPLETED_250
        },
        m_control_connection.read_until(CRLF)
    );
    return response;
}

auto client::system_info()
-> std::string
{
    m_control_connection.write(syst_command());
    auto response = m_control_connection.read_until(CRLF);
    check_success(
        {reply_code::X_SYSTEM_TYPE_215},
        response
    );

    if (response.size() > 4)
    {
        auto ret{response.substr(4)};
        logger::debug(ret);
        return ret;
    }

    throw std::length_error("Server returned malformed response");
}

// TODO - Validate server response
auto client::progress()
-> std::string
{
    m_control_connection.write(stat_command());
    auto response = m_control_connection.read_until(CRLF);
    check_success(
        {
            reply_code::DIRECTORY_STATUS_212,
            reply_code::FILE_STATUS_213
        },
        response
    );

    if (response.size() > 4)
    {
        auto ret{response.substr(4)};
        logger::debug(ret);
        return ret;
    }

    throw std::length_error("Server returned malformed response");
}

auto client::noop()
-> void
{
    m_control_connection.write(noop_command());
    check_success(
        {reply_code::OK_200},
        m_control_connection.read_until(CRLF)
    );
}

auto client::download_passive(
    std::string const& a_filename,
    std::function<void(std::vector<char> const&)> a_data_callback
)
-> void
{

    connection data_transfer_connection;
    enter_passive_mode(data_transfer_connection);
    m_control_connection.write(retr_command(a_filename));
    check_success(
        {
            reply_code::DATA_CONNECTION_OPEN_TRANSFER_STARTING_125,
            reply_code::FILE_STATUS_OK_OPENING_DATA_CONNECTION_150
        },
        m_control_connection.read_until(CRLF)
    );

    // TODO - Handle more transfer modes - default stream.
    // NOTE - Default transfer mode - STREAM. When the server closes the connection - the transfer
    //        is done.
    while (true)
    {
        try
        {
            a_data_callback(data_transfer_connection.read(65536));
        } catch (end_of_file_error const& e)
        {
            break;
        }
    }

    check_success(
        {reply_code::CLOSING_DATA_CONNECTION_226},
        m_control_connection.read_until(CRLF)
    );
}

auto client::enter_passive_mode(connection& a_data_transfer_connection)
-> void
{
    m_control_connection.write(pasv_command());
    auto response = m_control_connection.read_until(CRLF);
    check_success(
        {
            reply_code::OK_200,
            reply_code::ENTERING_PASSIVE_MODE_227
        },
        response);
    auto [ip_vec, port] = parse_pasv_ipv4_port_reply(response);
    a_data_transfer_connection.connect(
        ipv4_vec_to_str(ip_vec),
        port,
        m_options.timeout
    );
}

}   // namespace ftp
}   // namespace rs

