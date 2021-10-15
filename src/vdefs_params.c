/**
 * Copyright (c) 2019 Parrot Drones SAS
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the Parrot Drones SAS Company nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE PARROT DRONES SAS COMPANY BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <video-defs/vdefs.h>

/* codecheck_ignore_file[LONG_LINE] */


/**
 * Derivation of luminance signal EY from colour signals ER, EG and EB:
 * EY = KR * ER + KG * EG + KB * EB   (E.1)
 * KR, KG and KB are defined so that KR + KG + KB = 1
 * so KG = 1 - KB - KR   (E.2)
 *
 * Derivation of colour-difference (chrominance) signals ECB and ECR:
 * ECB = (EB - EY) / A
 * ECR = (ER - EY) / B
 * ECB and ECR are defined such as their range is [-0.5 .. +0.5] so
 *   - for [ER, EG, EB] = [0, 0, 1], ECB is maximized and equal to 0.5
 *     EY = KB * EB and 0.5 = (EB - KB * EB) / A
 *     so A = (1 - KB) / 0.5 = 2 * (1 - KB)
 *   - for [ER, EG, EB] = [1, 0, 0], ECR is maximized and equal to 0.5
 *     EY = KR * ER and 0.5 = (ER - KR * ER) / B
 *     so B = (1 - KR) / 0.5 = 2 * (1 - KR)
 * ECB = (EB - EY) / (2 * (1 - KB))   (E.3)
 * ECR = (ER - EY) / (2 * (1 - KR))   (E.4)
 *
 * See the explanation in
 * - Rec. ITU-T H.273, chap. 8.3 "Matrix coefficients"
 * - or Rec. ITU-T H.264, E.2.1 VUI parameters semantics, matrix_coefficients
 * - or Rec. ITU-T H.265, E.3.1 VUI parameters semantics, matrix_coeffs
 */


/* Matrix coefficients */

/* Rec. ITU-R BT.601-6 */
#define VDEF_BT601_MAT_KR 0.299f
#define VDEF_BT601_MAT_KB 0.114f

/* Rec. ITU-R BT.709-5 */
#define VDEF_BT709_MAT_KR 0.2126f
#define VDEF_BT709_MAT_KB 0.0722f

/* Rec. ITU-R BT.2020 */
#define VDEF_BT2020_MAT_KR 0.2627f
#define VDEF_BT2020_MAT_KB 0.0593f


/* clang-format off */
/**
 * RGB to YUV colourspace conversion matrix:
 *
 *                        KR                       1 - KB - KR                          KB
 *      -KR / (2 * (1 - KB))   -(1 - KB - KR) / (2 * (1 - KB))   (1 - KB) / (2 * (1 - KB))
 * (1 - KR) / (2 * (1 - KR))   -(1 - KB - KR) / (2 * (1 - KR))        -KB / (2 * (1 - KR))
 *
 * Values (1 - KR) / (2 * (1 - KR)) and (1 - KB) / (2 * (1 - KB)) are
 * simplified as 0.5
 */
/* clang-format on */
#define VDEF_RGB2YUV_MAT_11(KR, KB) (KR)
#define VDEF_RGB2YUV_MAT_12(KR, KB) (1.f - KB - KR)
#define VDEF_RGB2YUV_MAT_13(KR, KB) (KB)
#define VDEF_RGB2YUV_MAT_21(KR, KB) (-KR / (2.f * (1.f - KB)))
#define VDEF_RGB2YUV_MAT_22(KR, KB) (-(1.f - KB - KR) / (2.f * (1.f - KB)))
#define VDEF_RGB2YUV_MAT_23(KR, KB) (0.5f)
#define VDEF_RGB2YUV_MAT_31(KR, KB) (0.5f)
#define VDEF_RGB2YUV_MAT_32(KR, KB) (-(1.f - KB - KR) / (2.f * (1.f - KR)))
#define VDEF_RGB2YUV_MAT_33(KR, KB) (-KB / (2.f * (1.f - KR)))


/* clang-format off */
/**
 * YUV to RGB colourspace conversion matrix:
 *
 * from E.3: EB = EY + 2 * (1 - KB) * ECB
 * from E.4: ER = EY + 2 * (1 - KR) * ECR
 * and from E.1: EY = KR * (EY + 2 * (1 - KR) * ECR)
 *                    + KG * EG
 *                    + KB * (EY + 2 * (1 - KB) * ECB)
 * so EG = ((1 - KB - KR) * EY - KB * 2 * (1 - KB) * ECB - KR * 2 * (1 - KR) * ECR) / KG
 * and from E.2, KG = 1 - KB - KR
 * so EG = EY - 2 * KB / KG * (1 - KB) * ECB - 2 * KR / KG * (1 - KR) * ECR
 * then the matrix is:
 *
 * 1                         0              2 * (1 - KR)
 * 1   -2 * KB / KG * (1 - KB)   -2 * KR / KG * (1 - KR)
 * 1              2 * (1 - KB)                         0
 */
