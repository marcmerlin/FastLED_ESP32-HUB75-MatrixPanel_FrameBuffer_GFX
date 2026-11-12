// Enable LEDMATRIX support in neomatrix_config
#define LEDMATRIX

// create this file with
// #define WIFI_SSID "foo"
// #define WIFI_PASSWORD "bar"
// #define WIFI_HOSTNAME screen1
#if __has_include("wifi_secrets.h") && !defined(ARDUINO_ARCH_RP2040)
    #define WIFI
    #include "wifi_secrets.h"
    #define WIFI_AP_CHANNEL 6
    #ifndef WIFI_HOSTNAME 
        #define WIFI_HOSTNAME "TME69-changeme"
    #endif
    #define WIFI_BEACON_ID "TME69"
    #include "wifi_serial_ota.h"
#endif

#include "neomatrix_config.h"
//#include "matrixconfig.h"
#define zeds ledmatrix

#include "Table_Mark_Estes_config.h"

//#define SHOW_PATTERN_NUM uses Adafruit GFX fonts
#include <Fonts/TomThumb.h>

// TME now uses LEDText from LEDMatrix (a competing library)
#include <LEDText.h>
// Not all fonts are used, but the compiler should keep the unused ones

//  is only availble on teensy v4, turn it off elsewhere
/*#ifndef __IMXRT1062__
  #define EXTMEM
  #endif
*/


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
// Aurora includes
#include "matrix.h"
#include "Drawable.h"
#include "Boid.h"
#include "Attractor.h"
#include "Geometry.h"
#include "PatternAttract.h"
#include "PatternBounce.h"
#include "PatternCube.h"
#include "PatternFlock.h"
#include "PatternFlowField.h"
#include "PatternIncrementalDrift.h"
#include "PatternIncrementalDrift2.h"
#include "PatternPendulumWave.h"
#include "PatternRadar.h"
#include "PatternSpiral.h"
#include "PatternSpiro.h"
#include "PatternSwirl.h"
#include "PatternWave.h"
#pragma GCC diagnostic pop

extern Effects *effects_ptr;
#define effects (*effects_ptr)

AuroraDrawable* items[13];
AuroraDrawable *patternz;


// no-op for now, need to write code
bool DEMO_BESTOF = true;

int8_t item = 0; // random(13);  cannot use random at global scope on RP2040

#define MIDLX               (MATRIX_WIDTH/2)
#define MIDLY               (MATRIX_HEIGHT/2)
#define mpatterns           (199)// max number of patterns
#define BallCount           20
#define BIGGER              mmax(MATRIX_WIDTH, MATRIX_HEIGHT)
#define MAXBIG              BIGGER * 2


#ifdef TME_AUDIO
EasyTransfer ETin;
#endif
struct RECEIVE_DATA_STRUCTURE {
  byte laudio[17];
  byte raudio[17];
  byte target;
  byte peakL;
  byte peakR;
};
RECEIVE_DATA_STRUCTURE music;

int16_t slowest = 2, fastest = 15, cool,  bpm[34], targetfps = 10, adjunct = 3, sizzer, sizzer2;
int16_t sparky = 80, ccc, xxx, yyy, dot = 3, radius2, rr, gg, bb;
int16_t  maxiaud, pointy = 4,  hue, steper = 2,  xblender, hhowmany = 3, blender = 120, radius3,  quietcount, ccoolloorr,  h = 0;
int16_t RC1, RC2, RC3, dot4 = 5, dot2 = 6,  dot3 = 1, phew, lender = 128, xsizer, ysizer, xx,  yy, flipme = 1, shifty = 4,  poffset, wind = 2, fancy , sinewidth = 3;
uint8_t  whichone = 2, regen = 0, mstep, LLaudio[MAXBIG], RRaudio[MAXBIG], fpeed[MAXBIG], fcolor[MAXBIG], fcountr[MAXBIG];

uint16_t raad, howmany, xhowmany, how, velo = 30 , pointyfix = 4;
//float scalerW = MATRIX_WIDTH / 64.0;   // floating point is illegal at global scope on RP2040
//float scalerH = MATRIX_HEIGHT / 64.0;
// moved to setup
float scalerW = 1;
float scalerH = 1;

int8_t zoomer = 1, bigmax, directn = 1, quash = 5, quiet = 0 ;
uint16_t  counter, ringdelay, bringdelay, firedelay, hitcounter, waiter = 7;


uint8_t  inner, bfade = 3,  acount,  iscalerW = int(scalerW + .1), iscalerH = int(scalerH + .1);

