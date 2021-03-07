[![License: WTFPL](http://www.wtfpl.net/wp-content/uploads/2012/12/wtfpl-badge-2.png)](http://www.wtfpl.net/txt/copying/)
[![Build Status](http://drone-ci.lan/api/badges/RS/ftp/status.svg)](http://drone-ci.lan/RS/ftp)

# FTP
Minimal FTP client implementation attempting to follow [RFC959](https://tools.ietf.org/html/rfc959).
Partial support for RFC2428. Work on RFC4217 (FTP over TLS) is in progress. Yet another project
that has not been battle tested and probably has issues.

## Requirements
- Boost ASIO
- Catch2 (if you enable the tests)
- CMake
- Compiler with C++17 support

## Usage
### CMake
Just add it as a subdirectory and link it to your program. It builds both static and dynamic
libraries and you can choose between the `ftp::ftp_static` and `ftp::ftp_shared` targets.

```CMake
add_subdirectory(ftp)
...
target_link_libraries(my_executable PRIVATE ftp::ftp_static)
```

### The library
Add the headers to your file:
```cpp
#include <ftp/ftp.hpp>
```

Create a `rs::ftp::connection_options` object and populate it with FTP server information:
```cpp
rs::ftp::connection_options opts;
opts.username = "admin";
opts.password = "admin";
opts.server_hostname = "localhost";
opts.server_port = 21;
```

Then create a `rs::ftp::client` object, pass it the connection options, `connect` and `login`:
```cpp
rs::ftp::client client;
client.set_connection_options(opts);
client.connect();
client.login();
```

The library provides methods for basic file and folder manipulation, also for downloading/uploading
files. For more examples take a look at the test suite. Finally don't forget to `close` the
connection:
```cpp
client.close();
```

Also there is a timeout period for the commands, that you can control from
`rs::ftp::connection_options`.

## Debugging
In case the client misbehaves, debug logging is available. To enable it, set
`rs::ftp::connection_options` to `true`. This will print all the command exchange that occurs
between the client and the server. Attach it with the issue for the bug.

## Limitations
- Only passive transfer mode supported, because of firewalls
- Not thread safe
- Not a complete implementation
Only a subset of the FTP commands are supported. No fancy transmission modes/format controls/etc.
supported.
- Only tested with vsFTPd - might not work correctly with other FTP servers

## Performance
Yes...Maybe...No.

## Disclamer
**DO NOT USE** FTP if you have a more secure way to transfer your data. FTP has been terribly
unsecure for decades now. The only acceptable use case is for integration with **VERY** legacy
systems.

