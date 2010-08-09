This project contains the following targets:

* Skeleton 52221 -- skeleton code to run on an MCF52221, with USB host
                    and device drivers (source code)
* Skeleton 52233 -- skeleton code to run on an MCF52233, with Niche Lite
                    TCP/IP drivers (binary)
* Flasher (RAM)  -- RAM target to clone firmware to another MCU via
                    QSPI/EzPort
* Empty          -- empty project to hold places for header files, etc.,
                    outside of Link Order
                    
Note that the remaining targets are private, with no source code provided.
The Niche Lite TCP/IP stack is provided in binary form to make the skeleton
code stand alone.  If you have the v6.4 headers (available from Freescale),
you can use other features other than just accepting a raw connection for
the command-line.

The skeleton code provides a rudimentary command-line interface for the
MCU.  For the MCF52221, this is provided by a USB device driver and an
upper level FTDI device class driver that connects to a USB Virtual COM
Port on the host PC.  For the MCF52233, this is provided by the Niche
Lite TCP/IP stack that accepts a raw connection from a socket on the host
PC on port 1234.

The skeleton code also provides the following features:

* upgrade firmware via the terminal command-line
* clone firmware to another MCU via QSPI/EzPort
* a host mode USB driver and MST class driver to connect to USB devices
  when not using the command-line (MCF52221 only)
* programmatic access to flash memory; optional flash security
* lightweight printf to either terminal or CW debug console
* boot-time detection of debugger presence
* programmable interval timer, a/d converter, sleep mode example code

To run the Skeleton 52221 code, build the bits and flash the M52221DEMO
board.  Connect the Mini-AB connector on the board to the host PC.  Let
Windows auto-install the drivers (or alternatively, install them from
http://www.ftdichip.com/FTDrivers.htm).  Open HyperTerminal and connect
to the new Virtual Com Port.  Press <Enter> for a prompt (this may take
a few seconds).

To run the Skeleton 52233 code, build the bits and flash the M52233DEMO
board.  Connect the Ethernet connector on the board to your router.  Let
the board get an IP address from DHCP (query your DHCP server to figure
out which IP address it got).  Open HyperTerminal and connect to port
1234 of the obtained IP address.  Press <Enter> for a prompt (this may
take a few seconds).  You can then use the "ipaddress" command to set a
static IP address; you can override the static IP address and revert to
DHCP by holding SW2 depressed during boot.

Putty is an excellent alternative to HyperTerminal, and is available
from http://www.putty.org.  Be sure to use a raw connection and to force
local echo and line feeds off when talking to the MCF52233.

The code starts at startup.c.  For the MCF52221 bits, it continues to
main.c.  For the MCF52233 bits, on the other hand, it continues to the
Niche Lite code which starts a tasking system, and then resumes to
main.c.  From there code continues to the project-specific skeleton.c.

The general purpose sources files are as follows:

adc.[ch]       -- simple a/d converter driver
clone.[ch]     -- QSPI/EzPort CPU-to-CPU flash cloner
flash.[ch]     -- flash access and FTDI-based USB upgrade routines
ftdi.[ch]      -- FTDI device class driver
led.[ch]       -- trivial LED status driver
printf.[ch]    -- lightweight printf to terminal or CW debug console
scsi.[ch]      -- mst class host controller driver
sleep.[ch]     -- sleep mode driver
terminal.[ch]  -- vt100 terminal emulator driver (on FTDI or tcp/ip)
timer.[ch]     -- simple programmable timer interrupt driver
usb.[ch]       -- dual mode host controller/device driver
util.[ch]      -- basic utility routines

Once you install the real Freescale headers (MCF52221.h, MCF52235.h,
and their descendants) in your headers directory, you can turn off the
#define EXTRACT in main.h to begin using them.