/* clang-format on */
#define VDEF_YUV2RGB_MAT_11(KR, KB) (1.f)
#define VDEF_YUV2RGB_MAT_12(KR, KB) (0.f)
#define VDEF_YUV2RGB_MAT_13(KR, KB) (2.f * (1.f - KR))
#define VDEF_YUV2RGB_MAT_21(KR, KB) (1.f)
#define VDEF_YUV2RGB_MAT_22(KR, KB) (-2.f * KB / (1.f - KB - KR) * (1.f - KB))
#define VDEF_YUV2RGB_MAT_23(KR, KB) (-2.f * KR / (1.f - KB - KR) * (1.f - KR))
#define VDEF_YUV2RGB_MAT_31(KR, KB) (1.f)
#define VDEF_YUV2RGB_MAT_32(KR, KB) (2.f * (1.f - KB))
#define VDEF_YUV2RGB_MAT_33(KR, KB) (0.f)


/**
 * Digital representation (for 8-bit values)
 */

/* Rec. ITU-R BT.601-6 */
#define VDEF_BT601_LUMA_MIN 16
#define VDEF_BT601_LUMA_MAX 235
#define VDEF_BT601_LUMA_RANGE (VDEF_BT601_LUMA_MAX - VDEF_BT601_LUMA_MIN)
#define VDEF_BT601_CHROMA_MIN 16
#define VDEF_BT601_CHROMA_ZERO 128
#define VDEF_BT601_CHROMA_MAX 240
#define VDEF_BT601_CHROMA_RANGE (VDEF_BT601_CHROMA_MAX - VDEF_BT601_CHROMA_MIN)

/* Rec. ITU-R BT.709-5 */
#define VDEF_BT709_LUMA_MIN 16
#define VDEF_BT709_LUMA_MAX 235
#define VDEF_BT709_LUMA_RANGE (VDEF_BT709_LUMA_MAX - VDEF_BT709_LUMA_MIN)
#define VDEF_BT709_CHROMA_MIN 16
#define VDEF_BT709_CHROMA_ZERO 128
#define VDEF_BT709_CHROMA_MAX 240
#define VDEF_BT709_CHROMA_RANGE (VDEF_BT709_CHROMA_MAX - VDEF_BT709_CHROMA_MIN)

/* Rec. ITU-R BT.2020 */
#define VDEF_BT2020_LUMA_MIN 16
#define VDEF_BT2020_LUMA_MAX 235
#define VDEF_BT2020_LUMA_RANGE (VDEF_BT2020_LUMA_MAX - VDEF_BT2020_LUMA_MIN)
#define VDEF_BT2020_CHROMA_MIN 16
#define VDEF_BT2020_CHROMA_ZERO 128
#define VDEF_BT2020_CHROMA_MAX 240
#define VDEF_BT2020_CHROMA_RANGE                                               \
	(VDEF_BT2020_CHROMA_MAX - VDEF_BT2020_CHROMA_MIN)


/**
 * RGB to YUV conversion offsets for normalized values for both limited and
 * full range.
 * Note: the matrix multiplication must be done before applying the offset:
 * YUV = (RGB * mat) + off
 * so that values of luminance are in the range [0.0 .. 1.0] and values of
 * chrominance are in the range [-0.5 .. +0.5] in full range
 * or values of luminance are in the range [LUMA_MIN / 255 .. LUMA_MAX / 255]
 * and values of chrominance are in the range
 * [CHROMA_MIN - CHROMA_ZERO / 255 .. CHROMA_MAX - CHROMA_ZERO / 255] in
 * limited range.
 * Note: this table share the same values as vdef_yuv_to_rgb_norm_offset.
 */
