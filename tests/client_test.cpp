#include <catch2/catch.hpp>

#include <system_error>

#include <ftp/ftp.hpp>


class logged_in_fixture
{
public:
    logged_in_fixture()
    {
        rs::ftp::connection_options opts;
        opts.username = "admin";
        opts.password = "admin";
        opts.server_hostname = "localhost";
        opts.server_port = 21;
        opts.data_connection_port = 50000;
        opts.debug_output = true;

        m_client.set_connection_options(opts);

        std::error_code ec;
        m_client.connect(ec);

        REQUIRE_FALSE(ec);

        m_client.login(ec);

        REQUIRE_FALSE(ec);
    }

protected:
    rs::ftp::client m_client;
};


TEST_CASE("Connection test", "[ftp][connect]")
{
    SECTION("Successful connection")
    {
        rs::ftp::connection_options opts;
        opts.server_hostname = "localhost";
        opts.server_port = 21;
        opts.debug_output = true;

        rs::ftp::client client(opts);
        std::error_code ec;
        client.connect(ec);

        REQUIRE_FALSE(ec);

        SECTION("Close")
        {
            client.close(ec);
            REQUIRE_FALSE(ec);
        }
    }

    SECTION("Unsuccessful connection")
    {
        rs::ftp::connection_options opts;
        opts.server_hostname = "localhost";
        opts.server_port = 3333;
        opts.debug_output = true;

        rs::ftp::client client(opts);
        std::error_code ec;
        client.connect(ec);

        REQUIRE(ec);
    }
}

TEST_CASE("Login test", "[ftp][login]")
{
    rs::ftp::connection_options opts;
    opts.server_hostname = "localhost";
    opts.server_port = 21;
    opts.debug_output = true;

    SECTION("Successful login")
    {
        opts.username = "admin";
        opts.password = "admin";

        rs::ftp::client client(opts);
        std::error_code ec;
        client.connect(ec);

        REQUIRE_FALSE(ec);

        client.login(ec);

        REQUIRE_FALSE(ec);
    }

    SECTION("Failed login")
    {
        opts.username = "";
        opts.password = "";

        rs::ftp::client client(opts);
        std::error_code ec;
        client.connect(ec);

        REQUIRE_FALSE(ec);

        client.login(ec);

        REQUIRE(ec);
    }
}

TEST_CASE_METHOD(logged_in_fixture, "NOOP test", "[ftp][noop]")
{
    std::error_code ec;
    m_client.noop(ec);
    REQUIRE_FALSE(ec);
}

TEST_CASE_METHOD(logged_in_fixture, "CWD/CDUP test", "[ftp][cwd][cdup]")
{
    std::error_code ec;
    m_client.cwd("documents", ec);
    REQUIRE_FALSE(ec);
    m_client.cdup(ec);
    REQUIRE_FALSE(ec);
}

TEST_CASE_METHOD(logged_in_fixture, "SMNT test", "[ftp][smnt]")
{
    std::error_code ec;
    m_client.smnt("documents", ec);
    REQUIRE_FALSE(ec);
}

TEST_CASE_METHOD(logged_in_fixture, "REIN test", "[ftp][rein][logout]")
{
    std::error_code ec;
    m_client.logout(ec);
    REQUIRE_FALSE(ec);
}

TEST_CASE_METHOD(logged_in_fixture, "RETR test", "[ftp][download][retr]")
{
    std::error_code ec;
    m_client.download("documents/document1.txt", ec);
    REQUIRE_FALSE(ec);
}

TEST_CASE_METHOD(logged_in_fixture, "Rename test", "[ftp][rnfr][rnto][rename]")
{
    std::error_code ec;
    m_client.rename("documents/document2.txt", "documents/document22.txt", ec);
    REQUIRE_FALSE(ec);
    m_client.rename("documents/document22.txt", "documents/document2.txt", ec);
    REQUIRE_FALSE(ec);
}

TEST_CASE_METHOD(logged_in_fixture, "Remove file test", "[ftp][dele][remove]")
{
    SECTION("Remove existing")
    {
        std::error_code ec;
        m_client.remove_file("documents/document2.txt", ec);
        REQUIRE_FALSE(ec);
    }

    SECTION("Remove missing")
    {
        std::error_code ec;
        m_client.remove_file("1337.txt", ec);
        REQUIRE(ec);
    }
}

TEST_CASE_METHOD(logged_in_fixture, "Create/remove directory", "[ftp][mkd][mkdir][rmd][rmdir]")
{
    std::error_code ec;
    m_client.mkdir("test", ec);
    REQUIRE_FALSE(ec);
    m_client.rmdir("test", ec);
    REQUIRE_FALSE(ec);
}

