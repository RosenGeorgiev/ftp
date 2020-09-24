#include <ftp/ftp.hpp>

#include <thread>
#include <cassert>
#include <algorithm>

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
            try
            {
                close();
            } catch (std::exception const& e)
            {
                log_debug(e.what());
            }
        }
    }

    auto connect(
        std::string const& a_hostname,
        int a_port
    )
    -> void
    {
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
        auto endpoints = resolver.resolve(a_hostname, std::to_string(a_port));
        boost::asio::connect(m_socket, endpoints);
    }

    auto accept_v4(int a_port)
    -> void
    {
        boost::asio::ip::tcp::acceptor acceptor(
            m_io_context,
            boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), a_port)
        );

        acceptor.accept(m_socket);
    }

    auto close() -> void
    {
        if (!m_socket.is_open())
        {
            log_debug("Closing a non-opened socket!");
            return;
        }

        m_socket.close();
    }

    auto read(int a_max)
    -> std::vector<char>
    {
        if (!m_socket.is_open())
        {
            throw std::logic_error("Reading from socket that is not connected");
        }

        try
        {
            std::vector<char> buf(a_max);
            m_socket.read_some(boost::asio::buffer(buf));
            return buf;
        } catch (boost::system::system_error const& e)
        {
            if (e.code() == boost::asio::error::eof)
            {
                throw end_of_file_error("Connection reset by peer");
            }

            throw;
        }
    }

    auto read_until(std::string const& a_delimiter)
    -> std::string
    {
        if (!m_socket.is_open())
        {
            throw std::logic_error("Reading from socket that is not connected");
        }

        std::vector<char> buf;
        boost::asio::read_until(
            m_socket,
            boost::asio::dynamic_buffer(buf),
            a_delimiter
        );
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
        boost::asio::read_until(
            m_socket,
            boost::asio::dynamic_buffer(buf),
            a_delimiter
        );
        return std::string(buf.begin(), buf.end());
    }

    auto write(std::string const& a_buf)
    -> void
    {
        if (!m_socket.is_open())
        {
            throw std::logic_error("Writing to socket that is not connected");
        }

        m_socket.write_some(boost::asio::buffer(a_buf));
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
            log_debug(e.what());
        }
    }
}

auto client::connection::connect(
    std::string const& a_host,
    int a_port
)
-> void
{
    m_impl->connect(a_host, a_port);
}

auto client::connection::accept_v4(int a_port)
-> void
{
    m_impl->accept_v4(a_port);
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
    log_debug(result);
    return result;
}

auto client::connection::read_until(char a_delimiter)
-> std::string
{
    auto result = m_impl->read_until(a_delimiter);
    log_debug(result);
    return result;
}

auto client::connection::write(std::string const& a_buf)
-> void
{
    log_debug(a_buf);
    m_impl->write(a_buf);
}

auto client::connection::is_open() noexcept
-> bool
{
    return m_impl->is_open();
}

auto client::resolver::resolve_v4(std::string const& a_hostname)
-> std::string
{
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(a_hostname, "ftp");

    for (auto it = endpoints.begin(); it != endpoints.end(); ++it)
    {
        if (it->endpoint().address().is_v4())
        {
            return it->endpoint().address().to_string();
        }
    }

    throw std::runtime_error("Failed to resolve address");
}

client::client(connection_options const& a_opts) :
    m_options(a_opts)
{
    assert(!a_opts.server_hostname.empty() && "empty hostname");
    assert(a_opts.server_port > 0 && "negative server port");
}

auto client::set_connection_options(connection_options const& a_opts) noexcept
-> void
{
    m_options = a_opts;
}

auto client::connect()
-> void
{
    m_control_connection.connect(
        m_options.server_hostname,
        m_options.server_port
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
    m_control_connection.write(user_command(m_options.username));
    check_success(
        {
            reply_code::USER_LOGGED_IN_230,
            reply_code::USERNAME_OK_NEED_PASSWORD_331,
            reply_code::NEED_ACCOUNT_332
        },
        m_control_connection.read_until(CRLF)
    );
    m_control_connection.write(password_command(m_options.password));
    check_success(
        {reply_code::USER_LOGGED_IN_230},
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

auto client::smnt(std::string const& a_mount_point)
-> void
{
    m_control_connection.write(smnt_command(a_mount_point));
    check_success(
        {
            reply_code::NOT_IMPLEMENTED_202,
            reply_code::FILE_ACTION_COMPLETED_250
        },
        m_control_connection.read_until(CRLF)
    );
}

auto client::logout()
-> void
{
    m_control_connection.write(rein_command());
    check_success(
        {reply_code::READY_FOR_NEW_USER_220},
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

    if (m_options.passive_mode)
    {
        download_passive(a_filename, data_callback);
        return ret_data;
    } else
    {
        download_active(a_filename, data_callback);
        return ret_data;
    }
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

    if (m_options.passive_mode)
    {
        download_passive(a_filename, data_callback);
    } else
    {
        download_active(a_filename, data_callback);
    }
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
        log_debug(ret);
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
        log_debug(ret);
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
        log_debug(ret);
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

auto client::download_active(
    std::string const& a_filename,
    std::function<void(std::vector<char> const&)> a_data_callback
)
-> void
{
    auto ip = resolver::resolve_v4(m_options.data_connection_host);
    auto ip_nums = parse_ipv4(ip);

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
        )
    );

    check_success(
        {reply_code::OK_200},
        m_control_connection.read_until(CRLF)
    );

    // TODO - Handle exceptions thrown here.
    auto accept_and_read = [&a_data_callback, this]() -> void
    {
        connection data_connection;
        data_connection.accept_v4(m_options.data_connection_port);
        // FIXME - Read according to the transmission type. Currently only default STREAM supported.
        while (true)
        {
            data_connection.read(65536);
        }
    };

    std::thread th(accept_and_read);

    try
    {
        m_control_connection.write(retr_command(a_filename));

        check_success(
            {
                reply_code::DATA_CONNECTION_OPEN_TRANSFER_STARTING_125,
                reply_code::FILE_STATUS_OK_OPENING_DATA_CONNECTION_150
            },

            m_control_connection.read_until(CRLF)
        );

        th.join();
    } catch (std::exception const& e)
    {
        log_debug(e.what());
        if (th.joinable())
        {
            th.join();
        }
    }

    check_success(
        {
            reply_code::CLOSING_DATA_CONNECTION_226,
            reply_code::FILE_ACTION_COMPLETED_250
        },
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
    a_data_transfer_connection.connect(ipv4_vec_to_str(ip_vec), port);
}

}   // namespace ftp
}   // namespace rs

