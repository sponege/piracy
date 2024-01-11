#include <nds.h>
#include <stdio.h>
#include <gl2d.h>

// grit adds a nice header we can include to access the data
// this has the same name as the image
#include "hudson.h"
#include "jail.h"
#include "noparty.h"

int main(void)
{
	videoSetMode(MODE_5_2D);
	videoSetModeSub(MODE_5_2D);
    // vramSetBankA(VRAM_A_MAIN_BG_0x06000000);

	int hudsonBg = bgInit(2, BgType_Bmp8, BgSize_B8_512x256, 0,0);
	bgSetControlBits(hudsonBg, BG_WRAP_ON); // scrolling background
	// int piracyBg = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
	int jailBg = bgInitSub(2, BgType_Bmp8, BgSize_B8_256x256, 0, 0);

	dmaCopy(hudsonBitmap, bgGetGfxPtr(hudsonBg), 512*256);
	dmaCopy(hudsonPal, BG_PALETTE, 256*2);
	// dmaCopy(nopartyBitmap, bgGetGfxPtr(piracyBg), 256*256);
	// dmaCopy(nopartyPal, bgGetPalPtr(piracyBg), 256*2);
	dmaCopy(jailBitmap, bgGetGfxPtr(jailBg), 256*256);
	dmaCopy(jailPal, BG_PALETTE_SUB, 256*2);

	int x = 0;
	int flipflop = 0; // flopflop slows scrolling by 2x
	while(1) {
		bgSetScroll(hudsonBg, x, x);
		bgUpdate();
		if(flipflop)x++;
		x%=512; // lcm of 256 and 512 is 512, repeating pattern after scrolling 512 pixels
		flipflop ^= 1;
		swiWaitForVBlank();
	}

	return 0;
}
