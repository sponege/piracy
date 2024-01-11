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
	// vramSetBankB(VRAM_B_MAIN_BG);
  	// vramSetBankC(VRAM_C_SUB_BG);
    // vramSetBankA(VRAM_A_MAIN_BG_0x06000000);

	// enable extended palettes
	bgExtPaletteEnable();
	bgExtPaletteEnableSub();

	int hudsonBg = bgInit(3, BgType_Bmp8, BgSize_B8_512x256, 0,0);
	bgSetControlBits(hudsonBg, BG_WRAP_ON); // scrolling background
	int piracyBg = bgInit(0, BgType_Text8bpp, BgSize_T_256x256, 1, 1);
	int jailBg = bgInitSub(2, BgType_Bmp8, BgSize_B8_256x256, 0, 0);

	dmaCopy(hudsonBitmap, bgGetGfxPtr(hudsonBg), 512*256);
	dmaCopy(hudsonPal, BG_PALETTE, 256*2);
	dmaCopy(nopartyTiles, bgGetGfxPtr(piracyBg), nopartyTilesLen);
	dmaCopy(nopartyMap, bgGetMapPtr(piracyBg), nopartyMapLen);
	// you can only access extended palettes in LCD mode
  	vramSetBankE(VRAM_E_LCD); // for main engine
	dmaCopy(nopartyPal,  &VRAM_E_EXT_PALETTE[0][0], nopartyPalLen);  // bg 0, slot 0
	// map vram banks to extended palettes
	// http://mtheall.com/banks.html#A=MBG0&C=MBG2&E=BGEPAL&H=SBGEPAL
	vramSetBankE(VRAM_E_BG_EXT_PALETTE);     // for main engine
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
