
//{{BLOCK(bowl_bg)

//======================================================================
//
//	bowl_bg, 256x256@8, 
//	+ palette 256 entries, not compressed
//	+ 185 tiles (t|f reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 11840 + 2048 = 14400
//
//	Time-stamp: 2025-10-08, 21:37:06
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BOWL_BG_H
#define GRIT_BOWL_BG_H

#define bowl_bgTilesLen 11840
extern const unsigned int bowl_bgTiles[2960];

#define bowl_bgMapLen 2048
extern const unsigned short bowl_bgMap[1024];

#define bowl_bgPalLen 512
extern const unsigned short bowl_bgPal[256];

#endif // GRIT_BOWL_BG_H

//}}BLOCK(bowl_bg)
