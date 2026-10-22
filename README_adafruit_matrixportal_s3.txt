Arduino IDE compile options:
USB Mode: CDG and JTAG
CDC on boot: enabled (allows Serial console)
firmware MSC on boot: disabled
DFU on boot: disabled
upload mode: hardware CDC
Partition Scheme: 2MB App/3.7MB FATFS
Core Debug: Info
Port should be: ttyACMx MatrixPortal

First flash from arduino may not work, you need to
1) press and hold Boot push button in the middle of the board right of tricolor LED
2) push reset button (leftmost with USB at the bottom)
3) release boot
4) flash from arduino
5) after flash you have to push reset button again to get out of flash mode

Subsequent flashes will work without those buttons
