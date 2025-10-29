//
// Will 8/5
//

#include <string.h>
#include <tonc.h>
#include "bowl_bg.h"
#include "fish.h"

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;

#define CBB_0  0
#define SBB_0 28

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

    volatile u8 fish_x = 120;
    volatile u8 fish_y =  80;

    while(1) {
        vid_vsync();
        key_poll();
        if (key_hit(KEY_SELECT)) {
            break; // Break out of waiting loop and restart
        }

        // TODO add acceleration and speed
        if (key_is_down(KEY_UP)) {
            fish_y--;
        }
        if (key_is_down(KEY_DOWN)) {
            fish_y++;
        }
        if (key_is_down(KEY_LEFT)) {
            fish_x--;
        }
        if (key_is_down(KEY_RIGHT)) {
            fish_x++;
        }

        obj_set_pos(&obj_buffer[0], fish_x, fish_y);
        oam_copy(oam_mem, obj_buffer, 1);
    }
}

int main() {

    // Copy Sprite Tiles
    memcpy32(&tile_mem[4][0], fish_tiles, fish_tiles_len / sizeof(u32));
    memcpy16(pal_obj_mem, fish_pal, fish_pal_len / sizeof(u16));
    oam_init(obj_buffer, 128);

    // Set fish sprite attributes
    obj_set_attr(&obj_buffer[0],
        ATTR0_WIDE | ATTR0_8BPP,
        ATTR1_SIZE_16x8,
        ATTR2_PALBANK(0)); // | BLOCK_TILE_OFFSET);
    obj_set_pos(&obj_buffer[0], 120, 80);
    oam_copy(oam_mem, obj_buffer, 1);

    // Copy background tiles
	// Load palette
	memcpy16(pal_bg_mem, bowl_bg_pal, bowl_bg_pal_len / sizeof(u16));
	// Load tiles into CBB 0
	memcpy32(&tile_mem[0][0], bowl_bg_tiles, bowl_bg_tiles_len / sizeof(u32));
	// Load map into SBB 30
	memcpy32(&se_mem[30][0], bowl_bg_map, bowl_bg_map_len / sizeof(u32));

    // Setup background and display registers
    REG_BG0CNT  = BG_CBB(0) | BG_SBB(30) | BG_8BPP | BG_REG_32x32;
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;


    sprite_loop();

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
