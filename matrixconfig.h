#ifndef MATRIX_CONFIG
#define MATRIX_CONFIG
#include <FastLED_ESP32-HUB75-MatrixPanel_FrameBuffer_GFX.h>

// Step 1) Provide the size of each individual physical panel LED Matrix panel that is chained (or not) together
#define PANEL_RES_X 64 // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 64 // Number of pixels tall of each INDIVIDUAL panel module.

// Step 2) Provide details of the physical panel chaining that is in place.
#define NUM_ROWS 1 // Number of rows of chained INDIVIDUAL PANELS
#define NUM_COLS 1 // Number of INDIVIDUAL PANELS per ROW
#define PANEL_CHAIN NUM_ROWS*NUM_COLS    // total number of panels chained one to another

// Step 3) How are the panels chained together?
#define PANEL_CHAIN_TYPE CHAIN_TOP_RIGHT_DOWN

// Refer to: https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-DMA/tree/master/examples/VirtualMatrixPanel
//      and: https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-DMA/blob/master/doc/VirtualMatrixPanel.pdf

#define VPANEL_W PANEL_RES_X*NUM_COLS
#define VPANEL_H PANEL_RES_Y*NUM_ROWS

// Step 4) Pin definitions
// custom pin mapping (this one is Adafruit Matrixportal ESP32S3
#define R1 42
#define G1 41
#define B1 40
#define R2 38
#define G2 39
#define B2 37
#define CH_A 45
#define CH_B 36
#define CH_C 48
#define CH_D 35
#define CH_E 21
#define CLK 2
#define LAT 47
#define OE  14

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// CONFIG STOPS HERE. Everything below is common code to all demos
///
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef LEDMATRIX
// Please use https://github.com/marcmerlin/LEDMatrix/ at least as recent as
// https://github.com/marcmerlin/LEDMatrix/commit/597ce703e924d45b2e676d6558c4c74a8ebc6991
// or https://github.com/Jorgen-VikingGod/LEDMatrix/commit/a11e74c8cd5b933021b6e15eb067280a52691449
// zero copy/no malloc code to work.
#include <LEDMatrix.h>
#endif

HUB75_I2S_CFG::i2s_pins _pins={R1, G1, B1, R2, G2, B2, CH_A, CH_B, CH_C, CH_D, CH_E, LAT, OE, CLK};

/// Please ignore the "TILE" part, for externally managed framebuffers like RGBPanels,
/// the resolution is coded here.
const uint16_t MATRIX_TILE_WIDTH = VPANEL_W;
const uint16_t MATRIX_TILE_HEIGHT= VPANEL_H;

// and these are used by LEDMatrix for Neopixel Matrices but do not change from 1 for RGBPanels
const uint8_t MATRIX_TILE_H     = 1;  // number of matrices arranged horizontally
const uint8_t MATRIX_TILE_V     = 1;  // number of matrices arranged vertically

// Used by NeoMatrix
const uint16_t mw = MATRIX_TILE_WIDTH *  MATRIX_TILE_H;
const uint16_t mh = MATRIX_TILE_HEIGHT * MATRIX_TILE_V;

// Used by some demos
const uint32_t NUMMATRIX = mw*mh;
const uint32_t NUM_LEDS = NUMMATRIX;

MatrixPanel_I2S_DMA *hub75matrix = nullptr;
VirtualMatrixPanel  *virtualDisp = nullptr;

#ifdef LEDMATRIX
// cLEDMatrix creation, don't allocate memory since we will feed it the framebuffer
// created by SmartMatrix at runtime ledmatrix(false)
cLEDMatrix<MATRIX_TILE_WIDTH, -MATRIX_TILE_HEIGHT, HORIZONTAL_MATRIX,
    MATRIX_TILE_H, MATRIX_TILE_V, HORIZONTAL_BLOCKS> ledmatrix(false);
#endif
CRGB *matrixleds = nullptr;

// Neither matrixleds point to anything useful yet because on ESP32 not as much
// RAM is available as static array allocation in global space, as RAM you can
// get via malloc in setup (or even ps_malloc to use PSRAM as needed).
// So we'll call Framebuffer_GFX->newLedsPtr and setvirtdisp to set those pointers correctly
// Same thing with hub75matrix, which is passed so we can call brightness
FastLED_ESP32_HUB75_MatrixPanel_FrameBuffer_GFX *matrix = new FastLED_ESP32_HUB75_MatrixPanel_FrameBuffer_GFX(matrixleds, MATRIX_TILE_WIDTH, MATRIX_TILE_HEIGHT, virtualDisp, hub75matrix);

