#ifndef BUILD_INFO_H
#define BUILD_INFO_H

typedef struct
{
    const char *date, *time, *src_version, *env_version;
} _tBuildInfo;
extern _tBuildInfo _BuildInfo;

#endif
