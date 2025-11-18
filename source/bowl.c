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

// Fish object
typedef struct {
    u8 x; // init to 120
    u8 y;

    // 0: stationary (always init)
    // 1: moving steady state
    // 2: accel positive
    // 3: accel negative
    u8 state;

    // index for v(t) function
    // always init to 0
    unsigned int count;

    // v(t) function
    // init to {0, 1, 2, 3, 4}
    u8 velocity[5];
} fish_t;

void wait_any_key(void) {
    while(1) {
        vid_vsync();
        key_poll();
        if(key_hit(KEY_ANY)) {
            break;
        }
    }
}

void update_fish_position(fish_t* ptr) {
    // afadfdsf
}


void sprite_loop(volatile fish_t* fish_ptr) {
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

        // TODO add acceleration and speed
        if (key_is_down(KEY_UP)) {
            fish_ptr->y--;
        }
        if (key_is_down(KEY_DOWN)) {
            fish_ptr->y++;
        }
        if (key_is_down(KEY_LEFT)) {
            fish_ptr->x--;
        }
        if (key_is_down(KEY_RIGHT)) {
            fish_ptr->x++;
        }

        obj_set_pos(&obj_buffer[0], fish_ptr->x, fish_ptr->y);
        oam_copy(oam_mem, obj_buffer, 1);
    }
}

int main() {
    volatile fish_t fish = {
        // init fish values
        .x = 120,
        .y =  80,
        .state = 0,
        .count = 0,
        .velocity = {0, 1, 2, 3, 4}
    };

    // Copy Sprite Tiles
    memcpy32(&tile_mem[4][0], fish_tiles, fish_tiles_len / sizeof(u32));
    memcpy16(pal_obj_mem, fish_pal, fish_pal_len / sizeof(u16));
    oam_init(obj_buffer, 128);

    // Set fish sprite attributes
    obj_set_attr(&obj_buffer[0],
        ATTR0_WIDE | ATTR0_8BPP,
        ATTR1_SIZE_16x8,
        ATTR2_PALBANK(0)); // | BLOCK_TILE_OFFSET);
    obj_set_pos(&obj_buffer[0], fish.x, fish.y);
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


    sprite_loop(&fish);

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
