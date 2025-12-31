/*-------------------------------------------------------------------------
  Arduino library based on Adafruit_Neomatrix
  by Marc MERLIN <marc_soft@merlins.org>

  You should have received a copy of the GNU Lesser General Public
  License along with NeoMatrix.  If not, see
  <http://www.gnu.org/licenses/>.
  -------------------------------------------------------------------------*/

#include <FastLED_ESP32-HUB75-MatrixPanel_FrameBuffer_GFX.h>

FastLED_ESP32_HUB75_MatrixPanel_FrameBuffer_GFX::FastLED_ESP32_HUB75_MatrixPanel_FrameBuffer_GFX(CRGB *__fb, const uint16_t fbw, const uint16_t fbh, VirtualMatrixPanel *virtualDisp, MatrixPanel_I2S_DMA *hub75matrix):
  Framebuffer_GFX(__fb, fbw, fbh, NULL), _fbw(fbw), _fbh(fbh), _virtdisp(virtualDisp), _hub75matrix(hub75matrix) {
}

void FastLED_ESP32_HUB75_MatrixPanel_FrameBuffer_GFX::show() {
    for (uint16_t y = 0; y < _fbh; y++) {
	for (uint16_t x = 0; x < _fbw; x++) {
	    CRGB pixel = _fb[y*matrixWidth + x];
            _virtdisp->drawPixelRGB888( x, y, pixel.r, pixel.g, pixel.b);
	}
    }
}

// vim:sts=4:sw=4