/* clang-format off */
const float vdef_rgb_to_yuv_norm_offset[VDEF_MATRIX_COEFS_MAX][2][3] = {
	[VDEF_MATRIX_COEFS_UNKNOWN] = {
		{
			/* Limited range */
			0.f,
			0.f,
			0.f,
		},
		{
			/* Full range */
			0.f,
			0.f,
			0.f,
		},
	},
	[VDEF_MATRIX_COEFS_SRGB] = {
	/* same as VDEF_MATRIX_COEFS_IDENTITY */
		{
			/* Limited range */
			0.f,
			0.f,
			0.f,
		},
		{
			/* Full range */
			0.f,
			0.f,
			0.f,
		},
	},
	[VDEF_MATRIX_COEFS_BT601_525] = {
	/* VDEF_MATRIX_COEFS_BT601_525 and VDEF_MATRIX_COEFS_BT601_625
	 * share the same values */
		{
			/* Limited range */
			-(float)VDEF_BT601_LUMA_MIN / 255.f,
			-(float)VDEF_BT601_CHROMA_ZERO / 255.f,
			-(float)VDEF_BT601_CHROMA_ZERO / 255.f,
		},
		{
			/* Full range */
			0.f,
			-(float)VDEF_BT601_CHROMA_ZERO / 255.f,
			-(float)VDEF_BT601_CHROMA_ZERO / 255.f,
		},
	},
	[VDEF_MATRIX_COEFS_BT601_625] = {
	/* VDEF_MATRIX_COEFS_BT601_525 and VDEF_MATRIX_COEFS_BT601_625
	 * share the same values */
		{
			/* Limited range */
			-(float)VDEF_BT601_LUMA_MIN / 255.f,
			-(float)VDEF_BT601_CHROMA_ZERO / 255.f,
			-(float)VDEF_BT601_CHROMA_ZERO / 255.f,
		},
		{
			/* Full range */
			0.f,
			-(float)VDEF_BT601_CHROMA_ZERO / 255.f,
			-(float)VDEF_BT601_CHROMA_ZERO / 255.f,
		},
	},
	[VDEF_MATRIX_COEFS_BT709] = {
		{
			/* Limited range */
			-(float)VDEF_BT709_LUMA_MIN / 255.f,
			-(float)VDEF_BT709_CHROMA_ZERO / 255.f,
			-(float)VDEF_BT709_CHROMA_ZERO / 255.f,
		},
		{
			/* Full range */
			0.f,
			-(float)VDEF_BT709_CHROMA_ZERO / 255.f,
			-(float)VDEF_BT709_CHROMA_ZERO / 255.f,
		},
	},
	[VDEF_MATRIX_COEFS_BT2020_NON_CST] = {
	/* same as VDEF_MATRIX_COEFS_BT2100;
	 * VDEF_MATRIX_COEFS_BT2020_NON_CST and VDEF_MATRIX_COEFS_BT2020_CST
	 * share the same values */
		{
			/* Limited range */
			-(float)VDEF_BT2020_LUMA_MIN / 255.f,
			-(float)VDEF_BT2020_CHROMA_ZERO / 255.f,
			-(float)VDEF_BT2020_CHROMA_ZERO / 255.f,
		},
		{
			/* Full range */
			0.f,
			-(float)VDEF_BT2020_CHROMA_ZERO / 255.f,
			-(float)VDEF_BT2020_CHROMA_ZERO / 255.f,
		},
	},
	[VDEF_MATRIX_COEFS_BT2020_CST] = {
	/* VDEF_MATRIX_COEFS_BT2020_NON_CST and VDEF_MATRIX_COEFS_BT2020_CST
	 * share the same values */
		{
			/* Limited range */
			-(float)VDEF_BT2020_LUMA_MIN / 255.f,
			-(float)VDEF_BT2020_CHROMA_ZERO / 255.f,
			-(float)VDEF_BT2020_CHROMA_ZERO / 255.f,
		},
		{
			/* Full range */
			0.f,
			-(float)VDEF_BT2020_CHROMA_ZERO / 255.f,
			-(float)VDEF_BT2020_CHROMA_ZERO / 255.f,
		},
	},
};
/* clang-format on */


/**
 * RGB to YUV conversion matrix for normalized values for both limited and
 * full range.
 * The matrix is in column-major order, which makes it usable in OpenGL.
 * Note: the matrix multiplication must be done before applying the offset:
 * YUV = (RGB * mat) + off
 * so that values of luminance are in the range [0.0 .. 1.0] and values of
 * chrominance are in the range [-0.5 .. +0.5] in full range
 * or values of luminance are in the range [LUMA_MIN / 255 .. LUMA_MAX / 255]
 * and values of chrominance are in the range
 * [CHROMA_MIN - CHROMA_ZERO / 255 .. CHROMA_MAX - CHROMA_ZERO / 255] in
 * limited range.
 */
