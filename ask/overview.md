# Detailed Analysis of NetHogs Repository

## Overview

NetHogs is a network monitoring tool that provides a unique approach to bandwidth monitoring. Unlike most network monitoring tools that break down traffic by protocol or subnet, NetHogs groups bandwidth usage by process. This makes it particularly useful for quickly identifying which processes are consuming network bandwidth.

## Core Functionality

NetHogs operates by:

1. Capturing network packets using libpcap
2. Associating these packets with specific connections
3. Mapping connections to processes using information from `/proc`
4. Displaying real-time bandwidth usage in a terminal-based UI using ncurses

## Key Components

### Main Architecture

The codebase is organized into several key components:

1. **Packet Capture** (`decpcap.c/h`): Interfaces with libpcap to capture network packets
2. **Packet Processing** (`packet.cpp/h`): Represents and processes network packets
3. **Connection Tracking** (`connection.cpp/h`): Manages network connections
4. **Process Mapping** (`process.cpp/h`, `inode2prog.cpp/h`, `conninode.cpp/h`): Maps connections to processes
5. **User Interface** (`cui.cpp/h`): Handles the terminal-based UI using ncurses
6. **Main Application Logic** (`nethogs.cpp`, `main.cpp`): Coordinates the overall application flow

### Terminal UI

The terminal UI is implemented in `cui.cpp` and provides:

- A real-time display of processes using network bandwidth
- Sorting by sent or received traffic
- Different view modes (KB/s, total KB, total bytes, MB, MB/s, GB/s)
- Interactive controls for changing display options

### Process Identification

NetHogs identifies which processes are using network connections by:

1. Reading connection information from `/proc/net/tcp` and `/proc/net/udp`
2. Mapping socket inodes to processes by examining `/proc/[pid]/fd/`
3. Retrieving process details like name, command line, and user ID

### Data Structures

Key data structures include:

- `Process`: Represents a process with its connections and bandwidth usage
- `Connection`: Represents a network connection with source/destination addresses and ports
- `Packet`: Represents a network packet with size and timestamp
- `Line`: Used for displaying a process in the UI

## Features

1. **Real-time Monitoring**: Shows bandwidth usage in KB/s, MB/s, or GB/s
2. **Process Identification**: Groups traffic by process name and PID
3. **User Identification**: Shows which user owns each process
4. **Multiple View Modes**: Can display instantaneous rates or cumulative totals
5. **Interactive Controls**: Allows sorting and changing display options on the fly
6. **Device Filtering**: Can monitor specific network interfaces
7. **Command Line Display**: Can show full command lines of processes

## Interactive Controls

When running, NetHogs supports these keyboard commands:
- `q`: Quit
- `s`: Sort by sent traffic
- `r`: Sort by received traffic
- `l`: Toggle command line display
- `m`: Switch between view modes (KB/s, total KB, etc.)
- `b`: Toggle showing only program basename

## Command Line Options

NetHogs supports numerous command-line options:
- `-V`: Print version
- `-h`: Print help
- `-d seconds`: Set refresh delay
- `-v mode`: Set view mode
- `-t`: Trace mode (outputs to stdout instead of ncurses UI)
- `-p`: Promiscuous mode
- `-s`: Sort by sent traffic
- `-l`: Display command line
- `-a`: Monitor all devices
- `-C`: Capture both TCP and UDP
- `-b`: Display program basename
- `-P pid`: Show only specific process(es)

## Library Support

The codebase also builds as a library (`libnethogs`), allowing other applications to use its functionality. This is marked as experimental in the README.

## Technical Details

1. **Refresh Rate**: Default refresh rate is 1 second
2. **Connection Timeout**: Connections are removed after 50 seconds of inactivity
3. **Process Timeout**: Processes are removed after 150 seconds of inactivity
4. **View Modes**: Supports KB/s, total KB, total bytes, total MB, MB/s, and GB/s
5. **Privileges**: Requires root access or specific capabilities (`cap_net_admin`, `cap_net_raw`, `cap_dac_read_search`, `cap_sys_ptrace`)

## Platform Support

- Full functionality on Linux
- Limited functionality on macOS and FreeBSD (shows connections but not processes)

## Build System

The project uses both a traditional Makefile and Meson build systems:
- Main dependencies: libpcap and ncurses
- Builds both the nethogs executable and libnethogs library

## Code Quality

The codebase follows the LLVM coding standards with some exceptions. The README notes that not all code adheres to this standard yet, and contributors are encouraged to use `clang-format` for new code.

## Unique Selling Point

NetHogs' main advantage is its ability to quickly identify bandwidth-hungry processes, making it invaluable for troubleshooting sudden network traffic spikes. As stated in the README: "If there's suddenly a lot of network traffic, you can fire up NetHogs and immediately see which PID is causing this."