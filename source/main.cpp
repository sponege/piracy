#include <nds.h>
#include <stdio.h>
#include <gl2d.h>
#include <maxmod9.h>
// #include <filesystem.h>
// #include <dirent.h>

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
	// nitroFSInit(NULL);
	// mmInitDefault( "soundbank.bin" );
	// load sound effects
	mmLoadEffect( SFX_NOPARTY );
	mmLoadEffect( SFX_5BEAT );
	// play sound effect
	mmEffect(SFX_NOPARTY);

	#define noparty_len 68197 * 60 / 1000 - 10
	#define partyend_len 39 * 4 * 2

	int x = 0;
	int scroll_counter = 0; // scroll counter slows scrolling by 43
	int frames = 0;
	#define period 39
	int brightness = 0;
	int songtime = 0;
	int songrepeats = 3;
	int partyend = 0;
	while(1) {
		songtime++;
		if (songrepeats > 0) {
			if (songtime == noparty_len) {
				songrepeats--;
				if (songrepeats > 0) mmEffect(SFX_NOPARTY);
				else mmEffect(SFX_5BEAT);
				songtime = 0;
			}
		} else {
			if (songtime == partyend_len) {
				partyend = 1;
			}
		}
		bgSetScroll(hudsonBg, x, x);
		bgUpdate();
		if (!scroll_counter) x++;
		if (partyend && brightness < 16) brightness++; // fade out
		x%=512; // lcm of 256 and 512 is 512, repeating pattern after scrolling 512 pixels
		scroll_counter += 1;
		if (scroll_counter > 2) scroll_counter = 0;
		// if (frames == 0) kicks--;
		frames++;
		frames %= period * 2;
		if (frames > period) bgHide(poweroffBg);
		else bgShow(poweroffBg);
		setBrightness(3, brightness);
		swiWaitForVBlank();
	}

	return 0;
}
