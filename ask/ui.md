# Understanding NetHogs Terminal Output

Based on the code snippets provided, I can explain how NetHogs displays information to the terminal.

## Overview of NetHogs UI

NetHogs is a "net top" tool that groups bandwidth usage by process rather than by protocol or subnet. It displays this information in a terminal-based UI using the ncurses library.

## Main Display Functions

The terminal output is primarily handled in `src/cui.cpp` through two main functions:

1. `show_ncurses()` - Displays the formatted output in the terminal UI
2. `show_trace()` - Used in trace/debug mode to output to stdout

## The UI Display Process

The main display logic happens in these steps:

1. In `do_refresh()` (lines 400-464), NetHogs:
   - Collects data about processes and their network usage
   - Creates `Line` objects for each process
   - Sorts them based on sent/received traffic
   - Calls either `show_ncurses()` or `show_trace()` depending on mode

2. The `show_ncurses()` function (lines 347-397):
   - Clears the screen
   - Prints the header with column titles
   - Calls `Line::show()` for each process
   - Adds a total row at the bottom

3. Each `Line::show()` method (lines 194-226):
   - Displays PID, username, program name, device, sent/received values
   - Uses `mvaddstr_truncate_cmdline()` to handle program names and command lines

## Key UI Components

### Header Row
```
    PID USER     PROGRAM                      DEV        SENT      RECEIVED       
```

### Process Rows
Each row shows:
- Process ID
- Username
- Program name (and optionally command line)
- Network device
- Sent traffic
- Received traffic
- Unit (KB/s, MB/s, etc.)

### Total Row
A summary row showing total sent/received traffic.

## Interactive Controls

The UI responds to keyboard input through `ui_tick()` (lines 301-328):
- `q` - Quit
- `s` - Sort by sent traffic
- `r` - Sort by received traffic
- `l` - Toggle command line display
- `m` - Switch between view modes (KB/s, total KB, etc.)
- `b` - Toggle showing only program basename

## Text Truncation

An important aspect of the display is how it handles long text:

- `mvaddstr_truncate_cmdline()` (lines 158-192) handles truncating program names and command lines
- `mvaddstr_truncate_trailing()` (lines 140-148) handles truncating other text fields

If text is too long, it's truncated and ".." is appended to indicate this.

## View Modes

NetHogs supports multiple view modes (KB/s, total KB, etc.) which are displayed in the rightmost column. The descriptions are defined in the `desc_view_mode` array (lines 70-71).

This terminal UI design allows NetHogs to provide a clear, real-time view of which processes are using network bandwidth, making it easy to identify bandwidth-hungry applications.