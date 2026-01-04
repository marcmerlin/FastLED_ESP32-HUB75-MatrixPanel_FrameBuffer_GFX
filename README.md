FastLED_ESP32-HUB75-MatrixPanel_FrameBuffer_GFX: FrameBuffer_GFX Driver for ESP32-HUB75-MatrixPanel
===================================================================================================

This is a display driver for Framebuffer::GFX that displays on RGB Panels using
https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-DMA . It runs on top of the same code that https://github.com/marcmerlin/Framebuffer_GFX and therefore provides access to the usual 
trio of APIs: FastLED, Adafruit::GFX (via Adafruit::NeoMatrix compatibility using FastLED::NeoMatrix)
and LEDMatrix compatibility too. To get example demo code, go to:
https://github.com/marcmerlin/FastLED_NeoMatrix_SmartMatrix_LEDMatrix_GFX_Demos

Here is a video demo of a 24bpp framebuffer based set of demos that simply can't run on ESP32-HUB75-MatrixPanel-DMA without this framebuffer driver:
https://www.youtube.com/watch?v=1d6U-6twpCk

How to use?
-----------
Please see this code to see how to use the framebuffer driver:
* This is where you edit or add a new matrix definition: https://github.com/marcmerlin/FastLED_NeoMatrix_SmartMatrix_LEDMatrix_GFX_Demos/blob/d416eb81f084631a9fc5023a7efaac34a9faeba4/neomatrix_config.h#L565
* Look for ESP32_HUB75_MATRIXPANEL_S3_PORTAL_64BY64 and ESP32_HUB75_MATRIXPANEL elsewhere in the file, although you should not need to modify anything but the first section with pin definition and layout, but if you have multiple kinds of panels, you're better off making new ifdefs for those new options
* the rest of the hub75matrix init code is in matrix_setup() which you call from your own setup(), and should not need to be modified: https://github.com/marcmerlin/FastLED_NeoMatrix_SmartMatrix_LEDMatrix_GFX_Demos/blob/d416eb81f084631a9fc5023a7efaac34a9faeba4/neomatrix_config.h#L1511

If you want something a bit simpler, you can look at https://github.com/marcmerlin/FastLED_ESP32-HUB75-MatrixPanel_FrameBuffer_GFX/blob/main/matrix_config.h  which will get basic demos working but it does not provide the full list of definitions and helper functions available in neomatrix_config.  It gets the work done for basic demos, you can look at BasicTest, which shows you the beauty of those include files, as the demo file is super short

Why not use mrcodetastic's library gives Adafruit::GFX compatibility?
---------------------------------------------------------------------
* Framebuffer_GFX gives you full compatiblity with FastLED, Adafruit::GFX, and LEDMatrix APIs

* As per its name, Framebuffer_GFX is a full framebuffer, so you can read back its content, scroll a frame or portion thereof (whereas, for memory and efficiency reasons, ESP32-HUB75-MatrixPanel is write only, like a TFT)

* Framebuffer_GFX comes with over 10 hardware backends that your code can run un-modified against (including native compile and running on linux). This means your code will run unmodified on plenty of other hardware ( 
So this layer on top of ESP32-HUB75-MatrixPanel allows you to write your 24bpp framebuffer code, and run it against ESP32-HUB75-MatrixPanel displays, as well as the 10+ other displays supported by https://github.com/marcmerlin/Framebuffer_GFX

![PXL_20260103_174726335](https://github.com/user-attachments/assets/21264cb2-3b2f-42da-a337-91aa91f7552f)
![PXL_20260103_175553151](https://github.com/user-attachments/assets/631d94b7-b759-45e5-86ec-2f8ca3c28528)

What you get is this:
```
Low Level Drv|Glue Driver for FrameBuffer::GFX
FastLED     - FastLED_NeoMatrix  -------------\     FastLED CRGB Array 
ESP32-HUB75-MatrixPanel -,                      |
FastLED_ESP32-HUB75-MatrixPanel_FrameBuffer_GFX
SmartMatrix - SmartMatrix_GFX -----------------\    24bit FB storage        API Support
                                                \   CRGB methods like
AF:ILI9341-\                                     \  scale8/fadeToBlackBy
AF:SSD1331  |--- FastLED_ArduinoGFX_TFT ----------|        |               ,FastLED API
AF:ST7735  /     FastLED_SPITFT_GFX (for AF)      |        |              / (XY 2D to 1D mapping)
ArduinoGFX/  AF:Adafruit (ArduinoGFX is all in 1) |        |             /
                                                  |        |            /
ArduinoOnPc-FastLED-GFX-LEDMatrix arduino         - FrameBuffer::GFX------ Adafruit::NeoMatrix +
emulation for linux / Raspberry Pi:               |        |             \ Adafruit::GFX APIs
----------------------------------               /    Adafruit::GFX       \ 
rpi-rgb-led-matrix - FastLED_RPIRGBPanel_GFX ---/   LEDMatrix (optional)   `LEDMatrix API
ArduinoOnPC X11/linux - FastLED_TFTWrapper_GFX /
FastLED_SDL (linux)   -  FastLED_NeoMatrix   -/
```
Other examples FastLED Matrix vs SmartMatrix matrix, still runnign the same code: 
<img width="1024" height="768" alt="image" src="https://github.com/user-attachments/assets/63b2daf4-1c63-42b5-aec7-3412aab30cf1" />

And a 3 panel smartmatrix vs a tiny TFT of the exact same 96x64 resolution, both running the same code:
<img width="1024" height="768" alt="image" src="https://github.com/user-attachments/assets/c9622e25-7d32-48c9-94c5-f50ba9c50036" />
(don't miss the small TFT at the bottom).

So you write against any of the 3 APIs in Framebuffer::GFX, and you get to run on all hardware backends
without having to change your code, including switching from teensy to ESP32, or rPi to display on RGBPanels.

I should stress that for RGBPanels, if you're writing code for ESP32 and ESP32-HUB75-MatrixPanel, you 
absolutely want to use this glue driver, because when you decide to scale up, you can switch to an rPi, 
get 3 parallel outputs for a display that is 3 times as large or refreshes 3 times faster, and all your code
will just work with no changes required outside of the matrix init.  
So the same code can run on these bigger displays (those run with rpi-rgb-panel):
![PXL_20260103_193506810](https://github.com/user-attachments/assets/fe9e9d5b-0362-48f5-aadf-e00f8004828f)  
https://youtu.be/MRRJw-Khkh0 

And one more reminder of how awesome it is to be able to compile your arduino code on linux, run it on linux
with full display, and debug it on linux with gdb and ASAN to find your memory leaks and dangling pointers. You
get this for free by using FrameBuffer::GFX for all your code.
