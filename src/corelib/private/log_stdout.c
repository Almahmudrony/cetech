/***********************************************************************
**** Includes
***********************************************************************/

#include <stdio.h>

#include <corelib/platform.h>

#if CT_PLATFORM_LINUX

#include <sys/file.h>

#endif

#include "corelib/log.h"
#include <memory.h>
#include "corelib/macros.h"

/***********************************************************************
**** Internals
***********************************************************************/

#define FBLACK      "\033["

#define BRED        "31m"
#define BGREEN      "32m"
#define BYELLOW     "33m"
#define BBLUE       "34m"

#define NONE        "\033[0m"

#define LOG_FORMAT   \
    "---\n"          \
    "level: %s\n"    \
    "where: %s\n"    \
    "time: %s\n"     \
    "worker: %d\n"   \
    "msg: |\n  %s\n"

#if CT_PLATFORM_LINUX || CT_PLATFORM_OSX
#define CETECH_COLORED_LOG
#endif

#ifdef CETECH_COLORED_LOG
#define COLORED_TEXT(color, text) FBLACK color text NONE
#else
#define COLORED_TEXT(color, text) text
#endif


static char *_time_to_str(struct tm *gmtm) {
    char *time_str = asctime(gmtm);
    time_str[strlen(time_str) - 1] = '\0';
    return time_str;
}


/***********************************************************************
**** Interface implementation
***********************************************************************/

void ct_log_stdout_handler(enum ct_log_level level,
                           time_t time,
                           char worker_id,
                           const char *where,
                           const char *msg,
                           void *data) {
    CT_UNUSED(data);

    static const char *_level_to_str[4] = {
            [LOG_INFO]    = "info",
            [LOG_WARNING] = "warning",
            [LOG_ERROR]   = "error",
            [LOG_DBG]     = "debug"
    };

    static const char *_level_format[4] = {
            [LOG_INFO]    = LOG_FORMAT,
            [LOG_WARNING] = COLORED_TEXT(BYELLOW, LOG_FORMAT),
            [LOG_ERROR]   = COLORED_TEXT(BRED, LOG_FORMAT),
            [LOG_DBG]     = COLORED_TEXT(BGREEN, LOG_FORMAT)
    };

    FILE *out = level == LOG_ERROR ? stderr : stdout;

    struct tm *gmtm = gmtime(&time);
    const char *time_str = _time_to_str(gmtm);

#if CT_PLATFORM_LINUX
    flock(out->_fileno, LOCK_EX);
#endif

    fprintf(out, _level_format[level], _level_to_str[level],
            where, time_str, worker_id, msg);

#if CT_PLATFORM_LINUX
    fflush_unlocked(out);
    flock(out->_fileno, LOCK_UN);
#endif
}
