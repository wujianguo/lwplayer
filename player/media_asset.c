//
//  media_asset.c
//  player
//
//  Created by Jianguo Wu on 2018/7/23.
//  Copyright © 2018年 wujianguo. All rights reserved.
//

#include "media_asset.h"
#include "video_decoder.h"
#include "audio_decoder.h"

int open_asset(const char *url, MediaAsset **asset) {
    av_register_all();
    int ret = 0;
    AVFormatContext *ifmt_ctx = NULL;
    ret = avformat_open_input(&ifmt_ctx, url, 0, 0);
    if (ret < 0) {
        return ret;
    }
    
    ret = avformat_find_stream_info(ifmt_ctx, 0);
    if (ret < 0) {
        avformat_close_input(&ifmt_ctx);
        return ret;
    }
    
    MediaAsset *asset_ptr = av_malloc(sizeof(MediaAsset));
    asset_ptr->ifmt_ctx = ifmt_ctx;
    asset_ptr->read_cond = SDL_CreateCond();
    
    av_dump_format(ifmt_ctx, 0, url, 0);
    
    unsigned int stream_index = 0;
    for (; stream_index < ifmt_ctx->nb_streams; ++stream_index) {
        if (ifmt_ctx->streams[stream_index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            open_video_track(ifmt_ctx, stream_index, &asset_ptr->video);
        } else if (ifmt_ctx->streams[stream_index]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {

        }
    }

    *asset = asset_ptr;
    return 0;

}

int close_asset(MediaAsset **asset) {
    MediaAsset *asset_ptr = *asset;
    close_video_track(&asset_ptr->video);
    avformat_close_input(&asset_ptr->ifmt_ctx);
    SDL_DestroyCond(asset_ptr->read_cond);
    av_free(asset_ptr);
    *asset = NULL;
    return 0;
}
