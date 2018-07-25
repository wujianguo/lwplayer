//
//  event_loop.c
//  player
//
//  Created by Jianguo Wu on 2018/7/20.
//  Copyright © 2018年 wujianguo. All rights reserved.
//

#include "event_loop.h"
#include <SDL2/SDL.h>
#include "video_decoder.h"
#include "reader_thread.h"
#include "video_player.h"

#define PLAYER_REFRESH_EVENT  (SDL_USEREVENT + 1)

static Uint32 sdl_refresh_timer_cb(Uint32 interval, void *param) {
    SDL_Event event;
    event.type = PLAYER_REFRESH_EVENT;
    event.user.data1 = param;
    SDL_PushEvent(&event);
    return 0;
}

int event_loop_run(MediaAsset *asset) {
    
    SDL_Window *screen = NULL;
    screen = SDL_CreateWindow(asset->ifmt_ctx->filename,
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              asset->video->track.codec_context->width,
                              asset->video->track.codec_context->height,
                              0);
    
    VideoDisplayContext *display_context = video_display_alloc_context(screen, asset->video->track.codec_context);
    
    SDL_CreateThread(reader_thread, "reader_thread", asset);
    SDL_CreateThread(video_decoder_thread, "video_decoder", asset);

    SDL_Event event = {0};
    AVFrame *frame = NULL;
    int quit = 0;
    int ret = 0;
    
    SDL_AddTimer(50, sdl_refresh_timer_cb, display_context);

    while (!quit) {
        SDL_PollEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                quit = 1;
                break;
            case PLAYER_REFRESH_EVENT:
                ret = frame_queue_get(&asset->video->track, &frame);
                if (ret == AVERROR(EAGAIN)) {
                    SDL_AddTimer(10, sdl_refresh_timer_cb, display_context);
                    break;
                }
                display_video_image(display_context, frame);
                SDL_Delay(50);
                frame_queue_release(&asset->video->track);
                SDL_AddTimer(50, sdl_refresh_timer_cb, display_context);
                break;
                
            default:
                break;
        }
    }
    return ret;
}
