//
//  media_asset.h
//  player
//
//  Created by Jianguo Wu on 2018/7/23.
//  Copyright © 2018年 wujianguo. All rights reserved.
//

#ifndef media_asset_h
#define media_asset_h

#include "media_asset_track.h"
#include <libavformat/avformat.h>
#include <SDL2/SDL.h>

typedef struct MediaAsset {
    AVFormatContext     *ifmt_ctx;
    
    VideoAssetTrack     *video;
    AudioAssetTrack     *audio;
    
    SDL_cond            *read_cond;
} MediaAsset;

int open_asset(const char *url, MediaAsset **asset);

int close_asset(MediaAsset **asset);


#endif /* media_asset_h */
