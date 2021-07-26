#pragma once

#include <stdio.h>
#include <stdint.h>

#pragma pack(push, 1)

    struct HST_HEADER
    {
        int32_t version;
        char copyright[64];
        char symbol[12];
        int32_t period;
        int32_t digits;
        int32_t timesign;
        int32_t last_sync;
        int32_t unused[13];
    };

    struct HST_RECORD_400
    {
        int32_t ctm;
        double open;
        double low;
        double high;
        double close;
        double volume;
    };

    struct HST_RECORD_401
    {
        int64_t ctm;
        double open;
        double high;
        double low;
        double close;
        int64_t volume;
        int32_t spread;
        int64_t real_volume;
    };

void print_header(FILE* f, const HST_HEADER& h);
void print_record400(FILE* f, const HST_RECORD_400& r, int32_t digits = 6);
void print_record401(FILE* f, const HST_RECORD_401& r, int32_t digits = 6);

#pragma pack(pop)