uint8_t divider, kind[BallCount], xpoffset[MAXBIG], fvelo[MAXBIG];

int16_t xvort[MAXBIG], yvort[MAXBIG];
uint16_t fcount[MAXBIG];

boolean square = false, gotmusic =  false, flop[13],   nextsong = false;
boolean rimmer[MAXBIG], xbouncer[MAXBIG], ybouncer[MAXBIG];
boolean   mixit = false, slowme = true;

uint32_t carlton,  lasttest, lastmilly, dwell = TIMING * 600;
uint16_t Nspeed = 3; // speed is set dynamically once we've started up
uint16_t pscale = 5; // scale is set dynamically once we've started up
static uint16_t x;
static uint16_t y;
static uint16_t z;
uint8_t noise[BIGGER][BIGGER];

// gemini says copying this at global scope is unsafe on RP2040, moved to setup
//CRGBPalette16 currentPalette( PartyColors_p );
CRGBPalette16 currentPalette;
uint8_t       colorLoop = 1;

// Define the actual pointer for the Effects macro we added in the header
Effects *effects_ptr = nullptr;


float  mscale = 1.4, fps = 30;
float radius, xslope[MAXBIG], yslope[MAXBIG], xfire[MAXBIG], yfire[MAXBIG], cangle, sangle;
float locusx, locusy, driftx, drifty, xcen, ycen, yangle, xangle, eeksangle, whyangle;
// float Gravity = -9.81, RATIO = (MATRIX_HEIGHT + 0.001) / (MATRIX_WIDTH + 0.001);
float Gravity = -9.81, RATIO = 1;
float Heighty[BallCount];
float ImpactVelocityStart;
float ImpactVelocity[BallCount];
float TimeSinceLastBounce[BallCount];
int16_t   Positiony[BallCount], Positionx[BallCount];
uint32_t  ClockTimeSinceLastBounce[BallCount];
float Dampening[BallCount];
CRGBPalette16 thepal;
CRGB darkcolor;
CRGB lightcolor;
volatile char readchar;

#ifdef SHOW_PATTERN_NUM
uint8_t print_width = 3;
#endif

void change_brightness(uint8_t nb) {
    matrix_brightness = nb;
    matrix->setBrightness(nb);
    Serial.printf("Set brightness to %d (not saving to nvram as high brightness can cause a crash loop due to power starvation)\n\r", nb);
}

#include "Table_Mark_Estes_Impl.h"


volatile uint32_t pause_until_millis = 0;
#ifdef WIFI
  bool DEMO_PAUSE = false;
  #define HTML_BESTOF	    100
  #define HTML_BRIGHT	    101
  #define HTML_PAUSE	    102
  #define HTML_REBOOTESP32  109
  #define HTML_BUTPREV	    110
  #define HTML_BUTNEXT	    111
  
  void actionProc(const char *pageName, const char *parameterName, int value, int ref1, void *ref2) {
      static int actionCount = 0;
      actionCount++;
      Serial.printf("%4d %s.%s value=0x%08x/%dslider 2 ref1=%d\n", actionCount, pageName, parameterName, value, value, ref1);
  
      switch (ref1) {
      case HTML_BUTPREV:
  	if (!value) break;
  	Serial.println("PREVIOUS pressed");
          readchar = 'p';
          newpattern();
  	break;
  
      case HTML_BUTNEXT:
  	if (!value) break;
  	Serial.println("NEXT pressed");
          readchar = 'n';
          newpattern();
  	break;
  
      case HTML_REBOOTESP32:
  	if (!value) break;
  	Serial.println("Reboot ESP32 pressed");
  	reboot();
  	break;
  
      case HTML_BESTOF:
  	if (!value) break;
  	Serial.print("Bestof: ");
  	Serial.println(value);
  	Serial.println("Write me");
  	break;
  
      case HTML_BRIGHT:
  	Serial.print("Brightness change to ");
  	Serial.println(value);
  	change_brightness(value);
  	break;
  
      case HTML_PAUSE:
          DEMO_PAUSE = value;
          if (value) {
              Serial.println("Pausing Demo");
          } else { 
              Serial.println("Resuming Demo");
              readchar = 'n';
              newpattern();
          }
  	break;
      }
  }
  
  void on_ota_start() {
      Serial.println("\r\nGot OTA - Executing Matrix Shutdown Callback");
      
      matrix->fillScreen(0);
      matrix->setFont(&TomThumb);
      matrix->setCursor(1, 6);
      matrix->print("Received OTA\nFlashing...");
      matrix->show(); 
      
      // just long enough to show it but not long enough to stall the OTA
      delay(1000);
  
      // 1. MUST blank the screen first to prevent LED burn-in!
      matrix->fillScreen(0);
      matrix->show(); 
  
      // 2. Allow one full frame to clock out to the hardware
      delay(50);
  
      // Shut down the I2S DMA engine to free the memory bus for the SPI flash
      // Note that this is not absolutely required but flashing is super slow
      // if the I2S DMA keeps running.
      if (hub75matrix) hub75matrix->stopDMAoutput();
  }
  
  void on_wifi_connect() {
      Serial.println("\r\nWifi Callback");
      
      // stop the main thread for 3 seconds
      pause_until_millis = millis() + 3000;
      matrix->setFont(&TomThumb);
      if (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA) {
          matrix->fillRect(0, 0, 64, 14, 0);
          matrix->setTextColor(matrix->Color(255,0,0));
          matrix->setCursor(0, 6);
          matrix->print(String(">>>> AP MODE <<<<"));
          matrix->setCursor(0, 13);
          matrix->print(String("> ") + WiFi.softAPIP().toString().c_str() + " <");
          matrix->fillRect(0, 64-7, 64, 7, 0);
          matrix->setCursor(0, 63);
          matrix->setTextColor(matrix->Color(255,255,0));
          matrix->print(String(">>>> AP MODE <<<<"));
      } else {
          matrix->fillRect(0, 0, 64, 7, 0);
          matrix->setTextColor(matrix->Color(0,255,0));
          matrix->setCursor(0, 6);
          matrix->print(String("> ") + WiFi.localIP().toString().c_str() + " <");
      }
      matrix->setTextColor(matrix->Color(255,255,255));
      matrix->show(); 
  }
