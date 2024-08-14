#include "render/render.h"
#include "global.h"
#include "render_internal.h"
#include "util.h"

static Render_State_Internal state = {0};

void render_init(u32 width, u32 height, Render_Flags flags) {
    //Makes the interpolation the nicest it can be and some other hints.
    SDL_SetHint(SDL_HINT_AUTO_UPDATE_JOYSTICKS, "1");
    SDL_SetHint(SDL_HINT_JOYSTICK_THREAD, "1");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
        WARN("Could not initialize everything for SDL!\nError: %s", SDL_GetError())
    }

    global.render.width = width;
    global.render.height = height;
    initialize_SDL("Engine", global.render.width, global.render.height, flags);


    //Set flags
    global.render_flags = flags;
}

void render_begin(void){
    SDL_RenderClear(global.render.renderer);
}

void render_end(void){
    if(global.engine & FOCUSED || global.render_flags & IGNORE_FOCUS) {
        //Show frame and reset bitmap debug information for next frame.
        SDL_RenderPresent(global.render.renderer);
        global.bitmap.bitmap_updates = 0;
        global.bitmap.bitmap_calls = 0;
    }
}

void set_window_size(i32 width, i32 height){
    if(width <= 0 || height <= 0){
        WARN("Invalid height or width provided\n")
        return;
    }

    SDL_SetWindowSize(global.render.window, width, height);
    global.render.width = width;
    global.render.height = height;
}
