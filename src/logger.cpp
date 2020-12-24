#include "logger.hpp"

#include <iostream>


namespace rs
{
namespace ftp
{

logger::logger() :
    m_log_level(log_level::WARNING)
{ }

static auto log_level_to_str(log_level a_log_level) noexcept -> std::string
{
    switch(a_log_level)
    {
    case log_level::DEBUG:
        return "DEBUG";
    case log_level::INFO:
        return "INFO";
    case log_level::WARNING:
        return "WARNING";
    case log_level::ERROR:
        return "ERROR";
    case log_level::CRITICAL:
        return "CRITICAL";
    default:
        return "INVALID LOG LEVEL";
    }
}

auto logger::log(log_level a_log_level, std::string const& a_log_message) noexcept -> void
{
    std::cout << log_level_to_str(a_log_level)
              << ": "
              << a_log_message
              << std::endl;
}

auto logger::should_log(log_level a_log_level) const noexcept -> bool
{
    return a_log_level >= m_log_level;
}

auto logger::set_log_level(log_level a_log_level) noexcept -> void
{
    logger& logger = logger::instance();
    logger.m_log_level = a_log_level;
}

auto logger::debug(std::string const& a_log_message) noexcept -> void
{
#ifndef NDEBUG
    logger& logger = logger::instance();
    if (logger.should_log(log_level::DEBUG))
    {
        logger.log(log_level::DEBUG, a_log_message);
    }
#endif
}

auto logger::info(std::string const& a_log_message) noexcept -> void
{
    logger& logger = logger::instance();
    if (logger.should_log(log_level::INFO))
    {
        logger.log(log_level::INFO, a_log_message);
    }
}

auto logger::warning(std::string const& a_log_message) noexcept -> void
{
    logger& logger = logger::instance();
    if (logger.should_log(log_level::WARNING))
    {
        logger.log(log_level::WARNING, a_log_message);
    }
}

auto logger::error(std::string const& a_log_message) noexcept -> void
{
    logger& logger = logger::instance();
    if (logger.should_log(log_level::ERROR))
    {
        logger.log(log_level::ERROR, a_log_message);
    }
}

auto logger::critical(std::string const& a_log_message) noexcept -> void
{
    logger& logger = logger::instance();
    if (logger.should_log(log_level::CRITICAL))
    {
        logger.log(log_level::CRITICAL, a_log_message);
    }
}

auto logger::instance() noexcept -> logger&
{
    static logger l;
    return l;
}

}   // namespace ftp
}   // namespace rs