#endif // WIFI

void display_hostname() {
  matrix->fillRect(0, 64-7*3, 64, 21, 0);
  matrix->setFont(&TomThumb);
  matrix->setCursor(0, 64-2*7);
  matrix->setTextColor(matrix->Color(255,0,0));
  matrix->println("TME 69 Demo");
#ifdef WIFI_HOSTNAME
  // This is the build time hostname, not used for anything else anymore
  matrix->setTextColor(matrix->Color(0,255,0));
  matrix->println(WIFI_HOSTNAME);
#endif
#ifdef WIFI
  matrix->setTextColor(matrix->Color(0, 0,255));
  matrix->println(get_hostname().c_str());
#endif
  matrix->show(); 
  matrix->setTextColor(matrix->Color(255,255,255));
  // stop the main thread for 3 seconds
  pause_until_millis = millis() + 3000;
}

#ifdef WIFI
void rebuild_main_page_local() {
    WebPages->addSlider(0, 1, "Bestof Demos Only", actionProc, DEMO_BESTOF, HTML_BESTOF);
    WebPages->addSlider(0, 1, "Pause on Demo", actionProc, DEMO_PAUSE, HTML_PAUSE);
    WebPages->addSlider(0, 255, "Brightness", actionProc, matrix_brightness, HTML_BRIGHT);
    /*
	 A button sends a value "1" when pressed (in the web page),
	 and zero when released.
    */
    WebPages->addButton("prev", actionProc, HTML_BUTPREV);
    WebPages->addButton("next", actionProc, HTML_BUTNEXT);
    WebPages->addButton("reboot", actionProc, HTML_REBOOTESP32);
}
#endif

