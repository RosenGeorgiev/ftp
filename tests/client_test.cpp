#include <catch2/catch.hpp>

#include <fstream>

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

        REQUIRE_NOTHROW(m_client.connect());
        REQUIRE_NOTHROW(m_client.login());
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
        REQUIRE_NOTHROW(client.connect());

        SECTION("Close")
        {
            REQUIRE_NOTHROW(client.close());
        }
    }

    SECTION("Unsuccessful connection")
    {
        rs::ftp::connection_options opts;
        opts.server_hostname = "localhost";
        opts.server_port = 3333;
        opts.debug_output = true;

        rs::ftp::client client(opts);
        REQUIRE_THROWS(client.connect());
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

        REQUIRE_NOTHROW(client.connect());
        REQUIRE_NOTHROW(client.login());
    }

    SECTION("Failed login")
    {
        opts.username = "";
        opts.password = "";

        rs::ftp::client client(opts);

        REQUIRE_NOTHROW(client.connect());
        REQUIRE_THROWS(client.login());
    }
}

TEST_CASE_METHOD(logged_in_fixture, "NOOP test", "[ftp][noop]")
{
    REQUIRE_NOTHROW(m_client.noop());
}

TEST_CASE_METHOD(logged_in_fixture, "CWD/CDUP test", "[ftp][cwd][cdup]")
{
    REQUIRE_NOTHROW(m_client.cwd("documents"));
    REQUIRE_NOTHROW(m_client.cdup());
}

TEST_CASE_METHOD(logged_in_fixture, "SMNT test", "[ftp][smnt]")
{
    REQUIRE_NOTHROW(m_client.smnt("documents"));
}

TEST_CASE_METHOD(logged_in_fixture, "REIN test", "[ftp][rein][logout]")
{
    REQUIRE_NOTHROW(m_client.logout());
}

TEST_CASE_METHOD(logged_in_fixture, "RETR test", "[ftp][download][retr]")
{
    rs::ftp::connection_options opts;
    opts.username = "admin";
    opts.password = "admin";
    opts.server_hostname = "localhost";
    opts.server_port = 21;
    opts.data_connection_port = 50000;
    opts.debug_output = true;

    SECTION("Passive")
    {

        opts.passive_mode = true;
        m_client.set_connection_options(opts);

        SECTION("Download to byte vector")
        {
            REQUIRE_NOTHROW(m_client.download("image.jpeg"));
        }

        SECTION("Download to a file")
        {
            std::ofstream out("image.jpeg", std::ios::binary);
            REQUIRE_NOTHROW(m_client.download("image.jpeg", out));
        }
    }

    SECTION("Active")
    {
        opts.passive_mode = false;
        m_client.set_connection_options(opts);

        SECTION("Download to byte vector")
        {
            REQUIRE_NOTHROW(m_client.download("image.jpeg"));
        }

        SECTION("Download to a file")
        {
            std::ofstream out("image.jpeg", std::ios::binary);
            REQUIRE_NOTHROW(m_client.download("image.jpeg", out));
        }
    }
}

TEST_CASE_METHOD(logged_in_fixture, "Rename test", "[ftp][rnfr][rnto][rename]")
{
    REQUIRE_NOTHROW(m_client.rename("documents/document2.txt", "documents/document22.txt"));
    REQUIRE_NOTHROW(m_client.rename("documents/document22.txt", "documents/document2.txt"));
}

TEST_CASE_METHOD(logged_in_fixture, "Remove file test", "[ftp][dele][remove]")
{
    SECTION("Remove existing")
    {
        REQUIRE_NOTHROW(m_client.remove_file("documents/document2.txt"));
    }

    SECTION("Remove missing")
    {
        REQUIRE_THROWS(m_client.remove_file("1337.txt"));
    }
}

TEST_CASE_METHOD(logged_in_fixture, "Create/remove directory test", "[ftp][mkd][mkdir][rmd][rmdir]")
{
    REQUIRE_NOTHROW(m_client.mkdir("test"));
    REQUIRE_NOTHROW(m_client.rmdir("test"));
}

TEST_CASE_METHOD(logged_in_fixture, "Pwd test", "[ftp][pwd]")
{
    REQUIRE_NOTHROW(m_client.pwd());
}

TEST_CASE_METHOD(logged_in_fixture, "LS test", "[ftp][ls][nlst]")
{
    SECTION("LS current directory")
    {
        REQUIRE_NOTHROW(m_client.ls());
    }

    SECTION("LS valid directory")
    {
        REQUIRE_NOTHROW(m_client.ls("documents"));
    }

    SECTION("LS invalid directory")
    {
        REQUIRE_THROWS(m_client.ls("i_dont_exist_neither_should_you"));
    }
}

TEST_CASE_METHOD(logged_in_fixture, "System info test", "[ftp][syst]")
{
    REQUIRE_NOTHROW(m_client.system_info());
}

TEST_CASE_METHOD(logged_in_fixture, "Progress test", "[ftp][stat][progress]")
{
    REQUIRE_NOTHROW(m_client.progress());
}

