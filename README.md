# File-Broadcaster

<p align="left">
    <a href="https://github.com/gistrec/File-Broadcaster/actions/workflows/tests.yml">
        <img src="https://github.com/gistrec/File-Broadcaster/actions/workflows/tests.yml/badge.svg" alt="Tests"></a>
    <a>
      <img src="https://img.shields.io/codacy/grade/4c8169bcab3a4df18baad4e5658ec8ce" alt="Code quality"></a>
    <a href="https://github.com/gistrec/File-Broadcaster/releases">
        <img src="https://img.shields.io/github/v/release/gistrec/File-Broadcaster" alt="Release"></a>
    <a>
      <img src="https://img.shields.io/badge/platform-windows%20%7C%20linux%20%7C%20macos-brightgreen" alt="Platform"></a>
    <a href="https://github.com/gistrec/File-Broadcaster/blob/master/LICENSE">
        <img src="https://img.shields.io/github/license/gistrec/File-Broadcaster?color=brightgreen" alt="License"></a>
</p>

UDP broadcast file transfer — sends a single file to every host on the same
LAN at once, with automatic retransmission of dropped packets.

## Table of Contents

- [Features](#features)
- [Quick Start](#quick-start)
- [Installation](#installation)
- [Parameters](#parameters)
- [Examples](#examples)
- [How It Works](#how-it-works)
- [Packet Structure](#packet-structure)
- [Limitations](#limitations)
- [Building from Source](#building-from-source)
- [License](#license)

## Features

- Broadcast to every host on a LAN with a single transmission
- Unicast mode for point-to-point transfer
- Automatic retransmission of lost packets
- Configurable MTU and timeout
- Windows, Linux, and macOS binaries built on every release

## Quick Start

Download the binary for your platform from the
[releases page](https://github.com/gistrec/File-Broadcaster/releases) and run
it directly. No installation required.

**Sender** (host that has the file):

```sh
./FileBroadcaster --type sender --file photo.jpg
```

**Receiver** (one or more hosts on the same LAN):

```sh
./FileBroadcaster --type receiver --file photo.jpg
```

Send to a specific host instead of broadcasting to the whole LAN:

```sh
./FileBroadcaster --type sender --file photo.jpg --broadcast 192.168.1.50
```

## Installation

Pre-built binaries for Linux x86_64, macOS arm64, and Windows x86_64 are
attached to every [GitHub Release](https://github.com/gistrec/File-Broadcaster/releases).

If your platform isn't covered, see [Building from Source](#building-from-source).

## Parameters

| Parameter | Default | Range | Description |
| --------- | ------- | ----- | ----------- |
| `-f, --file`  | `file.out` | — | File to send or save |
| `-t, --type`  | `sender`   | `sender` / `receiver` | Run mode |
| `--broadcast` | `yes`      | `yes` or IPv4 | `yes` for LAN broadcast, or a specific IP for unicast |
| `-p, --port`  | `33333`    | 1..65535 | Destination port for outgoing packets |
| `--bind-port` | `33333`    | 1..65535 | Local port to bind on |
| `--mtu`       | `1500`     | 64..65507 | Max packet size in bytes |
| `--ttl`       | `15`       | > 0 | Seconds of silence before giving up |
| `-h, --help`  | —          | — | Print help |
| `--version`   | —          | — | Print version |

## Examples

**LAN broadcast** (one sender, many receivers):

```sh
# On the sender host
./FileBroadcaster --type sender --file album.zip

# On every receiver host
./FileBroadcaster --type receiver --file album.zip
```

**Targeted unicast** (when broadcast is blocked or you only have one receiver):

```sh
# On the sender host (sends data to 10.0.0.42)
./FileBroadcaster --type sender --file album.zip --broadcast 10.0.0.42

# On 10.0.0.42 (receiver default --broadcast=yes broadcasts RESENDs)
./FileBroadcaster --type receiver --file album.zip
```

**Loopback test** (sender and receiver on the same host — useful for
development):

```sh
# Receiver listens on 33401, sends RESEND back to the sender's bind port (33402)
./FileBroadcaster --type receiver --file out.bin \
                  --broadcast 127.0.0.1 --port 33402 --bind-port 33401 &

# Sender listens on 33402, sends data to the receiver's bind port (33401)
./FileBroadcaster --type sender --file in.bin \
                  --broadcast 127.0.0.1 --port 33401 --bind-port 33402
```

## How It Works

1. Sender broadcasts a `NEW_PACKET` packet with the total file size.
2. Each receiver allocates a buffer of that size and clears its part registry.
3. Sender splits the file into MTU-sized chunks and broadcasts each one as a
   `TRANSFER` packet.
4. Sender broadcasts a `FINISH` packet when all chunks have been sent.
5. Each receiver scans for missing chunks and requests them with `RESEND`
   packets.
6. Sender retransmits each requested chunk.
7. Steps 5–6 repeat until every chunk is received or the TTL expires.

## Packet Structure

![Packet structure](https://www.gistrec.ru/wp-content/uploads/2019/01/Packets.png)

## Limitations

- The whole file is held in RAM on both sides. The receiver enforces a 4 GiB
  cap on the announced file size; the sender is bounded only by available
  memory.
- No data integrity check beyond UDP's optional 16-bit checksum. If the
  payload is corrupted in a way the checksum doesn't catch, the receiver will
  silently produce a corrupted file.
- No authentication. Any host on the same LAN can send a `NEW_PACKET` and any
  receiver bound to the chosen port will accept it.
- No encryption. The payload travels as plaintext UDP.

## Building from Source

### Requirements

- **Linux / macOS:** GCC 7+ or Clang 5+ with C++17 support, GNU Make, pthreads.
- **Windows (MinGW64):** [MSYS2](https://www.msys2.org/) with
  `mingw-w64-x86_64-gcc` and `make`.
- **Windows (Visual Studio):** Visual Studio 2017 or later with the v141
  platform toolset.

### Linux / macOS / MSYS2

```sh
git clone https://github.com/gistrec/File-Broadcaster.git
cd File-Broadcaster
git submodule update --init --recursive
make program
```

The binary is written to `./FileBroadcaster`.

### Windows (Visual Studio)

1. Clone the repository.
2. Run `git submodule update --init --recursive`.
3. Open `FileBroadcaster.sln` in Visual Studio.
4. Build the solution.

### Tests

```sh
make gtests   # unit tests (requires Google Test)
./GTests

make e2e      # loopback end-to-end test (small + large file)
```

## License

[MIT](LICENSE).
