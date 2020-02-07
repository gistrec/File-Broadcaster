# File-Broadcaster

<p align="left">
    <a href="https://circleci.com/gh/gistrec/File-Broadcaster/tree/master">
        <img src="https://img.shields.io/circleci/build/github/gistrec/File-Broadcaster/master" alt="Build status"></a>
    <a>
      <img src="https://img.shields.io/codacy/grade/4c8169bcab3a4df18baad4e5658ec8ce" alt="Code quality"></a>
    <a href="https://github.com/gistrec/File-Broadcaster/releases">
        <img src="https://img.shields.io/github/v/release/gistrec/File-Broadcaster" alt="Release"></a>
    <a href="https://github.com/gistrec/File-Broadcaster/blob/master/LICENSE">
        <img src="https://img.shields.io/github/license/gistrec/File-Broadcaster" alt="License"></a>
</p>

UDP File sender and receiver  
Can use broadcast address to send file on all computers in LAN

## Features

- Send file to one or all computers in LAN
- Reliability of data transmission
- Server timeout detection  
- Change MTU

## Overview

- [Requirements](#requirements)
- [Download](#download)
- [Installation](#installation)
- [Script Parameters](#script-parameters)
- [Packets Specification](#packets-specification)
- [Script Specification](#script-specification)

## Download
 Clone the [source repository](http://github.com/gistrec/File-Broadcaster) from Github. 
* On the command line, enter:
    ````
    git clone https://github.com/gistrec/File-Broadcaster.git
    git submodule init
    git submodule update --recursive --remote
    ````

* You can probably use [Github for Windows](http://windows.github.com/) or [Github for Mac](http://mac.github.com/) instead of the command line, however these aren't tested/supported and we only use the command line for development. Use [this link](https://git-scm.com/downloads) to download the command line version.


## Requirements
* Windows:
    * Visual Studio 2015 or 2017
* Linux:
    * g++
    * pthread
    * arpa

  

## Installation
* Windows
    * Open FileBroadcaster.sln via Visual Studio
    * Build project 
* Linux
    * Open a terminal/console/command prompt, change to the directory where you cloned project, and type:
      ````
      make all
      ````

## Script Parameters
| Parameter   | Default | Description |
| ------ | -------- | -------- |
| p, port | 33333 | Sender and receiver port |
| f, filename| `none` | Transmitted and received file |
| t, type | receiver | receiver or sender |
| ttl | 15 | Seconds to wait cliend requests or sender responses  |
| mtu | 1500 | MTU packet size |
| broadcast | 255.255.255.255 | Broadcast address. Can use to unicast. |

## Packets Specification
Packets structure
![alt text](https://www.gistrec.ru/wp-content/uploads/2019/01/Packets.png)

## Script Specification
1. Sender send `NEW_PACKET` packet to broadcast (or unicast) address
2. Sender send all parts of file via `TRANSFER` packet
3. If any pacckets were lost, receiver ask them sending `RESEND` packet to broadcast (or unicast) address
4. Sender wait `RESEND` packets or wait TTL and turns off
5. Receiver ask all lost parts, until the whole file is no downloaded or wait TTL and turns off
