/*
 * motion_comp_arm.c
 * Copyright (C) 2004 AGAWA Koji <i (AT) atty (DOT) jp>
 *
 * This file is part of mpeg2dec, a free MPEG-2 video stream decoder.
 * See http://libmpeg2.sourceforge.net/ for updates.
 *
 * mpeg2dec is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * mpeg2dec is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"

#define ARCH_ARM

#ifdef ARCH_ARM

#include "inttypes.h"

#include "mpeg2.h"
#include "attributes.h"
#include "mpeg2_internal.h"

#define inline __forceinline

#define avg2(a,b) ((a+b+1)>>1)
#define avg4(a,b,c,d) ((a+b+c+d+2)>>2)

#define predict_o(i) (ref[i])
#define predict_x(i) (avg2 (ref[i], ref[i+1]))
#define predict_y(i) (avg2 (ref[i], (ref+stride)[i]))
#define predict_xy(i) (avg4 (ref[i], ref[i+1], \
			     (ref+stride)[i], (ref+stride)[i+1]))

#define put(predictor,i) dest[i] = predictor (i)
#define avg(predictor,i) dest[i] = avg2 (predictor (i), dest[i])

/* mc function template */

#define MC_FUNC(op,xy)							\
static CODE_IN_ITCM void inline MC_##op##_##xy##_16_c (uint8_t * dest, const uint8_t * ref,	\
				   const int stride, int height)	\
{									\
    do {								\
	op (predict_##xy, 0);						\
	op (predict_##xy, 1);						\
	op (predict_##xy, 2);						\
	op (predict_##xy, 3);						\
	op (predict_##xy, 4);						\
	op (predict_##xy, 5);						\
	op (predict_##xy, 6);						\
	op (predict_##xy, 7);						\
	op (predict_##xy, 8);						\
	op (predict_##xy, 9);						\
	op (predict_##xy, 10);						\
	op (predict_##xy, 11);						\
	op (predict_##xy, 12);						\
	op (predict_##xy, 13);						\
	op (predict_##xy, 14);						\
	op (predict_##xy, 15);						\
	ref += stride;							\
	dest += stride;							\
    } while (--height);							\
}									\
static void MC_##op##_##xy##_8_c (uint8_t * dest, const uint8_t * ref,	\
				  const int stride, int height)		\
{									\
    do {								\
	op (predict_##xy, 0);						\
	op (predict_##xy, 1);						\
	op (predict_##xy, 2);						\
	op (predict_##xy, 3);						\
	op (predict_##xy, 4);						\
	op (predict_##xy, 5);						\
	op (predict_##xy, 6);						\
	op (predict_##xy, 7);						\
	ref += stride;							\
	dest += stride;							\
    } while (--height);							\
}									\
/* definitions of the actual mc functions */

#if 1
MC_FUNC (put,o)
MC_FUNC (avg,o)
MC_FUNC (put,x)
MC_FUNC (avg,x)
MC_FUNC (put,y)
MC_FUNC (avg,y)
MC_FUNC (put,xy)
MC_FUNC (avg,xy)
#endif

extern "C" {
void MC_put_o_16_arm (uint8_t * dest, const uint8_t * ref, int stride, int height);
void MC_put_x_16_arm (uint8_t * dest, const uint8_t * ref, int stride, int height);
void MC_put_o_8_arm (uint8_t * dest, const uint8_t * ref, int stride, int height);
void MC_put_x_8_arm (uint8_t * dest, const uint8_t * ref, int stride, int height);
}

static void MC_put_y_16_arm (uint8_t * dest, const uint8_t * ref, int stride, int height)
{
  MC_put_y_16_c(dest, ref, stride, height);
}

static void MC_put_xy_16_arm (uint8_t * dest, const uint8_t * ref, int stride, int height)
{
  MC_put_xy_16_c(dest, ref, stride, height);
}

static void MC_put_y_8_arm (uint8_t * dest, const uint8_t * ref, int stride, int height)
{
  MC_put_y_8_c(dest, ref, stride, height);
}

static void MC_put_xy_8_arm (uint8_t * dest, const uint8_t * ref, int stride, int height)
{
  MC_put_xy_8_c(dest, ref, stride, height);
}

static void MC_avg_o_16_arm (uint8_t * dest, const uint8_t * ref, int stride, int height)
{
//  MC_avg_o_16_c(dest, ref, stride, height);
}

static void MC_avg_x_16_arm (uint8_t * dest, const uint8_t * ref, int stride, int height)
{
//  MC_avg_x_16_c(dest, ref, stride, height);
}

static void MC_avg_y_16_arm (uint8_t * dest, const uint8_t * ref, int stride, int height)
{
//  MC_avg_y_16_c(dest, ref, stride, height);
}

static void MC_avg_xy_16_arm (uint8_t * dest, const uint8_t * ref, int stride, int height)
{
//  MC_avg_xy_16_c(dest, ref, stride, height);
}

static void MC_avg_o_8_arm (uint8_t * dest, const uint8_t * ref, int stride, int height)
{
//  MC_avg_o_8_c(dest, ref, stride, height);
}

static void MC_avg_x_8_arm (uint8_t * dest, const uint8_t * ref, int stride, int height)
{
//  MC_avg_x_8_c(dest, ref, stride, height);
}

static void MC_avg_y_8_arm (uint8_t * dest, const uint8_t * ref, int stride, int height)
{
//  MC_avg_y_8_c(dest, ref, stride, height);
}

static void MC_avg_xy_8_arm (uint8_t * dest, const uint8_t * ref, int stride, int height)
{
//  MC_avg_xy_8_c(dest, ref, stride, height);
}

DATA_IN_MTCM_SET MPEG2_MC_EXTERN (arm)

#endif
