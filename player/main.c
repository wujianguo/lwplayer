//
//  main.c
//  player
//
//  Created by Jianguo Wu on 2018/7/20.
//  Copyright © 2018年 wujianguo. All rights reserved.
//

#include <stdio.h>
#include "event_loop.h"

int main(int argc, const char * argv[]) {
    if(argc < 2) {
        printf("Please provide a movie file\n");
        return -1;
    }
    
    const char* in_filename = argv[1];
//    const char* in_filename = "http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/bipbop_4x3_variant.m3u8";
    printf("%s\n", in_filename);
    
    int ret = 0;
    MediaAsset *asset = NULL;
    ret = open_asset(in_filename, &asset);
    if (ret != 0) {
        return ret;
    }
    event_loop_run(asset);
    return 0;
}






