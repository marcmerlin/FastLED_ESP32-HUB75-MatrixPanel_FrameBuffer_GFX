/*--------------------------------------------------------------------
  Arduino library based on Adafruit_Neomatrix
  by Marc MERLIN <marc_soft@merlins.org>

  Original notice and license from Adafruit_Neomatrix:
  NeoMatrix is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  NeoMatrix is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with NeoMatrix.  If not, see
  <http://www.gnu.org/licenses/>.
  --------------------------------------------------------------------*/

#ifndef _FastLED_ESP32_HUB75_MatrixPanel_FrameBuffer_GFX_H_
#define _FastLED_ESP32_HUB75_MatrixPanel_FrameBuffer_GFX_H_
#include "Framebuffer_GFX.h"
#include "FastLED.h"
#include "ESP32-VirtualMatrixPanel-I2S-DMA.h"

class FastLED_ESP32_HUB75_MatrixPanel_FrameBuffer_GFX : public Framebuffer_GFX {
  public:
    FastLED_ESP32_HUB75_MatrixPanel_FrameBuffer_GFX(CRGB *, uint16_t, uint16_t, VirtualMatrixPanel *, MatrixPanel_I2S_DMA *);
    // These 2 methods are useful if the object is defined in global space, but the
    // framebuffer and hub75 objects don't exist yet because they get defined in setup()
    // when it's easier to allocate memory with malloc (on ESP32 more RAM is available via 
    // malloc than static array allocation in global space)
    void setvirtdisp(VirtualMatrixPanel *virtualDisp) { _virtdisp = virtualDisp; }
    void sethub75matrix(MatrixPanel_I2S_DMA *hub75matrix) { _hub75matrix = hub75matrix; }
    void show();
    void setBrightness(uint8_t b) { _hub75matrix->setBrightness(b); };

  protected:

  private:
    const uint16_t _fbw, _fbh;
    CRGB *__fb;
    VirtualMatrixPanel *_virtdisp;
    MatrixPanel_I2S_DMA *_hub75matrix;
};

#endif // _FastLED_ESP32_HUB75_MatrixPanel_FrameBuffer_GFX_H_
// vim:sts=4:sw=4
