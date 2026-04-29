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

UDP broadcast file transfer — sends a file to all computers in the same LAN simultaneously, with automatic retransmission of lost packets.

## Table of Contents

- [Features](#features)
- [Quick Start](#quick-start)
- [Requirements](#requirements)
- [Installation](#installation)
- [Parameters](#parameters)
- [How It Works](#how-it-works)
- [Packet Structure](#packet-structure)

## Features

- Broadcast to all computers in LAN with a single send
- Unicast mode for point-to-point transfer
- Automatic retransmission of lost packets
- Configurable MTU and TTL
- Windows, Linux, and macOS support

## Quick Start

**Sender** (machine that has the file):
```
./FileBroadcaster --type sender --file photo.jpg
```

**Receiver** (one or more machines in the same LAN):
```
./FileBroadcaster --type receiver --file photo.jpg
```

To send to a specific IP instead of broadcasting to the whole LAN:
```
./FileBroadcaster --type sender --file photo.jpg --broadcast 192.168.1.50
```

## Requirements

**Windows:**
- Visual Studio 2019 or later

**Linux / macOS:**
- g++ or clang++ with C++14 support
- pthreads

## Installation

**Linux / macOS:**
```
git clone https://github.com/gistrec/File-Broadcaster.git
git submodule update --init --recursive
make program
```

**Windows:**
1. Clone the repository and run `git submodule update --init --recursive`
2. Open `FileBroadcaster.sln` in Visual Studio
3. Build the project

## Parameters

| Parameter | Default | Description |
| --------- | ------- | ----------- |
| `-p, --port` | `33333` | Port for sender and receiver |
| `-f, --file` | `file.out` | File to send or save |
| `-t, --type` | `sender` | `sender` or `receiver` |
| `--ttl` | `15` | Seconds to wait before timing out |
| `--mtu` | `1500` | Max packet size in bytes |
| `--broadcast` | `yes` | `yes` for LAN broadcast, or a specific IP for unicast |

## How It Works

1. Sender broadcasts a `NEW_PACKET` with the total file size
2. Sender splits the file into chunks and broadcasts each one as a `TRANSFER` packet
3. Sender broadcasts a `FINISH` packet when all chunks are sent
4. Receiver checks for missing chunks and requests them with `RESEND` packets
5. Sender retransmits each requested chunk
6. Steps 4–5 repeat until all chunks are received or TTL expires

## Packet Structure

![Packet structure](https://www.gistrec.ru/wp-content/uploads/2019/01/Packets.png)
