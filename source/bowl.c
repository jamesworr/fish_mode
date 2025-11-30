//
// Will 8/5
//

#include <string.h>
#include <tonc.h>
#include "bowl_bg.h"
#include "fish.h"

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;

#define CBB_0  0
#define SBB_0 28

#define ACCEL_TIMER_LIMIT 30
#define COUNT_SHIFT_AMMOUNT 3

#define FISH_ACCELERATION_POS  4
#define FISH_ACCELERATION_NEG -2
#define FISH_VELOCITY_LIMIT 180

// Fish object
typedef struct {
    u8 x; // init to 120
    u8 y;

    // 0: stationary (always init)
    // 1: moving steady state
    // 2: accel positive
    // 3: accel negative
    u8 state;

    // 0: left (init)
    // 1: right
    u8 x_direction;

    // always init to 0
    unsigned int frame_counter; // free-running live counter
    unsigned int frame_0;       // counter on last update

    int vel_x;
    int vel_y;

    int x_accel;

} fish_t;

void update_fish_position(volatile fish_t* fish_ptr) {
    // TODO derive net acceleration from sum of forces
    //unsigned int delta_t = fish_ptr->frame_counter - fish_ptr->frame_0;
    //fish_ptr->frame_0 = fish_ptr->frame_counter; // update frame_0 for the next iteration

    // dx = 0.5*a*(dt)^2 + v*dt
    // dv = a*dt
    unsigned int delta_x = ( ((fish_ptr->x_direction)?-1:1) * fish_ptr->vel_x  ); // velocity term, delta_t always assumed 1
    if (fish_ptr->x_accel != 0) { // skip multiply by 0
        delta_x += (( ((fish_ptr->x_direction)?-1:1) * fish_ptr->x_accel ) >> 1); // acceleration term, delta_t always assumed 1
        fish_ptr->vel_x += fish_ptr->x_accel; // delta_t always assumed 1
    }
    fish_ptr->x += (delta_x >> 5);
    // TODO Y axis
}

// TODO merge states 0 and 1???
// TODO get rid of states altogether??

// 0: stationary
void fish_fsm_0(volatile fish_t* fish_ptr) {

    // TODO decide L/R priority
    if (key_hit(KEY_LEFT)) {
        fish_ptr->x_direction = 1;
        fish_ptr->state = 2;
        fish_ptr->x_accel = FISH_ACCELERATION_POS;
    }
    else if (key_hit(KEY_RIGHT)) {
        fish_ptr->x_direction = 0;
        fish_ptr->state = 2;
        fish_ptr->x_accel = FISH_ACCELERATION_POS;
    }
    else {
        fish_ptr->state = 0;
        fish_ptr->x_accel = 0;
    }
}

// 1: moving steady state
void fish_fsm_1(volatile fish_t* fish_ptr) {
    // skip to slow down when button released
    if ( (fish_ptr->x_direction == 0 && key_is_up(KEY_RIGHT)) || ((fish_ptr->x_direction == 1 && key_is_up(KEY_LEFT)))    ) {
        fish_ptr->state = 3;
        fish_ptr->x_accel = FISH_ACCELERATION_NEG;
    }
    else {
        fish_ptr->state = 1;
        fish_ptr->x_accel = 0;
    }
}

// 2: accel positive
void fish_fsm_2(volatile fish_t* fish_ptr) {
    // skip to slow down when button released
    if ( (fish_ptr->x_direction == 0 && key_is_up(KEY_RIGHT)) || ((fish_ptr->x_direction == 1 && key_is_up(KEY_LEFT)))    ) {
        fish_ptr->state = 3;
        fish_ptr->x_accel = FISH_ACCELERATION_NEG;
        return;
    }

    if (fish_ptr->vel_x > FISH_VELOCITY_LIMIT) {
        // move to steady state after timer hits limit
        fish_ptr->state = 1;
        fish_ptr->x_accel = 0;
    }
    else {
        fish_ptr->state = 2;
        fish_ptr->x_accel = FISH_ACCELERATION_POS;
    }
}

// 3: accel negative
void fish_fsm_3(volatile fish_t* fish_ptr) {
    // TODO button re-pressed
    //if (key_pressed(KEY_LEFT | KEY_RIGHT)) {
    //    fish_ptr->state = 2;
    //}

    if (fish_ptr->vel_x != 0) {
        fish_ptr->state =  3;
        fish_ptr->x_accel = FISH_ACCELERATION_NEG;
    }
    else {
        // move to stationary after timer expire
        fish_ptr->state = 0;
        fish_ptr->x_accel = 0;
    }
}

void update_fish_fsm(volatile fish_t* fish_ptr) {
    // TODO handle Y dimension
    switch (fish_ptr->state) {
        case 0:
            fish_fsm_0(fish_ptr);
            break;
        case 1:
            fish_fsm_1(fish_ptr);
            break;
        case 2:
            fish_fsm_2(fish_ptr);
            break;
        case 3:
            fish_fsm_3(fish_ptr);
            break;
        default:
            fish_fsm_0(fish_ptr);
            break;
    }
}

void wait_any_key(void) {
    while(1) {
        vid_vsync();
        key_poll();
        if(key_hit(KEY_ANY)) {
            break;
        }
    }
}

void sprite_loop(volatile fish_t* fish_ptr) {
    while(1) {
        vid_vsync();
        key_poll();
        if (key_hit(KEY_SELECT)) {
            break; // Break out of waiting loop and restart
        }

        // TODO get rid of me
        if (key_is_down(KEY_UP)) {
            fish_ptr->y--;
        }
        if (key_is_down(KEY_DOWN)) {
            fish_ptr->y++;
        }

        // Fish horizontal motion
        // TODO try every x frames
        update_fish_fsm(fish_ptr);
        //if (fish_ptr->frame_counter % 2 == 0) {
        //    update_fish_position(fish_ptr);
        //}
        update_fish_position(fish_ptr);

        // copy to buffers
        obj_set_pos(&obj_buffer[0], fish_ptr->x, fish_ptr->y);
        oam_copy(oam_mem, obj_buffer, 1);
        fish_ptr->frame_counter++;
    }
}

int main() {
    volatile fish_t fish = {
        // init fish values
        .x = 120,
        .y =  80,
        .state = 0,
        .x_direction = 0,
        .frame_counter = 0,
        .frame_0 = 0,
        .vel_x = 0,
        .vel_y = 0,
        .x_accel = 0
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