/* clang-format off */
const float vdef_rgb_to_yuv_norm_matrix[VDEF_MATRIX_COEFS_MAX][2][9] = {
	[VDEF_MATRIX_COEFS_UNKNOWN] = {
		{
			/* Limited range */
			0.f,
			0.f,
			0.f,
			0.f,
			0.f,
			0.f,
			0.f,
			0.f,
			0.f,
		},
		{
			/* Full range */
			0.f,
			0.f,
			0.f,
			0.f,
			0.f,
			0.f,
			0.f,
			0.f,
			0.f,
		},
	},
	[VDEF_MATRIX_COEFS_SRGB] = {
	/* same as VDEF_MATRIX_COEFS_IDENTITY */
		{
			/* Limited range */
			1.f,
			0.f,
			0.f,
			0.f,
			1.f,
			0.f,
			0.f,
			0.f,
			1.f,
		},
		{
			/* Full range */
			1.f,
			0.f,
			0.f,
			0.f,
			1.f,
			0.f,
			0.f,
			0.f,
			1.f,
		},
	},
	[VDEF_MATRIX_COEFS_BT601_525] = {
	/* VDEF_MATRIX_COEFS_BT601_525 and VDEF_MATRIX_COEFS_BT601_625
	 * share the same values */
		{
			/* Limited range */
			VDEF_RGB2YUV_MAT_11(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) *
				(float)VDEF_BT601_LUMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_21(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) *
				(float)VDEF_BT601_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_31(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) *
				(float)VDEF_BT601_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_12(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) *
				(float)VDEF_BT601_LUMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_22(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) *
				(float)VDEF_BT601_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_32(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) *
				(float)VDEF_BT601_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_13(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) *
				(float)VDEF_BT601_LUMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_23(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) *
				(float)VDEF_BT601_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_33(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) *
				(float)VDEF_BT601_CHROMA_RANGE / 255.f,
		},
		{
			/* Full range */
			VDEF_RGB2YUV_MAT_11(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_RGB2YUV_MAT_21(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_RGB2YUV_MAT_31(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_RGB2YUV_MAT_12(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_RGB2YUV_MAT_22(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_RGB2YUV_MAT_32(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_RGB2YUV_MAT_13(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_RGB2YUV_MAT_23(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_RGB2YUV_MAT_33(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
		},
	},
	[VDEF_MATRIX_COEFS_BT601_625] = {
	/* VDEF_MATRIX_COEFS_BT601_525 and VDEF_MATRIX_COEFS_BT601_625
	 * share the same values */
		{
			/* Limited range */
			VDEF_RGB2YUV_MAT_11(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) *
				(float)VDEF_BT601_LUMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_21(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) *
				(float)VDEF_BT601_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_31(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) *
				(float)VDEF_BT601_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_12(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) *
				(float)VDEF_BT601_LUMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_22(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) *
				(float)VDEF_BT601_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_32(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) *
				(float)VDEF_BT601_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_13(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) *
				(float)VDEF_BT601_LUMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_23(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) *
				(float)VDEF_BT601_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_33(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) *
				(float)VDEF_BT601_CHROMA_RANGE / 255.f,
		},
		{
			/* Full range */
			VDEF_RGB2YUV_MAT_11(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_RGB2YUV_MAT_21(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_RGB2YUV_MAT_31(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_RGB2YUV_MAT_12(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_RGB2YUV_MAT_22(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_RGB2YUV_MAT_32(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_RGB2YUV_MAT_13(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_RGB2YUV_MAT_23(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_RGB2YUV_MAT_33(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
		},
	},
	[VDEF_MATRIX_COEFS_BT709] = {
		{
			/* Limited range */
			VDEF_RGB2YUV_MAT_11(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB) *
				(float)VDEF_BT709_LUMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_21(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB) *
				(float)VDEF_BT709_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_31(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB) *
				(float)VDEF_BT709_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_12(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB) *
				(float)VDEF_BT709_LUMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_22(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB) *
				(float)VDEF_BT709_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_32(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB) *
				(float)VDEF_BT709_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_13(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB) *
				(float)VDEF_BT709_LUMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_23(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB) *
				(float)VDEF_BT709_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_33(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB) *
				(float)VDEF_BT709_CHROMA_RANGE / 255.f,
		},
		{
			/* Full range */
			VDEF_RGB2YUV_MAT_11(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB),
			VDEF_RGB2YUV_MAT_21(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB),
			VDEF_RGB2YUV_MAT_31(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB),
			VDEF_RGB2YUV_MAT_12(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB),
			VDEF_RGB2YUV_MAT_22(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB),
			VDEF_RGB2YUV_MAT_32(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB),
			VDEF_RGB2YUV_MAT_13(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB),
			VDEF_RGB2YUV_MAT_23(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB),
			VDEF_RGB2YUV_MAT_33(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB),
		},
	},
	[VDEF_MATRIX_COEFS_BT2020_NON_CST] = {
	/* same as VDEF_MATRIX_COEFS_BT2100;
	 * VDEF_MATRIX_COEFS_BT2020_NON_CST and VDEF_MATRIX_COEFS_BT2020_CST
	 * share the same values */
		{
			/* Limited range */
			VDEF_RGB2YUV_MAT_11(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) *
				(float)VDEF_BT2020_LUMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_21(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) *
				(float)VDEF_BT2020_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_31(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) *
				(float)VDEF_BT2020_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_12(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) *
				(float)VDEF_BT2020_LUMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_22(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) *
				(float)VDEF_BT2020_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_32(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) *
				(float)VDEF_BT2020_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_13(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) *
				(float)VDEF_BT2020_LUMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_23(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) *
				(float)VDEF_BT2020_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_33(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) *
				(float)VDEF_BT2020_CHROMA_RANGE / 255.f,
		},
		{
			/* Full range */
			VDEF_RGB2YUV_MAT_11(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_RGB2YUV_MAT_21(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_RGB2YUV_MAT_31(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_RGB2YUV_MAT_12(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_RGB2YUV_MAT_22(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_RGB2YUV_MAT_32(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_RGB2YUV_MAT_13(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_RGB2YUV_MAT_23(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_RGB2YUV_MAT_33(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
		},
	},
	[VDEF_MATRIX_COEFS_BT2020_CST] = {
	/* VDEF_MATRIX_COEFS_BT2020_NON_CST and VDEF_MATRIX_COEFS_BT2020_CST
	 * share the same values */
		{
			/* Limited range */
			VDEF_RGB2YUV_MAT_11(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) *
				(float)VDEF_BT2020_LUMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_21(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) *
				(float)VDEF_BT2020_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_31(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) *
				(float)VDEF_BT2020_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_12(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) *
				(float)VDEF_BT2020_LUMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_22(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) *
				(float)VDEF_BT2020_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_32(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) *
				(float)VDEF_BT2020_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_13(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) *
				(float)VDEF_BT2020_LUMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_23(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) *
				(float)VDEF_BT2020_CHROMA_RANGE / 255.f,
			VDEF_RGB2YUV_MAT_33(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) *
				(float)VDEF_BT2020_CHROMA_RANGE / 255.f,
		},
		{
			/* Full range */
			VDEF_RGB2YUV_MAT_11(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_RGB2YUV_MAT_21(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_RGB2YUV_MAT_31(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_RGB2YUV_MAT_12(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_RGB2YUV_MAT_22(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_RGB2YUV_MAT_32(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_RGB2YUV_MAT_13(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_RGB2YUV_MAT_23(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_RGB2YUV_MAT_33(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
		},
	},
};
/* clang-format on */


/**
 * YUV to RGB conversion offsets for normalized values for both limited and
 * full range.
 * Note: the matrix multiplication must be done after applying the offset:
 * RGB = (YUV + off) * mat
 * so that values of R, G and B are in the range [0.0 .. 1.0].
 * Note: this table share the same values as vdef_rgb_to_yuv_norm_offset.
 */
/* clang-format off */
const float vdef_yuv_to_rgb_norm_offset[VDEF_MATRIX_COEFS_MAX][2][3] = {
	[VDEF_MATRIX_COEFS_UNKNOWN] = {
		{
			/* Limited range */
			0.f,
			0.f,
			0.f,
		},
		{
			/* Full range */
			0.f,
			0.f,
			0.f,
		},
	},
	[VDEF_MATRIX_COEFS_SRGB] = {
	/* same as VDEF_MATRIX_COEFS_IDENTITY */
		{
			/* Limited range */
			0.f,
			0.f,
			0.f,
		},
		{
			/* Full range */
			0.f,
			0.f,
			0.f,
		},
	},
	[VDEF_MATRIX_COEFS_BT601_525] = {
	/* VDEF_MATRIX_COEFS_BT601_525 and VDEF_MATRIX_COEFS_BT601_625
	 * share the same values */
		{
			/* Limited range */
			-(float)VDEF_BT601_LUMA_MIN / 255.f,
			-(float)VDEF_BT601_CHROMA_ZERO / 255.f,
			-(float)VDEF_BT601_CHROMA_ZERO / 255.f,
		},
		{
			/* Full range */
			0.f,
			-(float)VDEF_BT601_CHROMA_ZERO / 255.f,
			-(float)VDEF_BT601_CHROMA_ZERO / 255.f,
		},
	},
	[VDEF_MATRIX_COEFS_BT601_625] = {
	/* VDEF_MATRIX_COEFS_BT601_525 and VDEF_MATRIX_COEFS_BT601_625
	 * share the same values */
		{
			/* Limited range */
			-(float)VDEF_BT601_LUMA_MIN / 255.f,
			-(float)VDEF_BT601_CHROMA_ZERO / 255.f,
			-(float)VDEF_BT601_CHROMA_ZERO / 255.f,
		},
		{
			/* Full range */
			0.f,
			-(float)VDEF_BT601_CHROMA_ZERO / 255.f,
			-(float)VDEF_BT601_CHROMA_ZERO / 255.f,
		},
	},
	[VDEF_MATRIX_COEFS_BT709] = {
		{
			/* Limited range */
			-(float)VDEF_BT709_LUMA_MIN / 255.f,
			-(float)VDEF_BT709_CHROMA_ZERO / 255.f,
			-(float)VDEF_BT709_CHROMA_ZERO / 255.f,
		},
		{
			/* Full range */
			0.f,
			-(float)VDEF_BT709_CHROMA_ZERO / 255.f,
			-(float)VDEF_BT709_CHROMA_ZERO / 255.f,
		},
	},
	[VDEF_MATRIX_COEFS_BT2020_NON_CST] = {
	/* same as VDEF_MATRIX_COEFS_BT2100;
	 * VDEF_MATRIX_COEFS_BT2020_NON_CST and VDEF_MATRIX_COEFS_BT2020_CST
	 * share the same values */
		{
			/* Limited range */
			-(float)VDEF_BT2020_LUMA_MIN / 255.f,
			-(float)VDEF_BT2020_CHROMA_ZERO / 255.f,
			-(float)VDEF_BT2020_CHROMA_ZERO / 255.f,
		},
		{
			/* Full range */
			0.f,
			-(float)VDEF_BT2020_CHROMA_ZERO / 255.f,
			-(float)VDEF_BT2020_CHROMA_ZERO / 255.f,
		},
	},
	[VDEF_MATRIX_COEFS_BT2020_CST] = {
	/* VDEF_MATRIX_COEFS_BT2020_NON_CST and VDEF_MATRIX_COEFS_BT2020_CST
	 * share the same values */
		{
			/* Limited range */
			-(float)VDEF_BT2020_LUMA_MIN / 255.f,
			-(float)VDEF_BT2020_CHROMA_ZERO / 255.f,
			-(float)VDEF_BT2020_CHROMA_ZERO / 255.f,
		},
		{
			/* Full range */
			0.f,
			-(float)VDEF_BT2020_CHROMA_ZERO / 255.f,
			-(float)VDEF_BT2020_CHROMA_ZERO / 255.f,
		},
	},
};
/* clang-format on */


/**
 * YUV to RGB conversion matrix for normalized values for both limited and
 * full range.
 * The matrix is in column-major order, which makes it usable in OpenGL.
 * Note: the matrix multiplication must be done after applying the offset:
 * RGB = (YUV + off) * mat
 * so that values of R, G and B are in the range [0.0 .. 1.0].
 */
/* clang-format off */
const float vdef_yuv_to_rgb_norm_matrix[VDEF_MATRIX_COEFS_MAX][2][9] = {
	[VDEF_MATRIX_COEFS_UNKNOWN] = {
		{
			/* Limited range */
			0.f,
			0.f,
			0.f,
			0.f,
			0.f,
			0.f,
			0.f,
			0.f,
			0.f,
		},
		{
			/* Full range */
			0.f,
			0.f,
			0.f,
			0.f,
			0.f,
			0.f,
			0.f,
			0.f,
			0.f,
		},
	},
	[VDEF_MATRIX_COEFS_SRGB] = {
	/* same as VDEF_MATRIX_COEFS_IDENTITY */
		{
			/* Limited range */
			1.f,
			0.f,
			0.f,
			0.f,
			1.f,
			0.f,
			0.f,
			0.f,
			1.f,
		},
		{
			/* Full range */
			1.f,
			0.f,
			0.f,
			0.f,
			1.f,
			0.f,
			0.f,
			0.f,
			1.f,
		},
	},
	[VDEF_MATRIX_COEFS_BT601_525] = {
	/* VDEF_MATRIX_COEFS_BT601_525 and VDEF_MATRIX_COEFS_BT601_625
	 * share the same values */
		{
			/* Limited range */
			VDEF_YUV2RGB_MAT_11(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) * 255.f /
				(float)VDEF_BT601_LUMA_RANGE,
			VDEF_YUV2RGB_MAT_21(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) * 255.f /
				(float)VDEF_BT601_LUMA_RANGE,
			VDEF_YUV2RGB_MAT_31(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) * 255.f /
				(float)VDEF_BT601_LUMA_RANGE,
			VDEF_YUV2RGB_MAT_12(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) * 255.f /
				(float)VDEF_BT601_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_22(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) * 255.f /
				(float)VDEF_BT601_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_32(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) * 255.f /
				(float)VDEF_BT601_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_13(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) * 255.f /
				(float)VDEF_BT601_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_23(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) * 255.f /
				(float)VDEF_BT601_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_33(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) * 255.f /
				(float)VDEF_BT601_CHROMA_RANGE,
		},
		{
			/* Full range */
			VDEF_YUV2RGB_MAT_11(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_YUV2RGB_MAT_21(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_YUV2RGB_MAT_31(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_YUV2RGB_MAT_12(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_YUV2RGB_MAT_22(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_YUV2RGB_MAT_32(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_YUV2RGB_MAT_13(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_YUV2RGB_MAT_23(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_YUV2RGB_MAT_33(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
		},
	},
	[VDEF_MATRIX_COEFS_BT601_625] = {
	/* VDEF_MATRIX_COEFS_BT601_525 and VDEF_MATRIX_COEFS_BT601_625
	 * share the same values */
		{
			/* Limited range */
			VDEF_YUV2RGB_MAT_11(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) * 255.f /
				(float)VDEF_BT601_LUMA_RANGE,
			VDEF_YUV2RGB_MAT_21(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) * 255.f /
				(float)VDEF_BT601_LUMA_RANGE,
			VDEF_YUV2RGB_MAT_31(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) * 255.f /
				(float)VDEF_BT601_LUMA_RANGE,
			VDEF_YUV2RGB_MAT_12(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) * 255.f /
				(float)VDEF_BT601_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_22(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) * 255.f /
				(float)VDEF_BT601_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_32(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) * 255.f /
				(float)VDEF_BT601_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_13(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) * 255.f /
				(float)VDEF_BT601_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_23(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) * 255.f /
				(float)VDEF_BT601_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_33(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB) * 255.f /
				(float)VDEF_BT601_CHROMA_RANGE,
		},
		{
			/* Full range */
			VDEF_YUV2RGB_MAT_11(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_YUV2RGB_MAT_21(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_YUV2RGB_MAT_31(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_YUV2RGB_MAT_12(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_YUV2RGB_MAT_22(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_YUV2RGB_MAT_32(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_YUV2RGB_MAT_13(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_YUV2RGB_MAT_23(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
			VDEF_YUV2RGB_MAT_33(VDEF_BT601_MAT_KR,
					    VDEF_BT601_MAT_KB),
		},
	},
	[VDEF_MATRIX_COEFS_BT709] = {
		{
			/* Limited range */
			VDEF_YUV2RGB_MAT_11(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB) * 255.f /
				(float)VDEF_BT709_LUMA_RANGE,
			VDEF_YUV2RGB_MAT_21(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB) * 255.f /
				(float)VDEF_BT709_LUMA_RANGE,
			VDEF_YUV2RGB_MAT_31(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB) * 255.f /
				(float)VDEF_BT709_LUMA_RANGE,
			VDEF_YUV2RGB_MAT_12(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB) * 255.f /
				(float)VDEF_BT709_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_22(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB) * 255.f /
				(float)VDEF_BT709_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_32(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB) * 255.f /
				(float)VDEF_BT709_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_13(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB) * 255.f /
				(float)VDEF_BT709_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_23(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB) * 255.f /
				(float)VDEF_BT709_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_33(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB) * 255.f /
				(float)VDEF_BT709_CHROMA_RANGE,
		},
		{
			/* Full range */
			VDEF_YUV2RGB_MAT_11(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB),
			VDEF_YUV2RGB_MAT_21(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB),
			VDEF_YUV2RGB_MAT_31(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB),
			VDEF_YUV2RGB_MAT_12(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB),
			VDEF_YUV2RGB_MAT_22(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB),
			VDEF_YUV2RGB_MAT_32(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB),
			VDEF_YUV2RGB_MAT_13(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB),
			VDEF_YUV2RGB_MAT_23(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB),
			VDEF_YUV2RGB_MAT_33(VDEF_BT709_MAT_KR,
					    VDEF_BT709_MAT_KB),
		},
	},
	[VDEF_MATRIX_COEFS_BT2020_NON_CST] = {
	/* same as VDEF_MATRIX_COEFS_BT2100;
	 * VDEF_MATRIX_COEFS_BT2020_NON_CST and VDEF_MATRIX_COEFS_BT2020_CST
	 * share the same values */
		{
			/* Limited range */
			VDEF_YUV2RGB_MAT_11(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) * 255.f /
				(float)VDEF_BT2020_LUMA_RANGE,
			VDEF_YUV2RGB_MAT_21(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) * 255.f /
				(float)VDEF_BT2020_LUMA_RANGE,
			VDEF_YUV2RGB_MAT_31(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) * 255.f /
				(float)VDEF_BT2020_LUMA_RANGE,
			VDEF_YUV2RGB_MAT_12(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) * 255.f /
				(float)VDEF_BT2020_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_22(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) * 255.f /
				(float)VDEF_BT2020_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_32(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) * 255.f /
				(float)VDEF_BT2020_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_13(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) * 255.f /
				(float)VDEF_BT2020_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_23(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) * 255.f /
				(float)VDEF_BT2020_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_33(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) * 255.f /
				(float)VDEF_BT2020_CHROMA_RANGE,
		},
		{
			/* Full range */
			VDEF_YUV2RGB_MAT_11(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_YUV2RGB_MAT_21(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_YUV2RGB_MAT_31(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_YUV2RGB_MAT_12(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_YUV2RGB_MAT_22(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_YUV2RGB_MAT_32(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_YUV2RGB_MAT_13(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_YUV2RGB_MAT_23(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_YUV2RGB_MAT_33(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
		},
	},
	[VDEF_MATRIX_COEFS_BT2020_CST] = {
	/* VDEF_MATRIX_COEFS_BT2020_NON_CST and VDEF_MATRIX_COEFS_BT2020_CST
	 * share the same values */
		{
			/* Limited range */
			VDEF_YUV2RGB_MAT_11(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) * 255.f /
				(float)VDEF_BT2020_LUMA_RANGE,
			VDEF_YUV2RGB_MAT_21(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) * 255.f /
				(float)VDEF_BT2020_LUMA_RANGE,
			VDEF_YUV2RGB_MAT_31(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) * 255.f /
				(float)VDEF_BT2020_LUMA_RANGE,
			VDEF_YUV2RGB_MAT_12(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) * 255.f /
				(float)VDEF_BT2020_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_22(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) * 255.f /
				(float)VDEF_BT2020_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_32(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) * 255.f /
				(float)VDEF_BT2020_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_13(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) * 255.f /
				(float)VDEF_BT2020_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_23(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) * 255.f /
				(float)VDEF_BT2020_CHROMA_RANGE,
			VDEF_YUV2RGB_MAT_33(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB) * 255.f /
				(float)VDEF_BT2020_CHROMA_RANGE,
		},
		{
			/* Full range */
			VDEF_YUV2RGB_MAT_11(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_YUV2RGB_MAT_21(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_YUV2RGB_MAT_31(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_YUV2RGB_MAT_12(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_YUV2RGB_MAT_22(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_YUV2RGB_MAT_32(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_YUV2RGB_MAT_13(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_YUV2RGB_MAT_23(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
			VDEF_YUV2RGB_MAT_33(VDEF_BT2020_MAT_KR,
					    VDEF_BT2020_MAT_KB),
		},
	},
};
/* clang-format on */


/**
 * BT.709 to BT.2020 conversion matrix for linearly represented, normalized
 * RGB values.
 * See Rec. ITU-R BT.2087.
 * The matrix is in column-major order, which makes it usable in OpenGL.
 */
/* clang-format off */
const float vdef_bt709_to_bt2020_matrix[9] = {
	0.6274f, 0.0691f, 0.0164f,
	0.3293f, 0.9195f, 0.0880f,
	0.0433f, 0.0114f, 0.8956f,
};
/* clang-format on */

/**
 * BT.2020 to BT.709 conversion matrix for linearly represented, normalized
 * RGB values.
 * See Rep. ITU-R BT.2407.
 * The matrix is in column-major order, which makes it usable in OpenGL.
 * Note: no tone mapping is done here, so that resulting R, G and B values
 * can be less than 0 or greater than 1 for BT.2020 values outside of the
 * BT.709 gamut; these values will be clipped which can lead to significant
 * shift in hue; however this does not alter colours within the BT.709 gamut.
 */
/* clang-format off */
const float vdef_bt2020_to_bt709_matrix[9] = {
	 1.6605f, -0.1246f, -0.0182f,
	-0.5876f,  1.1329f, -0.1006f,
	-0.0728f, -0.0083f,  1.1187f,
};
/* clang-format on */
