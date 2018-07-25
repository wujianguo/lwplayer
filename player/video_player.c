//
//  video_player.c
//  player
//
//  Created by Jianguo Wu on 2018/7/20.
//  Copyright © 2018年 wujianguo. All rights reserved.
//

#include "video_player.h"
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>

VideoDisplayContext *video_display_alloc_context(SDL_Window *screen, AVCodecContext *codec_ctx) {
    VideoDisplayContext *context = av_malloc(sizeof(VideoDisplayContext));
    context->codec_ctx = codec_ctx;
    context->renderer = SDL_CreateRenderer(screen, -1, 0);
    context->bmp = SDL_CreateTexture(context->renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, codec_ctx->width, codec_ctx->height);

    context->rect.x = 0;
    context->rect.y = 0;
    context->rect.w = codec_ctx->width;
    context->rect.h = codec_ctx->height;

    return context;
}


int display_video_image(VideoDisplayContext *context, AVFrame *frame) {
//    av_log(NULL, AV_LOG_ERROR, "%lld, display\n", av_gettime()/1000/1000);
    SDL_UpdateYUVTexture(context->bmp, &context->rect,
                         frame->data[0], frame->linesize[0],
                         frame->data[1], frame->linesize[1],
                         frame->data[2], frame->linesize[2]);
    SDL_RenderClear(context->renderer);
    SDL_RenderCopy(context->renderer, context->bmp, &context->rect, &context->rect);
    SDL_RenderPresent(context->renderer);
    return 0;
}
