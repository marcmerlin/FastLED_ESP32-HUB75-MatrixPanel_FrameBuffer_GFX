
// Enable LEDMATRIX support in neomatrix_config
#define LEDMATRIX

// create this file with
// #define WIFI_SSID "foo"
// #define WIFI_PASSWORD "bar"
// #define WIFI_HOSTNAME screen1
#if __has_include("wifi_secrets.h")
    #include "wifi_secrets.h"
    #define WIFI_AP_CHANNEL 6
    #ifndef WIFI_HOSTNAME 
        #define WIFI_HOSTNAME "TME69-changeme"
    #endif
    #define WIFI_BEACON_ID "TME69"
    #include "wifi_serial_ota.h"
#endif

#include "neomatrix_config.h"
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
#include "Effects.h"
#include "Drawable.h"
#include "Boid.h"
#include "Attractor.h"
#include "Geometry.h"
#include "PatternAttract.h"
PatternAttract attract;
#include "PatternBounce.h"
PatternBounce bounce;
#include "PatternCube.h"
PatternCube cube;
#include "PatternFlock.h"
PatternFlock aflock;
#include "PatternFlowField.h"
PatternFlowField flowfield;
#include "PatternIncrementalDrift.h"
PatternIncrementalDrift incrementaldrift;
#include "PatternIncrementalDrift2.h"
PatternIncrementalDrift2 incrementaldrift2;
#include "PatternPendulumWave.h"
PatternPendulumWave pendulumwave;
#include "PatternRadar.h"
PatternRadar radar;
#include "PatternSpiral.h"
PatternSpiral spiral;
#include "PatternSpiro.h"
PatternSpiro spiro;
#include "PatternSwirl.h"
PatternSwirl swirl;
#include "PatternWave.h"
PatternWave wave;
#pragma GCC diagnostic pop


AuroraDrawable* items[] = {
  &attract,//0
  &bounce,//1
  &cube,//2
  &aflock,//3
  &flowfield,//4
  &incrementaldrift,//5
  &incrementaldrift2,//6
  &pendulumwave,//7
  &cube,//8  was radar
  &spiral,//9
  &spiro,//10
  &swirl,//11
  &wave,//12
};
AuroraDrawable *patternz;

// no-op for now, need to write code
bool DEMO_BESTOF = true;

int8_t item = random(13);

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
float scalerW = MATRIX_WIDTH / 64.0;
float scalerH = MATRIX_HEIGHT / 64.0;

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
CRGBPalette16 currentPalette( PartyColors_p );
uint8_t       colorLoop = 1;

float  mscale = 1.4, fps = 30;
float radius, xslope[MAXBIG], yslope[MAXBIG], xfire[MAXBIG], yfire[MAXBIG], cangle, sangle;
float locusx, locusy, driftx, drifty, xcen, ycen, yangle, xangle, eeksangle, whyangle;
float Gravity = -9.81, RATIO = (MATRIX_HEIGHT + 0.001) / (MATRIX_WIDTH + 0.001);
float Heighty[BallCount];
float ImpactVelocityStart = sqrt( -2 * Gravity );
float ImpactVelocity[BallCount];
float TimeSinceLastBounce[BallCount];
int16_t   Positiony[BallCount], Positionx[BallCount];
uint32_t  ClockTimeSinceLastBounce[BallCount];
float Dampening[BallCount];
CRGBPalette16 thepal;
CRGB darkcolor;
CRGB lightcolor;
char readchar;

#ifdef SHOW_PATTERN_NUM
uint8_t print_width = 3;
#endif

#include "Table_Mark_Estes_Impl.h"

void change_brightness(uint8_t nb) {
    matrix_brightness = nb;
    matrix->setBrightness(nb);
    Serial.printf("Set brightness to %d (not saving to nvram as high brightness can cause a crash loop due to power starvation)\n\r", nb);
}


// WIFI_HOSTNAME isn't actually used to set hostname anymore now that it's
// done from NVRAM, but it's still used to gate the WIFI code.
#ifdef WIFI_HOSTNAME

bool DEMO_PAUSE = false;
#define HTML_BESTOF	    100
#define HTML_BRIGHT	    101
#define HTML_PAUSE	    102
#define HTML_REBOOTESP32    109
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
	rebootESP32();
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

volatile uint32_t pause_until_millis = 0;

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
#endif

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
  matrix->setTextColor(matrix->Color(0, 0,255));
  matrix->println(get_hostname().c_str());
  matrix->show(); 
  matrix->setTextColor(matrix->Color(255,255,255));
  // stop the main thread for 3 seconds
  pause_until_millis = millis() + 3000;
}

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

void setup()
{
  x = random16();
  y = random16();
  z = random16();
  if (MATRIX_WIDTH == MATRIX_HEIGHT) square = true;
  //delay(1500);
  Serial.begin(115200);

#ifdef TME_AUDIO
  Serial1.begin(57600);

  ETin.begin(details(music), &Serial1);
  randomSeed(analogRead(1) - analogRead(2) + analogRead(5));
  pinMode(LATCHA, OUTPUT);
  digitalWrite(LATCHA, LOW);    // sets the audio module hardware to off
  pinMode(LATCHB, OUTPUT);
  digitalWrite(LATCHB, LOW);
#endif

#if 0
  Serial.println("Reset");
  Serial.println("scalerW, iscalerW");
  Serial.println(scalerW);
  Serial.println(iscalerW);
  Serial.println("scalerH, iscalerH");
  Serial.println(scalerH);
  Serial.println(iscalerH);
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

  ////////////////////////////////////////////////////////////////////////////
  // Matrix Init
  ////////////////////////////////////////////////////////////////////////////
  // don't re-init the serial port.
  matrix_setup(false);
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

  effects.leds = matrixleds;
  effects.Setup();
  matrix->show();

  // Do not init wifi before matrix_setup since wifi has callbacks that display on matrix
  #ifdef WIFI_HOSTNAME
    wifi_setup(rebuild_main_page_local, on_ota_start, on_wifi_connect);
  #endif

#ifdef SHOW_PATTERN_NUM
  // default font is 5x7, this one is 3x5
  matrix->setFont(&TomThumb);
#endif
  // Disable showing FPS on serial port in Framebuffer_GFX, but compute it
#ifdef DISPLAY_FPS
  matrix->setfpsfreq(0);
#endif


  display_hostname();
}

void marc_overlay() {
  matrix->fillRect(0, 0, 64, 8, 0);
  // Reset to default adafruit font, that font wants cursor at top left corner
  matrix->setFont();
  matrix->setCursor(0, 0);
  matrix->setTextColor(matrix->Color(0,255,0));
  matrix->print("Marc");
  matrix->setCursor(28, 0);
  matrix->setTextColor(matrix->Color(0,0,255));
  matrix->print("MERLIN");
  matrix->fillRect(0, 55, 64, 9, 0);
  matrix->setCursor(0, 56);
  matrix->setTextColor(matrix->Color(255,255,0));
  matrix->print("SuperCon");
  matrix->setCursor(51, 56);
  matrix->setTextColor(matrix->Color(255,0,0));
  matrix->print("26");
}


void loop()
{
  wifi_loop();
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

  marc_overlay();

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