void setup()
{

  ////////////////////////////////////////////////////////////////////////////
  // Matrix Init
  ////////////////////////////////////////////////////////////////////////////
  // 1. Initialize serial port
  // Note that the serial port is likely starved by Protomatter, so most serial
  // is lost. TODO: move Protomatter to setup1() and loop1()
  Serial.begin(115200);
  
  // 2. Allow up to 4 seconds for Linux host CDC-ACM enumeration
  #ifndef ESP32
  uint32_t t0 = millis();
  while (!Serial && (millis() - t0 < 4000)) {
    delay(500);
    Serial.println("Priming Serial (especially for RP2xxx chips)");
    Serial.flush();
  }
  #endif

  // this is dirty but needed. On RP2350, Serial returns ok, but the end to 
  // end connection isn't working and the boot messages are lost without this
  delay(1000);

  Serial.println("\n\n============================================");
  Serial.println("     HARDWARE BOOT OK: Entering setup()");
  Serial.println("============================================");
  Serial.flush();

  // 3. Initialize matrix and memory
  matrix_setup(false);
  Serial.println(">> matrix_setup() completed successfully.");
  Serial.flush();

  // 4. Visual Test: Draw 3 distinct colored stripes across the 64x64 panel
  // to confirm bitplane DMA and shift register timing are active.
  matrix->fillScreen(0);
  matrix->fillRect(0,  0, 64, 16, matrix->Color(255, 0, 0));   // Red top
  matrix->fillRect(0, 16, 64, 16, matrix->Color(0, 255, 0));   // Green middle
  matrix->fillRect(0, 32, 64, 16, matrix->Color(0, 0, 255));   // Blue bottom
  matrix->show();
  // Why this? Because, some chips like RP2xxx will fail to get serial working
  // for a variety of reasons, making it hard to debug
  // But worse, if any global variables happen to do floating point, or random()
  // they will simply crash the CPU before setup() gets any chance to even run.
  // So this is basically a 'hello world' in case serial is busted but the CPU
  // didn't crash and things fail later.

  Serial.println("Test pattern pushed via matrix->show().");
  Serial.flush();

  x = random16();
  y = random16();
  z = random16();
  if (MATRIX_WIDTH == MATRIX_HEIGHT) square = true;

  // RP2040: NOW it is safe to do hardware random and FPU math
  ImpactVelocityStart = sqrt( -2 * Gravity );
  item = random(13);

// Safely construct Effects now that hardware is awake
effects_ptr = new Effects();

  // Construct patterns in permanent memory using 'static'
  static PatternAttract attract;
  static PatternBounce bounce;
  static PatternCube cube;
  static PatternFlock aflock;
  static PatternFlowField flowfield;
  static PatternIncrementalDrift incrementaldrift;
  static PatternIncrementalDrift2 incrementaldrift2;
  static PatternPendulumWave pendulumwave;
  static PatternRadar radar;
  static PatternSpiral spiral;
  static PatternSpiro spiro;
  static PatternSwirl swirl;
  static PatternWave wave;

  // Bind their addresses to the global items array
  items[0] = &attract;
  items[1] = &bounce;
  items[2] = &cube;
  items[3] = &aflock;
  items[4] = &flowfield;
  items[5] = &incrementaldrift;
  items[6] = &incrementaldrift2;
  items[7] = &pendulumwave;
  items[8] = &cube; // (was radar)
  items[9] = &spiral;
  items[10] = &spiro;
  items[11] = &swirl;
  items[12] = &wave;

#ifdef TME_AUDIO
  Serial1.begin(57600);

  ETin.begin(details(music), &Serial1);
  randomSeed(analogRead(1) - analogRead(2) + analogRead(5));
  pinMode(LATCHA, OUTPUT);
  digitalWrite(LATCHA, LOW);    // sets the audio module hardware to off
  pinMode(LATCHB, OUTPUT);
  digitalWrite(LATCHB, LOW);
#endif

  driftx = random(3, MATRIX_WIDTH - 4);//set an initial location for the animation center
  drifty = random(3, MATRIX_HEIGHT - 4);// set an initial location for the animation center
  mstep = byte(256 / (MATRIX_WIDTH - 1)); //mstep is the step size to distribute 256 over an array the width of the matrix

  steper = random8(2, 8);// steper is used to modify h to generate a bigger color step on each move  early stuff not so common any more
  for (byte j = 0; j < 13; j++) {
    flop[j] = false;
    if (random8() < 128) flop[j] = true;
  }

  flop[1] = true;
  flop[6] = false;
  flop[1] = false;
  flop[2] = true;
  flop[3] = false;
  flop[5] = true;


  lastmilly = millis();
  lasttest = millis();
#ifdef TME_AUDIO
  digitalWrite(LATCHB, HIGH);
  digitalWrite(LATCHA, HIGH);
#endif
  hue = random8();//get a starting point for the color progressions

  cangle = (sin8(random(25, 220)) - 128.0) / 128.0;//angle of movement for the center of animation gives a float value between -1 and 1
  sangle = (sin8(random(25, 220)) - 128.0) / 128.0;//angle of movement for the center of animation in the y direction gives a float value between -1 and 1

  whatami();//this prints out the current status of stuff

#ifdef TME_AUDIO
  audioprocess();

  digitalWrite(LATCHA, LOW);
  digitalWrite(LATCHB, LOW);
#endif

  // Safely initialize globals now that crt0 is done and XIP/FPU are active
  scalerW = MATRIX_WIDTH / 64.0;
  scalerH = MATRIX_HEIGHT / 64.0;
  currentPalette = PartyColors_p;

#ifdef NVRAM
  Serial.println("Adjusting Brightness, if your panels are powered by USB and things crash before you");
  Serial.println("can reset it with b and B, unplug power to your panels to allow boot and adjustments");
  uint8_t nb = get_nvram_brightness();
  if (nb) {
    Serial.printf("Replacing default brightness %d with NVRAM saved brightness %d\n\r", matrix_brightness, nb);
    change_brightness(nb);
  } else {
    Serial.printf("Mo brightness saved in NVRAM, using neomatrix_config.h default of %d\n\r", matrix_brightness);
    Serial.printf("Use 'b' to change brightness and 'B' to save to NVRAM\n\r");
  }
#endif

  effects.leds = matrixleds;
  effects.Setup();

  // Do not init wifi before matrix_setup since wifi has callbacks that display on matrix
  #ifdef WIFI
    wifi_setup(rebuild_main_page_local, on_ota_start, on_wifi_connect);
  #endif

  matrix->clear();
  display_hostname();
  matrix->show();
}

