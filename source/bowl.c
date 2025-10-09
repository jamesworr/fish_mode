//
// Will 8/5
//

#include <string.h>
#include <tonc.h>
#include "bowl_bg.h"

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;

#define CBB_0  0
#define SBB_0 28

#define FPID 0
#define WPID 1

// [7:4] is Palette ID
// [3:0] is Tile ID
#define GRID_TL_W  0x30
#define GRID_TR_W  0x31
#define GRID_BL_W  0x33
#define GRID_BR_W  0x32
#define GRID_TL_G  0x50
#define GRID_TR_G  0x51
#define GRID_BL_G  0x53
#define GRID_BR_G  0x52
#define BLANK_BG   0x04

#define BLOCK_WIDTH  16
#define BLOCK_HEIGHT 16

#define NUM_COLS 9
#define NUM_ROWS 9
#define MAX_BLOCKS 81 // FIXME
#define BLOCK_GAP_WIDTH 0

#define BLOCK_TILE_OFFSET 0

//BG_POINT bg0_pt= { 0, 0 };
//SCR_ENTRY *bg0_map= se_mem[SBB_0];


void wait_any_key(void) {
    while(1) {
        vid_vsync();
        key_poll();
        if(key_hit(KEY_ANY)) {
            break;
        }
    }
}


void sprite_loop() {
    //OBJ_ATTR *block_obj;
    //for (int i = 0; i < MAX_BLOCKS; i++) {
    //    block_obj = &obj_buffer[i];
    //    obj_set_attr(block_obj,
    //        ATTR0_SQUARE | ATTR0_HIDE,
    //        ATTR1_SIZE_16x16,
    //        ATTR2_PALBANK(0) | BLOCK_TILE_OFFSET);
    //    obj_set_pos(block_obj, (i/NUM_COLS)*BLOCK_HEIGHT, (i%NUM_ROWS)*BLOCK_WIDTH);
    //}


    while(1) {
        vid_vsync();
        key_poll();
        if (key_hit(KEY_SELECT)) {
            break; // Break out of waiting loop and restart
        }
        
        // FIXME redo the bounds checking to account for different piece shapes
        //if (key_hit(KEY_UP) && (live_piece_y > 0)) {
        //    live_piece_y--;
        //}
        //if (key_hit(KEY_DOWN) && (live_piece_y < (NUM_ROWS - piece_library[live_piece_idx].y_len))) {
        //    live_piece_y++;
        //}
        //if (key_hit(KEY_LEFT) && (live_piece_x > 0)) {
        //    live_piece_x--;
        //}
        //if (key_hit(KEY_RIGHT) && (live_piece_x < (NUM_ROWS - piece_library[live_piece_idx].x_len))) {
        //    live_piece_x++;
        //}

        //oam_copy(oam_mem, obj_buffer,  MAX_BLOCKS /* + other things??? TODO */);
    }
}

int main() {
    
    // TODO sprite init
    // Setup for tiled mode
    //   into LOW obj memory (cbb == 4)
    //  copy block
    //memcpy32(&tile_mem[4][0], block_spriteTiles, block_spriteTilesLen / sizeof(u32));
    //memcpy16(pal_obj_mem, block_spritePal, block_spritePalLen / sizeof(u16));
    //oam_init(obj_buffer, 128);
    //REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;


	// Load palette
	memcpy16(pal_bg_mem, bowl_bgPal, bowl_bgPalLen / sizeof(u16));
	// Load tiles into CBB 0
	memcpy32(&tile_mem[0][0], bowl_bgTiles, bowl_bgTilesLen / sizeof(u32));
	// Load map into SBB 30
	memcpy32(&se_mem[30][0], bowl_bgMap, bowl_bgMapLen / sizeof(u32));

	// set up BG0 for a 4bpp 64x32t map, using
	//   using charblock 0 and screenblock 31
	REG_BG0CNT= BG_CBB(0) | BG_SBB(30) | BG_8BPP | BG_REG_64x32;
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0;




    //sprite_loop();

    // Waiting for new commands
    while(1) {
        vid_vsync();
        key_poll();
        if (key_hit(KEY_SELECT)) {
            break; // Break out of waiting loop and restart
        }
    }
    return 0;
}
