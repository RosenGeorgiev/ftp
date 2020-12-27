/**
 * @file codes.hpp
 */
#pragma once

#include <string>
#include <cassert>
#include <exception>


namespace rs
{
namespace ftp
{

// @Unused
static int const TRANSFER_BYTE_SIZE_BITS{8};
// @Unused
static int const LOGICAL_BYTE_SIZE_BITS{8};

static unsigned short const DEFAULT_CONTROL_CONNECTION_PORT{21};
static unsigned short const DEFAULT_DATA_CONNECTION_PORT{DEFAULT_CONTROL_CONNECTION_PORT};

static std::string const CRLF{"\r\n"};
static std::string const SP{' '};
static std::string const COMMA{','};
static std::string const SEPARATOR{'|'};

enum class data_type
{
    ASCII,
    EBCDIC,
    IMAGE,
    LOCAL,
};

inline auto data_type_to_str(data_type a_data_type) noexcept -> std::string
{
    switch (a_data_type)
    {
    case data_type::ASCII:
        return "A";
    case data_type::EBCDIC:
        return "E";
    case data_type::IMAGE:
        return "I";
    case data_type::LOCAL:
        return "L";
    default:
        return "unknown data type";
    }
}

enum class format_control
{
    NONE,
    NON_PRINT,
    TELNET_FROMAT_CONTROLS,
    CARRIAGE_CONTROL,
};

enum class file_structure
{
    FILE_STRUCTURE,
    RECORD_STRUCTURE,
    PAGE_STRUCTURE,
};

inline auto file_structure_to_str(file_structure a_file_structure) noexcept -> std::string
{
    switch (a_file_structure)
    {
    case file_structure::FILE_STRUCTURE:
        return "F";
    case file_structure::RECORD_STRUCTURE:
        return "R";
    case file_structure::PAGE_STRUCTURE:
        return "P";
    default:
        return "unknown file structure";
    }
}

enum class transmission_mode
{
    BLOCK,
    COMPRESSED,
    STREAM,
};

inline auto transmission_mode_to_str(transmission_mode a_transmission_mode) noexcept -> std::string
{
    switch (a_transmission_mode)
    {
    case transmission_mode::BLOCK:
        return "B";
    case transmission_mode::COMPRESSED:
        return "C";
    case transmission_mode::STREAM:
        return "S";
    default:
        return "unknown transmission mode";
    }
}

enum class block_header_descriptor_codes
{
    DATA_BLOCK_IS_A_RESTART_MARKER = 16,
    SUSPECTED_ERRORS_IN_DATA_BLOCK = 32,
    END_OF_DATA_BLOCK_IS_EOF = 64,
    END_OF_DATA_BLOCK_IS_EOR = 128,
};

enum class data_channel_protection_level
{
    CLEAR,
    SAFE,
    CONFIDENTIAL,
    PRIVATE,
};

inline auto data_channel_protection_level_to_str(data_channel_protection_level a_protection_level) noexcept -> std::string
{
    switch (a_protection_level)
    {
    case data_channel_protection_level::CLEAR:
        return "C";
    case data_channel_protection_level::SAFE:
        return "S";
    case data_channel_protection_level::CONFIDENTIAL:
        return "E";
    case data_channel_protection_level::PRIVATE:
        return "P";
    default:
        return "unknown data channel protection level";
    }
}

enum class authentication_method
{
    TLS,
};

inline auto authentication_method_to_str(authentication_method a_auth_method)
{
    switch (a_auth_method)
    {
    case authentication_method::TLS:
        return "TLS";
    default:
        return "unknown authentication method";
    }
}

enum class address_family
{
    AF_INET4 = 1,
    AF_INET6 = 2,
    ALL,
};

inline auto address_family_to_str(address_family a_af)
{
    switch (a_af)
    {
    case address_family::AF_INET4:
        return "1";
    case address_family::AF_INET6:
        return "2";
    case address_family::ALL:
        return "ALL";
    default:
        return "unknown address family";
    }
}

inline auto str_to_address_family(std::string const& a_af) -> address_family
{
    if (a_af == "1")
    {
        return address_family::AF_INET4;
    } else if (a_af == "2")
    {
        return address_family::AF_INET6;
    } else if (a_af == "ALL")
    {
        return address_family::ALL;
    } else
    {
        throw std::runtime_error("Invalid address family");
    }
}

enum class ftp_command
{
    /**
     * RFC959 commands
     */
    /**
     * 230
     * 530
     * 500, 501, 421
     * 331, 332
     *
     * Added by RFC2228
     *
     * 232
     * 336
     */
    USER,
    /**
     * 230
     * 202
     * 530
     * 500, 501, 503, 421
     * 332
     */
    PASS,
    /**
     * 230
     * 202
     * 530
     * 500, 501, 503, 421
     */
    ACCT,
    /**
     * 250
     * 500, 501, 502, 421, 530, 550
     */
    CWD,
    /**
     * 200, 250(Non-standard, but vsftpd)
     * 500, 501, 502, 421, 530, 550
     */
    CDUP,
    /**
     * 202, 250
     * 500, 501, 502, 421, 530, 550
     */
    SMNT,
    /**
     * 221
     * 500
     */
    QUIT,
    /**
     * 120
     *    220
     * 220
     * 421
     * 500, 502
     */
    REIN,
    /**
     * 200
     * 500, 501, 421, 530
     */
    PORT,
    /**
     * 227
     * 500, 501, 502, 421, 530
     */
    PASV,
    /**
     * 200
     * 500, 501, 504, 421, 530
     */
    TYPE,
    /**
     * 200
     * 500, 501, 504, 421, 530
     */
    STRU,
    /**
     * 200
     * 500, 501, 504, 421, 530
     */
    MODE,
    /**
     * 125, 150
     *    (110)
     *    226, 250
     *    425, 426, 451
     * 450, 550
     * 500, 501, 421, 530
     *
     * Added by RFC2228
     *
     * 534, 535
     */
    RETR,
    /**
     * 125, 150
     *    (110)
     *    226, 250
     *    425, 426, 451, 551, 552
     * 532, 450, 452, 553
     * 500, 501, 421, 530
     *
     * Added by RFC2228
     *
     * 534, 535
     */
    STOR,
    /**
     * 125, 150
     *    (110)
     *    226, 250
     *    425, 426, 451, 551, 552
     * 532, 450, 452, 553
     * 500, 501, 421, 530
     *
     * Added by RFC2228
     *
     * 534, 535
     */
    STOU,
    /**
     * 125, 150
     *    (110)
     *    226, 250
     *    425, 426, 451, 551, 552
     * 532, 450, 550, 452, 553
     * 500, 501, 502, 421, 530
     */
    APPE,
    /**
     * 200
     * 202
     * 500, 501, 504, 421, 530
     *
     * Added by RFC2228
     *
     * 534, 535
     */
    ALLO,
    /**
     * 500, 501, 502, 421, 530
     * 350
     */
    REST,
    /**
     * 450, 550
     * 500, 501, 502, 421, 530
     * 350
     */
    RNFR,
    /**
     * 250
     * 532, 553
     * 500, 501, 502, 503, 421, 530
     */
    RNTO,
    /**
     * 225, 226
     * 500, 501, 502, 421
     */
    ABOR,
    /**
     * 250
     * 450, 550
     * 500, 501, 502, 421, 530
     */
    DELE,
    /**
     * 250
     * 500, 501, 502, 421, 530, 550
     */
    RMD,
    /**
     * 257
     * 500, 501, 502, 421, 530, 550
     */
    MKD,
    /**
     * 257
     * 500, 501, 502, 421, 550
     */
    PWD,
    /**
     * 125, 150
     *    226, 250
     *    425, 426, 451
     * 450
     * 500, 501, 502, 421, 530
     *
     * Added by RFC2228
     *
     * 534, 535
     */
    LIST,
    /**
     * 125, 150
     *    226, 250
     *    425, 426, 451
     * 450
     * 500, 501, 502, 421, 530
     *
     * Added by RFC2228
     *
     * 534, 535
     */
    NLST,
    /**
     * 200
     * 202
     * 500, 501, 530
     */
    SITE,
    /**
     * 215
     * 500, 501, 502, 421
     */
    SYST,
    /**
     * 211, 212, 213
     * 450
     * 500, 501, 502, 421, 530
     */
    STAT,
    /**
     * 211, 214
     * 500, 501, 502, 421
     */
    HELP,
    /**
     * 200
     * 500 421
     */
    NOOP,
    /**
     * RFC2228 commands
     */
    /**
     * 234
     * 334
     * 502, 504, 534, 431
     * 500, 501, 421
     */
    AUTH,
    /**
     * 235
     * 335
     * 503, 501, 535
     * 500, 501, 421
     */
    ADAT,
    /**
     * 200
     * 503
     * 500, 501, 421, 530
     */
    PBSZ,
    CCC,
    /**
     * 200
     * 504, 536, 503, 534, 431
     * 500, 501, 421, 530
     */
    PROT,
    /**
     * 535, 533
     * 500, 501, 421
     */
    MIC,
    /**
     * 535, 533
     * 500, 501, 421
     */
    CONF,
    /**
     * 535, 533
     * 500, 501, 421
     */
    ENC,
    /**
     * RFC2428 commands
     */
    /**
     * 200
     * 500, 501, 522
     */
    EPRT,
    /**
     * 229
     * 500, 501, 522
     */
    EPSV,
};

inline auto ftp_command_to_str(ftp_command a_ftp_command) noexcept -> std::string
{
    switch (a_ftp_command)
    {
    case ftp_command::USER:
        return"USER";
    case ftp_command::PASS:
        return  "PASS";
    case ftp_command::ACCT:
        return "ACCT";
    case ftp_command::CWD:
        return "CWD";
    case ftp_command::CDUP:
        return "CDUP";
    case ftp_command::SMNT:
        return "SMNT";
    case ftp_command::QUIT:
        return "QUIT";
    case ftp_command::REIN:
        return "REIN";
    case ftp_command::PORT:
        return "PORT";
    case ftp_command::PASV:
        return "PASV";
    case ftp_command::TYPE:
        return "TYPE";
    case ftp_command::STRU:
        return "STRU";
    case ftp_command::MODE:
        return "MODE";
    case ftp_command::RETR:
        return "RETR";
    case ftp_command::STOR:
        return "STOR";
    case ftp_command::STOU:
        return "STOU";
    case ftp_command::APPE:
        return "APPE";
    case ftp_command::ALLO:
        return "ALLO";
    case ftp_command::REST:
        return "REST";
    case ftp_command::RNFR:
        return "RNFR";
    case ftp_command::RNTO:
        return "RNTO";
    case ftp_command::ABOR:
        return "ABOR";
    case ftp_command::DELE:
        return "DELE";
    case ftp_command::RMD:
        return "RMD";
    case ftp_command::MKD:
        return "MKD";
    case ftp_command::PWD:
        return "PWD";
    case ftp_command::LIST:
        return "LIST";
    case ftp_command::NLST:
        return "NLST";
    case ftp_command::SITE:
        return "SITE";
    case ftp_command::SYST:
        return "SYST";
    case ftp_command::STAT:
        return "STAT";
    case ftp_command::HELP:
        return "HELP";
    case ftp_command::NOOP:
        return "NOOP";
    case ftp_command::AUTH:
        return "AUTH";
    case ftp_command::ADAT:
        return "ADAT";
    case ftp_command::PBSZ:
        return "PBSZ";
    case ftp_command::CCC:
        return "CCC";
    case ftp_command::PROT:
        return "PROT";
    case ftp_command::MIC:
        return "MIC";
    case ftp_command::CONF:
        return "CONF";
    case ftp_command::ENC:
        return "ENC";
    case ftp_command::EPRT:
        return "EPRT";
    case ftp_command::EPSV:
        return "EPSV";
    default:
        return "unknown command";
    }
}

enum class reply_code
{
    /**
     * 1yz - Positive Preliminary reply
     *
     * The requested action is being initiated; expect another
     * reply before proceeding with a new command.  (The
     * user-process sending another command before the
     * completion reply would be in violation of protocol; but
     * server-FTP processes should queue any commands that
     * arrive while a preceding command is in progress.)  This
     * type of reply can be used to indicate that the command
     * was accepted and the user-process may now pay attention
     * to the data connections, for implementations where
     * simultaneous monitoring is difficult.  The server-FTP
     * process may send at most, one 1yz reply per command.
     */
    RESTART_MARKER_110 = 110,
    SERVICE_READY_IN_X_MINUTES_120 = 120,
    DATA_CONNECTION_OPEN_TRANSFER_STARTING_125 = 125,
    FILE_STATUS_OK_OPENING_DATA_CONNECTION_150 = 150,
    /**
     * 2yz - Positive Completion reply
     *
     * The requested action has been successfully completed.  A
     * new request may be initiated.
     */
    OK_200 = 200,
    NOT_IMPLEMENTED_202 = 202,
    SYSTEM_STATUS_211 = 211,
    DIRECTORY_STATUS_212 = 212,
    FILE_STATUS_213 = 213,
    HELP_MESSAGE_214 = 214,
    X_SYSTEM_TYPE_215 = 215,
    READY_FOR_NEW_USER_220 = 220,
    CLOSING_CONTROL_CONNECTION_221 = 221,
    DATA_CONNECTION_OPEN_NO_TRANSFER_225 = 225,
    CLOSING_DATA_CONNECTION_226 = 226,
    ENTERING_PASSIVE_MODE_227 = 227,
    USER_LOGGED_IN_230 = 230,
    FILE_ACTION_COMPLETED_250 = 250,
    PATHNAME_CREATED_257 = 257,
    /**
     * 3yz - Positive Intermediate reply
     *
     * The command has been accepted, but the requested action
     * is being held in abeyance, pending receipt of further
     * information.  The user should send another command
     * specifying this information.  This reply is used in
     * command sequence groups.
     */
    USERNAME_OK_NEED_PASSWORD_331 = 331,
    NEED_ACCOUNT_332 = 332,
    REQUESTED_FILE_ACTION_INFO_PENDING_350 = 350,
    /**
     * 4yz - Transient Negative Completion reply
     *
     * The command was not accepted and the requested action did
     * not take place, but the error condition is temporary and
     * the action may be requested again.  The user should
     * return to the beginning of the command sequence, if any.
     * It is difficult to assign a meaning to "transient",
     * particularly when two distinct sites (Server- and
     * User-processes) have to agree on the interpretation.
     * Each reply in the 4yz category might have a slightly
     * different time value, but the intent is that the
     * user-process is encouraged to try again.  A rule of thumb
     * in determining if a reply fits into the 4yz or the 5yz
     * (Permanent Negative) category is that replies are 4yz if
     * the commands can be repeated without any change in
     * command form or in properties of the User or Server
     * (e.g., the command is spelled the same with the same
     * arguments used; the user does not change his file access
     * or user name; the server does not put up a new
     * implementation.)
     */
    SERVICE_NOT_AVAILABLE_421 = 421,
    CANT_OPEN_DATA_CONNECTION_425 = 425,
    CONNECTION_CLOSED_TRANSFER_ABORTED_426 = 426,
    FILE_ACTION_NOT_TAKEN_450 = 450,
    ACTION_ABORTED_LOCAL_ERROR_451 = 451,
    ACTION_NOT_TAKEN_452 = 452,
    /**
     * 5yz - Permanent Negative Completion reply
     *
     * The command was not accepted and the requested action did
     * not take place.  The User-process is discouraged from
     * repeating the exact request (in the same sequence).  Even
     * some "permanent" error conditions can be corrected, so
     * the human user may want to direct his User-process to
     * reinitiate the command sequence by direct action at some
     * point in the future (e.g., after the spelling has been
     * changed, or the user has altered his directory status.)
     */
    COMMAND_SYNTAX_ERROR_500 = 500,
    PARAMETER_SYNTAX_ERROR_501 = 501,
    COMMAND_NOT_IMPLEMENTED_502 = 502,
    BAD_SEQUENCE_503 = 503,
    COMMAND_NOT_IMPLEMENTED_FOR_PARAMETER_504 = 504,
    NOT_LOGGED_IN_530 = 530,
    NEED_ACCOUNT_532 = 532,
    ACTION_NOT_TAKEN_550 = 550,
    ACTION_ABORTED_PAGE_TYPE_UNKNOWN_551 = 551,
    FILE_ACTION_ABORTED_552 = 552,
    ACTION_NOT_TAKEN_553 = 553,
    /**
     * Added by RFC2228
     */
    USER_LOGGED_IN_232 = 232,
    SECURITY_DATA_EXCHANGE_COMPLETE_234 = 234,
    /**
     * This reply indicates that the security data exchange
     * completed successfully.  The square brackets are not
     * to be included in the reply, but indicate that
     * security data in the reply is optional.
     */
    SECURITY_DATA_EXCHANGE_COMPLETE_235 = 235,
    /**
     * This reply indicates that the requested security mechanism
     * is OK, and includes security data to be used by the client
     * to construct the next command.  The square brackets are not
     * to be included in the reply, but indicate that
     * security data in the reply is optional.
     */
    SECURITY_MECHANISM_OK_334 = 334,
    /**
     * This reply indicates that the security data is
     * acceptable, and more is required to complete the
     * security data exchange.  The square brackets
     * are not to be included in the reply, but indicate
     * that security data in the reply is optional.
     */
    SECURITY_DATA_OK_AGAIN_335 = 335,
    /**
     * The exact representation of the challenge should be chosen
     * by the mechanism to be sensible to the human user of the
     * system.
     */
    USERNAME_OK_NEED_PASSWORD_336 = 336,
    NEED_SOME_UNAVAILABLE_RESOURCE_TO_PROCESS_SECURITY_431 = 431,
    COMMAND_PROTECTION_LEVEL_DENIED_FOR_POLICY_REASONS_533 = 533,
    REQUEST_DENIED_FOR_POLICY_REASONS_534 = 534,
    FAILED_SECURITY_CHECK_535 = 535,
    REQUESTED_PROT_LEVEL_NOT_SUPPORTED_BY_MECHANISM_536 = 536,
    COMMAND_PROTECTION_LEVEL_NOT_SUPPORTED_BY_SECURITY_MECHANISM_537 = 537,
    /**
     * 6yz - Protected reply
     */
    INTEGRITY_PROTECTED_REPLY_631 = 631,
    CONFIDENTIALITY_AND_INTEGRITY_PROTECTED_REPLY_632 = 632,
    CONFIDENTIALITY_PROTECTED_REPLY_633 = 633,
    /**
     * Added by RFC2428
     */
    ENTERING_EXTENDED_PASSIVE_MODE_229 = 229,
    REQUESTED_NETWORK_PROTOCOL_UNSUPPORTED_522 = 522,
};

inline auto reply_code_to_str(reply_code a_reply_code) noexcept -> std::string
{
    switch (a_reply_code)
    {
    case reply_code::RESTART_MARKER_110:
        return "Restart marker reply.";
    case reply_code::SERVICE_READY_IN_X_MINUTES_120:
        return "Service ready in nnn minutes.";
    case reply_code::DATA_CONNECTION_OPEN_TRANSFER_STARTING_125:
        return "Data connection already open; transfer starting.";
    case reply_code::FILE_STATUS_OK_OPENING_DATA_CONNECTION_150:
        return "File status okay; about to open data connection.";
    case reply_code::OK_200:
        return "Command okay.";
    case reply_code::NOT_IMPLEMENTED_202:
        return "Command not implemented, superfluous at this site.";
    case reply_code::SYSTEM_STATUS_211:
        return "System status, or system help reply.";
    case reply_code::DIRECTORY_STATUS_212:
        return "Directory status.";
    case reply_code::FILE_STATUS_213:
        return "File status.";
    case reply_code::HELP_MESSAGE_214:
        return "Help message.";
    case reply_code::X_SYSTEM_TYPE_215:
        return "NAME system type.";
    case reply_code::READY_FOR_NEW_USER_220:
        return "Service ready for new user.";
    case reply_code::CLOSING_CONTROL_CONNECTION_221:
        return "Service closing control connection.";
    case reply_code::DATA_CONNECTION_OPEN_NO_TRANSFER_225:
        return "Data connection open; no transfer in progress.";
    case reply_code::CLOSING_DATA_CONNECTION_226:
        return "Closing data connection.";
    case reply_code::ENTERING_PASSIVE_MODE_227:
        return "Entering Passive Mode (h1,h2,h3,h4,p1,p2).";
    case reply_code::USER_LOGGED_IN_230:
        return "User logged in, proceed.";
    case reply_code::FILE_ACTION_COMPLETED_250:
        return "Requested file action okay, completed.";
    case reply_code::PATHNAME_CREATED_257:
        return "\"PATHNAME\" created.";
    case reply_code::USERNAME_OK_NEED_PASSWORD_331:
        return "User name okay, need password.";
    case reply_code::NEED_ACCOUNT_332:
        return "Need account for login.";
    case reply_code::REQUESTED_FILE_ACTION_INFO_PENDING_350:
        return "Requested file action pending further information.";
    case reply_code::SERVICE_NOT_AVAILABLE_421:
        return "Service not available, closing control connection.";
    case reply_code::CANT_OPEN_DATA_CONNECTION_425:
        return "Can't open data connection.";
    case reply_code::CONNECTION_CLOSED_TRANSFER_ABORTED_426:
        return "Connection closed; transfer aborted.";
    case reply_code::FILE_ACTION_NOT_TAKEN_450:
        return "Requested file action not taken.";
    case reply_code::ACTION_ABORTED_LOCAL_ERROR_451:
        return "Requested action aborted. Local error in processing.";
    case reply_code::ACTION_NOT_TAKEN_452:
        return "Requested action not taken.";
    case reply_code::COMMAND_SYNTAX_ERROR_500:
        return "Syntax error, command unrecognized.";
    case reply_code::PARAMETER_SYNTAX_ERROR_501:
        return "Syntax error in parameters or arguments.";
    case reply_code::COMMAND_NOT_IMPLEMENTED_502:
        return "Command not implemented.";
    case reply_code::BAD_SEQUENCE_503:
        return "Bad sequence of commands.";
    case reply_code::COMMAND_NOT_IMPLEMENTED_FOR_PARAMETER_504:
        return "Command not implemented for that parameter.";
    case reply_code::NOT_LOGGED_IN_530:
        return "Not logged in.";
    case reply_code::NEED_ACCOUNT_532:
        return "Need account for storing files.";
    case reply_code::ACTION_NOT_TAKEN_550:
        return "Requested action not taken.";
    case reply_code::ACTION_ABORTED_PAGE_TYPE_UNKNOWN_551:
        return "Requested action aborted. Page type unknown.";
    case reply_code::FILE_ACTION_ABORTED_552:
        return "Requested file action aborted.";
    case reply_code::ACTION_NOT_TAKEN_553:
        return "Requested action not taken.";
    case reply_code::USER_LOGGED_IN_232:
        return "User logged-in.";
    case reply_code::SECURITY_DATA_EXCHANGE_COMPLETE_234:
        return "Security data exchange complete.";
    case reply_code::SECURITY_DATA_EXCHANGE_COMPLETE_235:
        return "Security data exchange complete.";
    case reply_code::SECURITY_MECHANISM_OK_334:
        return "Security mechanism OK.";
    case reply_code::SECURITY_DATA_OK_AGAIN_335:
        return "Security data OK, but more is required.";
    case reply_code::USERNAME_OK_NEED_PASSWORD_336:
        return "Username OK, password is needed.";
    case reply_code::NEED_SOME_UNAVAILABLE_RESOURCE_TO_PROCESS_SECURITY_431:
        return "Need some unavailable resource to process security.";
    case reply_code::COMMAND_PROTECTION_LEVEL_DENIED_FOR_POLICY_REASONS_533:
        return "Command protection level denied for policy reasons.";
    case reply_code::REQUEST_DENIED_FOR_POLICY_REASONS_534:
        return "Request denied for policy reasons.";
    case reply_code::FAILED_SECURITY_CHECK_535:
        return "Failed security check.";
    case reply_code::REQUESTED_PROT_LEVEL_NOT_SUPPORTED_BY_MECHANISM_536:
        return "Requested protection level not supported by mechanism.";
    case reply_code::COMMAND_PROTECTION_LEVEL_NOT_SUPPORTED_BY_SECURITY_MECHANISM_537:
        return "Command protection level not supported by security mechanism.";
    case reply_code::INTEGRITY_PROTECTED_REPLY_631:
        return "Integrity protected reply.";
    case reply_code::CONFIDENTIALITY_AND_INTEGRITY_PROTECTED_REPLY_632:
        return "Confidentiality and integrity protected reply.";
    case reply_code::CONFIDENTIALITY_PROTECTED_REPLY_633:
        return "Confidentiality protected reply.";
    case reply_code::ENTERING_EXTENDED_PASSIVE_MODE_229:
        return "Entering extended passive mode.";
    case reply_code::REQUESTED_NETWORK_PROTOCOL_UNSUPPORTED_522:
        return "Requested network protocol unsupported, use (1,2).";
    default:
        return "unknown reply code";
    }
}

}   // namespace ftp
}   // namespace rs