// ----------------------------------------------------------------------------

void show_free_mem(const char *pre=NULL) {
    Framebuffer_GFX::show_free_mem(pre);
}

void die(const char *mesg) {
    Serial.println(mesg);
    while(1) delay((uint32_t)1); // while 1 loop only triggers watchdog on ESP chips
}

void *mallocordie(const char *varname, uint32_t req, bool psram=true) {
    // If varname starts with @, show debug for the allocation
    void *mem;

#ifndef BOARD_HAS_PSRAM
    psram = false;
#endif
    if (varname[0] == '@') {
        if (psram) Serial.print("PS");
        Serial.print("Malloc ");
        Serial.print(varname);
        Serial.print(" . Requested bytes: ");
        Serial.println(req);
    }
#ifdef ESP32
    if (psram) {
        mem = ps_malloc(req);
    } else {
        mem = calloc(1, req);
    }
#else
    mem = calloc(1, req);
#endif

    if (mem) {
        return mem;
    } else {
        show_free_mem();
        Serial.println("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv");
        Serial.print("FATAL: ");
        if (psram) Serial.print("ps_");
        Serial.print("malloc failed for ");
        Serial.print(varname);
        Serial.print(" . Requested bytes: ");
        Serial.println(req);
        Serial.println("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
        while (1); // delay(1);  Adding this seems to cause an ESP32 bug
    }
    return NULL;
}

void matrix_setup(bool initserial=true, int reservemem = 40000) {
    reservemem = reservemem; // squelch compiler warning if var is unused.

    if (initserial) {
	Serial.begin(115200);
        Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Serial.begin");
	delay(250);
    }

    Serial.println("Matrix FB Init");
    // ESP32 has more memory available for allocation in setup than in global
    // (if this were a global array), so we use malloc here.
    // https://forum.arduino.cc/index.php?topic=647833
    matrixleds = (CRGB *) mallocordie("matrixleds", sizeof(CRGB) * NUMMATRIX, 1);
    // and then fix the until now NULL pointer in the object.
    Serial.print("Setting Matrix framebuffer to ");
    Serial.print((uint32_t) matrixleds);
    Serial.print(" for ");
    Serial.print(NUMMATRIX);
    Serial.println(" pixels");
    // Now we can finally set the framebuffer pointer (without this, the lib will crash)
    matrix->newLedsPtr(matrixleds);

    show_free_mem("After matrixleds malloc");
    #ifdef LEDMATRIX
        Serial.println("ledmatrix.SetLEDArray");
        ledmatrix.SetLEDArray(matrixleds);
    #endif

    Serial.println("HUB75 Matrix Init");
    // Configure your matrix setup here
    HUB75_I2S_CFG mxconfig(PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN);

    mxconfig.gpio = _pins;
    // Can help with an off by one pixel display error on some panels
    mxconfig.clkphase = false;

    // OK, now we can create our matrix object
    hub75matrix = new MatrixPanel_I2S_DMA(mxconfig);

    // Allocate memory and start DMA display
    if (! hub75matrix->begin()) die ("****** !KABOOM! I2S memory allocation failed ***********");
    matrix->sethub75matrix(hub75matrix);

    // create VirtualDisplay object based on our newly created dma_display object
    virtualDisp = new VirtualMatrixPanel((*hub75matrix), NUM_ROWS, NUM_COLS, PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN_TYPE);
    // Same comment as setfb above.
    matrix->setvirtdisp(virtualDisp);

    Serial.print("MATRIX_WIDTH: ");  Serial.println(PANEL_RES_X*PANEL_CHAIN);
    Serial.print("MATRIX_HEIGHT: "); Serial.println(PANEL_RES_Y);
    Serial.println("VIRTUAL PANEL WIDTH " + String(VPANEL_W));
    Serial.println("VIRTUAL PANEL HEIGHT " + String(VPANEL_H));

    Serial.println("HUB75 Virtual Matrix Init Done");
    delay(1000);
}

#endif // MATRIX_CONFIG
