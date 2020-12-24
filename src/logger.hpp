/**
 * @file logger.hpp
 */
#pragma once

#include <string>


namespace rs
{
namespace ftp
{

enum class log_level
{
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4,
};

class logger
{
public:
    logger(logger const&) =delete;
    logger(logger&&) =delete;

    auto operator=(logger const&) -> logger& =delete;
    auto operator=(logger&&) -> logger& =delete;

    static auto set_log_level(log_level a_log_level) noexcept -> void;

    static auto debug(std::string const& a_log_message) noexcept -> void;
    static auto info(std::string const& a_log_message) noexcept -> void;
    static auto warning(std::string const& a_log_message) noexcept -> void;
    static auto error(std::string const& a_log_message) noexcept -> void;
    static auto critical(std::string const& a_log_message) noexcept -> void;

private:
    logger();

    auto log(log_level a_log_level, std::string const& a_log_message) noexcept -> void;
    auto should_log(log_level a_log_level) const noexcept -> bool;

    static auto instance() noexcept -> logger&;

private:
    log_level m_log_level;
};

}   // namespace ftp
}   // namespace rs

