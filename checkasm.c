/*
 * Assembly testing and benchmarking tool
 * Copyright (c) 2015 Henrik Gramner
 * Copyright (c) 2008 Loren Merritt
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with FFmpeg; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "config.h"
#include "config_components.h"

#if CONFIG_LINUX_PERF
# ifndef _GNU_SOURCE
#  define _GNU_SOURCE // for syscall (performance monitoring API)
# endif
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "checkasm.h"
#include "libavutil/common.h"
#include "libavutil/cpu.h"
#include "libavutil/intfloat.h"
#include "libavutil/random_seed.h"

#if HAVE_IO_H
#include <io.h>
#endif

#if HAVE_SETCONSOLETEXTATTRIBUTE && HAVE_GETSTDHANDLE
#include <windows.h>
#define COLOR_RED    FOREGROUND_RED
#define COLOR_GREEN  FOREGROUND_GREEN
#define COLOR_YELLOW (FOREGROUND_RED|FOREGROUND_GREEN)
#else
#define COLOR_RED    1
#define COLOR_GREEN  2
#define COLOR_YELLOW 3
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if !HAVE_ISATTY
#define isatty(fd) 1
#endif

#if ARCH_ARM && HAVE_ARMV5TE_EXTERNAL
#include "libavutil/arm/cpu.h"

void (*checkasm_checked_call)(void *func, int dummy, ...) = checkasm_checked_call_novfp;
#endif


#if HAVE_SETCONSOLETEXTATTRIBUTE && HAVE_GETSTDHANDLE
/* float compare support code */
static int is_negative(union av_intfloat32 u)
{
    return u.i >> 31;
}
#else
/* float compare support code */
static int is_negative(union av_intfloat32 u)
{
    return u.i >> 32;
}
#endif


int float_near_ulp(float a, float b, unsigned max_ulp)
{
    union av_intfloat32 x, y;

    x.f = a;
    y.f = b;

    if (is_negative(x) != is_negative(y)) {
        // handle -0.0 == +0.0
        return a == b;
    }

    if (llabs((int64_t)x.i - y.i) <= max_ulp)
        return 1;

    return 0;
}
