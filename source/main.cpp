#include <nds.h>
#include <stdio.h>
#include <gl2d.h>
#include <maxmod9.h>

#include "soundbank.h"  // Soundbank definitions
#include "soundbank_bin.h"  // Soundbank binary file

// grit adds a nice header we can include to access the data
// this has the same name as the image
#include "hudson.h"
#include "jail.h"
#include "noparty.h"
#include "poweroff.h"

int main(void)
{
	videoSetMode(MODE_5_2D);
	videoSetModeSub(MODE_5_2D);
	vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
	vramSetBankB(VRAM_B_MAIN_BG_0x06020000);

	// enable extended palettes
	bgExtPaletteEnable();
	bgExtPaletteEnableSub();

	int hudsonBg = bgInit(3, BgType_Bmp8, BgSize_B8_512x256, 8,0);
	bgSetControlBits(hudsonBg, BG_WRAP_ON); // scrolling background
	int nopartyBg = bgInit(0, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
	int jailBg = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
	int poweroffBg = bgInitSub(0, BgType_Text8bpp, BgSize_T_256x256, 30, 4);

	dmaCopy(hudsonBitmap, bgGetGfxPtr(hudsonBg), 512*256);
	dmaCopy(hudsonPal, BG_PALETTE, 256*2);
	dmaCopy(jailBitmap, bgGetGfxPtr(jailBg), 256*256);
	dmaCopy(jailPal, BG_PALETTE_SUB, 256*2);

	dmaCopy(nopartyTiles, bgGetGfxPtr(nopartyBg), nopartyTilesLen);
	dmaCopy(nopartyMap, bgGetMapPtr(nopartyBg), nopartyMapLen);
	dmaCopy(poweroffTiles, bgGetGfxPtr(poweroffBg), poweroffTilesLen);
	dmaCopy(poweroffMap, bgGetMapPtr(poweroffBg), poweroffMapLen);
	// you can only access extended palettes in LCD mode
  	vramSetBankE(VRAM_E_LCD); // for main engine
	vramSetBankH(VRAM_H_LCD); // for sub engine
	dmaCopy(nopartyPal,  &VRAM_E_EXT_PALETTE[0][0], nopartyPalLen);  // bg 0, slot 0
	dmaCopy(poweroffPal,  &VRAM_H_EXT_PALETTE[0][0], poweroffPalLen);  // bg 0, slot 0
	// map vram banks to extended palettes
	// http://mtheall.com/banks.html#A=MBG0&C=MBG2&E=BGEPAL&H=SBGEPAL
	vramSetBankE(VRAM_E_BG_EXT_PALETTE);     // for main engine
  	vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE); // for sub engine

	// Use this if you have the soundbank loaded into memory
    mmInitDefaultMem( (mm_addr)soundbank_bin );
	// load sound effects
	mmLoadEffect( SFX_NOPARTY );
	// play sound effect
	mmEffect(SFX_NOPARTY);

	int x = 0;
	int flipflop = 0; // flopflop slows scrolling by 2x
	int frames = 0;
	#define period 39
	while(1) {
		bgSetScroll(hudsonBg, x, x);
		bgUpdate();
		if(flipflop)x++;
		x%=512; // lcm of 256 and 512 is 512, repeating pattern after scrolling 512 pixels
		flipflop ^= 1;
		frames++;
		frames %= period * 2;
		if (frames > period) bgHide(poweroffBg);
		else bgShow(poweroffBg);
		swiWaitForVBlank();
	}

	return 0;
}