// void local_overlay() {}
// defined here
#include "local_overlay.h"

void loop()
{
  #ifdef WIFI
    wifi_loop();
  #endif
  if (millis() < pause_until_millis) {
    delay(100);
    EVERY_N_SECONDS(1) Serial.println("Pausing main loop to show other thread output");
    return;
  }


  hue += 1;//increment the color basis
  h = hue;  //set h to the color basis
  checkshifty();
  // test for frame rate,  every 8 frames
  checktempo();

  checkfader();

  checkfancy();

  checkwind();

  if (slowme)
    slowmo();
  EVERY_N_SECONDS(12) {

    cangle = random(25, 85) / 100.0;
    sangle = random(25, 85) / 100.0;
    if (random8() > 128) cangle = -cangle;
    if (random8() > 128) sangle = -sangle;
    blender += random(4, 12);
    xblender -= random(4, 12);
  }

  runpattern();//go generate a updted screen
  /*if (fancy == 28) president2();
    if (fancy == 29) president();
    if (fancy == 30) president2();
    if (fancy == 31) president();*/
  counter++;//increment the counter which is used for many things
  // spititout();//map the 2d screen to the 8 outpus and push them out on my other matrix, not used here

  local_overlay();

  uint16_t update_freq = matrix->fps();
#ifdef SHOW_PATTERN_NUM
  // Make dark overlay for the white font to be visible
  matrix->fillRect(0, 0, 4 * print_width, 7, 0);
  matrix->setFont(&TomThumb);
  matrix->setCursor(0, 6);
  matrix->setTextColor(matrix->Color(255,255,255));
  matrix->print(pattern);
#endif
#ifdef DISPLAY_FPS
  // Make dark overlay for the white font to be visible
  matrix->fillRect(MATRIX_WIDTH - 1 - 4 * 3, 0, 4 * 3 + 1, 7, 0);
  matrix->setFont(&TomThumb);
  matrix->setCursor(MATRIX_WIDTH - 4 * 3, 6);
  matrix->setTextColor(matrix->Color(255,255,255));
  matrix->print(update_freq);
#endif

  EVERY_N_SECONDS(30) {
    // On RP2350  at 125Mhz, demo 82 at 64x64 updates at 7-10 fps (TODO move RGBPanel ISR to 2nd core)
    // On ESP32-S3 @ 240Mhz, demo 82 at 64x64 updates at 7-10 fps (TODO move main code to core0?)
    Serial.printf("\r\nFramebuffer::GFX fps: %d\r\n", update_freq);
    // 64x64 5BPP on RP2350   @125Mhz is 224 FPS
    // 64x64 5BPP on ESP32-S3 @240Mhz is 732 FPS!!!
    #if defined(ARDUINO_ARCH_RP2040) || defined(ESP32)
        Serial.printf(">>>> Panel FPS: %d <<<<\r\n", matrix->backend_fps());
    #endif
  }
  matrix->show();

  delay(waiter);//frame rate control
  if (Serial.available()) readchar = Serial.read(); else readchar = 0;
  if (readchar > 31 || millis() > lastmilly + dwell || nextsong) //when to change patterns
  {
    Serial.print(", Actual FPS: ");
    Serial.print (fps, 2);

    if (regen > 0 || readchar > 31 ) {
      Serial.print(",  count:");
      carlton++;
      Serial.println(carlton);
      newpattern();
    }
    else {
      Serial.print (", same: ");
      Serial.print(",  count:");
      carlton++;
      Serial.println(carlton);

      regenpattern();
    }
  }
}
