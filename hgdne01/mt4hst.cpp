#include "pch.h"
#include "mt4hst.h"

#include <inttypes.h>

void print_header(FILE* f, const HST_HEADER& h)
{
    fprintf(
        f,
        "{ version=%d, copyright='%.*s', symbol='%.*s', period=%d, digits=%d, timesign=%d, last_sync=%d' }\n",
        h.version,
        sizeof(h.copyright), h.copyright,
        sizeof(h.symbol), h.symbol,
        h.period,
        h.digits,
        h.timesign,
        h.last_sync
    );
}

void print_record400(FILE* f, const HST_RECORD_400& r, int32_t digits)
{
    fprintf(
        f,
        "{ ctm=%" PRId32 ", open=%.*f, high=%.*f, low=%.*f, close=%.*f, volume=%f }\n",
        r.ctm,
        digits, r.open,
        digits, r.high,
        digits, r.low,
        digits, r.close,
        r.volume
    );
}

void print_record401(FILE* f, const HST_RECORD_401& r, int32_t digits)
{
    fprintf(
        f,
        "{ ctm=%" PRId64 ", open=%.*f, high=%.*f, low=%.*f, close=%.*f, volume=%" PRId64 ", spread=%d, real_volume=%" PRId64 " }\n",
        r.ctm,
        digits, r.open,
        digits, r.high,
        digits, r.low,
        digits, r.close,
        r.volume,
        r.spread,
        r.real_volume
    );
}
