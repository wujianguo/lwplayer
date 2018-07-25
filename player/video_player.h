//
//  video_player.h
//  player
//
//  Created by Jianguo Wu on 2018/7/20.
//  Copyright © 2018年 wujianguo. All rights reserved.
//

#ifndef video_player_h
#define video_player_h

#include <SDL2/SDL.h>
#include <libavcodec/avcodec.h>

typedef struct VideoDisplayContext {
    SDL_Renderer    *renderer;
    SDL_Texture     *bmp;
    SDL_Rect        rect;
    
    AVCodecContext *codec_ctx;
} VideoDisplayContext;


VideoDisplayContext *video_display_alloc_context(SDL_Window *screen, AVCodecContext *codec_ctx);

int display_video_image(VideoDisplayContext *context, AVFrame *frame);

#endif /* video_player_h */
