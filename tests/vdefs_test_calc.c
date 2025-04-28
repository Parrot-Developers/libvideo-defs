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

#include "vdefs_test.h"
#include <futils/futils.h>

static const struct vdef_raw_format vdef_i444_semi = {
	.pix_format = VDEF_RAW_PIX_FORMAT_YUV444,
	.pix_order = VDEF_RAW_PIX_ORDER_YUV,
	.pix_layout = VDEF_RAW_PIX_LAYOUT_LINEAR,
	.pix_size = 8,
	.data_layout = VDEF_RAW_DATA_LAYOUT_SEMI_PLANAR_Y_UV,
	.data_pad_low = false,
	.data_little_endian = false,
	.data_size = 8,
};
static const struct vdef_raw_format vdef_rgb_planar = {
	.pix_format = VDEF_RAW_PIX_FORMAT_RGB24,
	.pix_order = VDEF_RAW_PIX_ORDER_RGB,
	.pix_layout = VDEF_RAW_PIX_LAYOUT_LINEAR,
	.pix_size = 8,
	.data_layout = VDEF_RAW_DATA_LAYOUT_PLANAR_R_G_B,
	.data_pad_low = false,
	.data_little_endian = false,
	.data_size = 8,
};

static const struct vdef_raw_format vdef_rgba_planar = {
	.pix_format = VDEF_RAW_PIX_FORMAT_RGBA32,
	.pix_order = VDEF_RAW_PIX_ORDER_RGBA,
	.pix_layout = VDEF_RAW_PIX_LAYOUT_LINEAR,
	.pix_size = 8,
	.data_layout = VDEF_RAW_DATA_LAYOUT_PLANAR_R_G_B_A,
	.data_pad_low = false,
	.data_little_endian = false,
	.data_size = 8,
};


struct single_test {
	/* Resolution */
	const struct vdef_dim res;
	/* Stride */
	const size_t input_stride[VDEF_RAW_MAX_PLANE_COUNT];
	const unsigned int stride_align[VDEF_RAW_MAX_PLANE_COUNT];
	const size_t expected_stride[VDEF_RAW_MAX_PLANE_COUNT];
	/* Scanline */
	const size_t input_scanline[VDEF_RAW_MAX_PLANE_COUNT];
	const unsigned int scanline_align[VDEF_RAW_MAX_PLANE_COUNT];
	const size_t expected_scanline[VDEF_RAW_MAX_PLANE_COUNT];
	/* Size */
	const size_t input_size[VDEF_RAW_MAX_PLANE_COUNT];
	const unsigned int size_align[VDEF_RAW_MAX_PLANE_COUNT];
	const size_t expected_size[VDEF_RAW_MAX_PLANE_COUNT];
};

struct test_case {
	const struct vdef_raw_format *fmt;
	struct single_test tests[];
};

static void run_single_test(const struct vdef_raw_format *fmt,
			    struct single_test *st)
{
	size_t pst[VDEF_RAW_MAX_PLANE_COUNT] = {0};
	size_t psc[VDEF_RAW_MAX_PLANE_COUNT] = {0};
	size_t psz[VDEF_RAW_MAX_PLANE_COUNT] = {0};

	printf(" -- Testing %ux%u\n", st->res.width, st->res.height);

	memcpy(pst, st->input_stride, sizeof(pst));
	memcpy(psc, st->input_scanline, sizeof(psc));
	memcpy(psz, st->input_size, sizeof(psz));
	int ret = vdef_calc_raw_frame_size(fmt,
					   &st->res,
					   pst,
					   st->stride_align,
					   psc,
					   st->scanline_align,
					   psz,
					   st->size_align);
	CU_ASSERT_EQUAL(ret, 0);
	if (ret != 0)
		return;

	bool st_ok = true;
	bool sc_ok = true;
	bool sz_ok = true;
	for (int i = 0; i < VDEF_RAW_MAX_PLANE_COUNT; i++) {
		if (pst[i] != st->expected_stride[i]) {
			st_ok = false;
		}
		if (psc[i] != st->expected_scanline[i]) {
			sc_ok = false;
		}
		if (psz[i] != st->expected_size[i]) {
			sz_ok = false;
		}
	}

	if (!st_ok) {
		CU_FAIL("stride is wrong");
		printf("Stride: Expected {%zu, %zu, %zu, %zu}, got {%zu, %zu, %zu, %zu}\n",
		       st->expected_stride[0],
		       st->expected_stride[1],
		       st->expected_stride[2],
		       st->expected_stride[3],
		       pst[0],
		       pst[1],
		       pst[2],
		       pst[3]);
	}
	if (!sc_ok) {
		CU_FAIL("scanline is wrong");
		printf("Scanline: Expected {%zu, %zu, %zu, %zu}, got {%zu, %zu, %zu, %zu}\n",
		       st->expected_scanline[0],
		       st->expected_scanline[1],
		       st->expected_scanline[2],
		       st->expected_scanline[3],
		       psc[0],
		       psc[1],
		       psc[2],
		       psc[3]);
	}
	if (!sz_ok) {
		CU_FAIL("size is wrong");
		printf("Size: Expected {%zu, %zu, %zu, %zu}, got {%zu, %zu, %zu, %zu}\n",
		       st->expected_size[0],
		       st->expected_size[1],
		       st->expected_size[2],
		       st->expected_size[3],
		       psz[0],
		       psz[1],
		       psz[2],
		       psz[3]);
	}
}

static void run_test_case(struct test_case *t)
{
	char *str = vdef_raw_format_to_str(t->fmt);
	printf("Testing %s\n", str);
	free(str);
	int idx = 0;
	struct single_test *st;
	while (true) {
		st = &t->tests[idx++];
		if (st->res.width == 0)
			break;
		run_single_test(t->fmt, st);
	}
}

static void run_test_contiguous(const struct vdef_raw_format *fmt,
				struct single_test *st)
{
	size_t pst[VDEF_RAW_MAX_PLANE_COUNT] = {0};
	size_t psc[VDEF_RAW_MAX_PLANE_COUNT] = {0};
	size_t expected_contiguous_size = 0;

	printf(" -- Testing %ux%u\n", st->res.width, st->res.height);

	memcpy(pst, st->input_stride, sizeof(pst));
	memcpy(psc, st->input_scanline, sizeof(psc));
	size_t contiguous_frame_size =
		vdef_calc_raw_contiguous_frame_size(fmt,
						    &st->res,
						    pst,
						    st->stride_align,
						    psc,
						    st->scanline_align,
						    st->size_align);
	for (size_t i = 0; i < VDEF_RAW_MAX_PLANE_COUNT; i++)
		expected_contiguous_size += st->expected_size[i];
	if (contiguous_frame_size != expected_contiguous_size) {
		CU_FAIL("contiguous frame size is wrong");
		printf("Contiguous frame size: Expected %zu, got %zu\n",
		       expected_contiguous_size,
		       contiguous_frame_size);
	}
}

#define END_TEST_ARRAY                                                         \
	{                                                                      \
		.res = { 0 }                                                   \
	}


struct test_case raw8 = {.fmt = &vdef_raw8,
			 .tests = {
				 {
					 .res = {8000, 6000},
					 .expected_stride = {8000},
					 .expected_scanline = {6000},
					 .expected_size = {48000000},
				 },
				 {
					 .res = {1920, 1080},
					 .expected_stride = {1920},
					 .expected_scanline = {1080},
					 .expected_size = {2073600},
				 },
				 {
					 .res = {1280, 720},
					 .expected_stride = {1280},
					 .expected_scanline = {720},
					 .expected_size = {921600},
				 },
				 {
					 .res = {1920, 1080},
					 .stride_align = {1024},
					 .expected_stride = {2048},
					 .scanline_align = {1024},
					 .expected_scanline = {2048},
					 .size_align = {1024},
					 .expected_size = {4194304},
				 },
				 END_TEST_ARRAY,
			 }};
struct test_case raw10_packed = {.fmt = &vdef_raw10_packed,
				 .tests = {
					 {
						 .res = {8000, 6000},
						 .expected_stride = {10000},
						 .expected_scanline = {6000},
						 .expected_size = {60000000},
					 },
					 {
						 .res = {1920, 1080},
						 .expected_stride = {2400},
						 .expected_scanline = {1080},
						 .expected_size = {2592000},
					 },
					 {
						 .res = {1280, 720},
						 .expected_stride = {1600},
						 .expected_scanline = {720},
						 .expected_size = {1152000},
					 },
					 {
						 .res = {1920, 1080},
						 .stride_align = {1024},
						 .expected_stride = {3072},
						 .scanline_align = {1024},
						 .expected_scanline = {2048},
						 .size_align = {1024},
						 .expected_size = {6291456},
					 },
					 END_TEST_ARRAY,
				 }};
struct test_case raw10 = {.fmt = &vdef_raw10,
			  .tests = {
				  {
					  .res = {8000, 6000},
					  .expected_stride = {16000},
					  .expected_scanline = {6000},
					  .expected_size = {96000000},
				  },
				  {
					  .res = {1920, 1080},
					  .expected_stride = {3840},
					  .expected_scanline = {1080},
					  .expected_size = {4147200},
				  },
				  {
					  .res = {1280, 720},
					  .expected_stride = {2560},
					  .expected_scanline = {720},
					  .expected_size = {1843200},
				  },
				  {
					  .res = {1920, 1080},
					  .stride_align = {1024},
					  .expected_stride = {4096},
					  .scanline_align = {1024},
					  .expected_scanline = {2048},
					  .size_align = {1024},
					  .expected_size = {8388608},
				  },
				  END_TEST_ARRAY,
			  }};
struct test_case raw12_packed = {.fmt = &vdef_raw12_packed,
				 .tests = {
					 {
						 .res = {8000, 6000},
						 .expected_stride = {12000},
						 .expected_scanline = {6000},
						 .expected_size = {72000000},
					 },
					 {
						 .res = {1920, 1080},
						 .expected_stride = {2880},
						 .expected_scanline = {1080},
						 .expected_size = {3110400},
					 },
					 {
						 .res = {1280, 720},
						 .expected_stride = {1920},
						 .expected_scanline = {720},
						 .expected_size = {1382400},
					 },
					 {
						 .res = {1920, 1080},
						 .stride_align = {1024},
						 .expected_stride = {3072},
						 .scanline_align = {1024},
						 .expected_scanline = {2048},
						 .size_align = {1024},
						 .expected_size = {6291456},
					 },
					 END_TEST_ARRAY,
				 }};
struct test_case raw12 = {.fmt = &vdef_raw12,
			  .tests = {
				  {
					  .res = {8000, 6000},
					  .expected_stride = {16000},
					  .expected_scanline = {6000},
					  .expected_size = {96000000},
				  },
				  {
					  .res = {1920, 1080},
					  .expected_stride = {3840},
					  .expected_scanline = {1080},
					  .expected_size = {4147200},
				  },
				  {
					  .res = {1280, 720},
					  .expected_stride = {2560},
					  .expected_scanline = {720},
					  .expected_size = {1843200},
				  },
				  {
					  .res = {1920, 1080},
					  .stride_align = {1024},
					  .expected_stride = {4096},
					  .scanline_align = {1024},
					  .expected_scanline = {2048},
					  .size_align = {1024},
					  .expected_size = {8388608},
				  },
				  END_TEST_ARRAY,
			  }};
struct test_case raw14_packed = {.fmt = &vdef_raw14_packed,
				 .tests = {
					 {
						 .res = {8000, 6000},
						 .expected_stride = {14000},
						 .expected_scanline = {6000},
						 .expected_size = {84000000},
					 },
					 {
						 .res = {1920, 1080},
						 .expected_stride = {3360},
						 .expected_scanline = {1080},
						 .expected_size = {3628800},
					 },
					 {
						 .res = {1280, 720},
						 .expected_stride = {2240},
						 .expected_scanline = {720},
						 .expected_size = {1612800},
					 },
					 {
						 .res = {1920, 1080},
						 .stride_align = {1024},
						 .expected_stride = {4096},
						 .scanline_align = {1024},
						 .expected_scanline = {2048},
						 .size_align = {1024},
						 .expected_size = {8388608},
					 },
					 END_TEST_ARRAY,
				 }};
struct test_case raw14 = {.fmt = &vdef_raw14,
			  .tests = {
				  {
					  .res = {8000, 6000},
					  .expected_stride = {16000},
					  .expected_scanline = {6000},
					  .expected_size = {96000000},
				  },
				  {
					  .res = {1920, 1080},
					  .expected_stride = {3840},
					  .expected_scanline = {1080},
					  .expected_size = {4147200},
				  },
				  {
					  .res = {1280, 720},
					  .expected_stride = {2560},
					  .expected_scanline = {720},
					  .expected_size = {1843200},
				  },
				  {
					  .res = {1920, 1080},
					  .stride_align = {1024},
					  .expected_stride = {4096},
					  .scanline_align = {1024},
					  .expected_scanline = {2048},
					  .size_align = {1024},
					  .expected_size = {8388608},
				  },
				  END_TEST_ARRAY,
			  }};
struct test_case raw16 = {.fmt = &vdef_raw16,
			  .tests = {
				  {
					  .res = {8000, 6000},
					  .expected_stride = {16000},
					  .expected_scanline = {6000},
					  .expected_size = {96000000},
				  },
				  {
					  .res = {1920, 1080},
					  .expected_stride = {3840},
					  .expected_scanline = {1080},
					  .expected_size = {4147200},
				  },
				  {
					  .res = {1280, 720},
					  .expected_stride = {2560},
					  .expected_scanline = {720},
					  .expected_size = {1843200},
				  },
				  {
					  .res = {1920, 1080},
					  .stride_align = {1024},
					  .expected_stride = {4096},
					  .scanline_align = {1024},
					  .expected_scanline = {2048},
					  .size_align = {1024},
					  .expected_size = {8388608},
				  },
				  END_TEST_ARRAY,
			  }};
struct test_case raw16_be = {.fmt = &vdef_raw16_be,
			     .tests = {
				     {
					     .res = {8000, 6000},
					     .expected_stride = {16000},
					     .expected_scanline = {6000},
					     .expected_size = {96000000},
				     },
				     {
					     .res = {1920, 1080},
					     .expected_stride = {3840},
					     .expected_scanline = {1080},
					     .expected_size = {4147200},
				     },
				     {
					     .res = {1280, 720},
					     .expected_stride = {2560},
					     .expected_scanline = {720},
					     .expected_size = {1843200},
				     },
				     {
					     .res = {1920, 1080},
					     .stride_align = {1024},
					     .expected_stride = {4096},
					     .scanline_align = {1024},
					     .expected_scanline = {2048},
					     .size_align = {1024},
					     .expected_size = {8388608},
				     },
				     END_TEST_ARRAY,
			     }};
struct test_case raw32 = {.fmt = &vdef_raw32,
			  .tests = {
				  {
					  .res = {8000, 6000},
					  .expected_stride = {32000},
					  .expected_scanline = {6000},
					  .expected_size = {192000000},
				  },
				  {
					  .res = {1920, 1080},
					  .expected_stride = {7680},
					  .expected_scanline = {1080},
					  .expected_size = {2073600 * 4},
				  },
				  {
					  .res = {1280, 720},
					  .expected_stride = {5120},
					  .expected_scanline = {720},
					  .expected_size = {3686400},
				  },
				  {
					  .res = {1920, 1080},
					  .stride_align = {1024},
					  .expected_stride = {8192},
					  .scanline_align = {1024},
					  .expected_scanline = {2048},
					  .size_align = {1024},
					  .expected_size = {16777216},
				  },
				  END_TEST_ARRAY,
			  }};
struct test_case raw32_be = {.fmt = &vdef_raw32_be,
			     .tests = {
				     {
					     .res = {8000, 6000},
					     .expected_stride = {32000},
					     .expected_scanline = {6000},
					     .expected_size = {192000000},
				     },
				     {
					     .res = {1920, 1080},
					     .expected_stride = {7680},
					     .expected_scanline = {1080},
					     .expected_size = {2073600 * 4},
				     },
				     {
					     .res = {1280, 720},
					     .expected_stride = {5120},
					     .expected_scanline = {720},
					     .expected_size = {3686400},
				     },
				     {
					     .res = {1920, 1080},
					     .stride_align = {1024},
					     .expected_stride = {8192},
					     .scanline_align = {1024},
					     .expected_scanline = {2048},
					     .size_align = {1024},
					     .expected_size = {16777216},
				     },
				     END_TEST_ARRAY,
			     }};
struct test_case gray = {.fmt = &vdef_gray,
			 .tests = {
				 {
					 .res = {8000, 6000},
					 .expected_stride = {8000},
					 .expected_scanline = {6000},
					 .expected_size = {48000000},
				 },
				 {
					 .res = {1920, 1080},
					 .expected_stride = {1920},
					 .expected_scanline = {1080},
					 .expected_size = {2073600},
				 },
				 {
					 .res = {1280, 720},
					 .expected_stride = {1280},
					 .expected_scanline = {720},
					 .expected_size = {921600},
				 },
				 {
					 .res = {1920, 1080},
					 .stride_align = {1024},
					 .expected_stride = {2048},
					 .scanline_align = {1024},
					 .expected_scanline = {2048},
					 .size_align = {1024},
					 .expected_size = {4194304},
				 },
				 END_TEST_ARRAY,
			 }};
struct test_case gray16 = {.fmt = &vdef_gray16,
			   .tests = {
				   {
					   .res = {8000, 6000},
					   .expected_stride = {16000},
					   .expected_scanline = {6000},
					   .expected_size = {96000000},
				   },
				   {
					   .res = {1920, 1080},
					   .expected_stride = {3840},
					   .expected_scanline = {1080},
					   .expected_size = {4147200},
				   },
				   {
					   .res = {1280, 720},
					   .expected_stride = {2560},
					   .expected_scanline = {720},
					   .expected_size = {1843200},
				   },
				   {
					   .res = {1920, 1080},
					   .stride_align = {1024},
					   .expected_stride = {4096},
					   .scanline_align = {1024},
					   .expected_scanline = {2048},
					   .size_align = {1024},
					   .expected_size = {8388608},
				   },
				   END_TEST_ARRAY,
			   }};
struct test_case i420 = {
	.fmt = &vdef_i420,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {8000, 4000, 4000},
			.expected_scanline = {6000, 3000, 3000},
			.expected_size = {48000000, 12000000, 12000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {1920, 960, 960},
			.expected_scanline = {1080, 540, 540},
			.expected_size = {2073600, 518400, 518400},
		},
		{
			.res = {1280, 720},
			.expected_stride = {1280, 640, 640},
			.expected_scanline = {720, 360, 360},
			.expected_size = {921600, 230400, 230400},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512, 0},
			.expected_stride = {2048, 1024, 960},
			.scanline_align = {1024, 512, 0},
			.expected_scanline = {2048, 1024, 540},
			.size_align = {1024, 512, 0},
			.expected_size = {4194304, 1048576, 518400},
		},
		END_TEST_ARRAY,
	}};
struct test_case i420_10_16le = {
	.fmt = &vdef_i420_10_16le,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {16000, 8000, 8000},
			.expected_scanline = {6000, 3000, 3000},
			.expected_size = {96000000, 24000000, 24000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {3840, 1920, 1920},
			.expected_scanline = {1080, 540, 540},
			.expected_size = {4147200, 1036800, 1036800},
		},
		{
			.res = {1280, 720},
			.expected_stride = {2560, 1280, 1280},
			.expected_scanline = {720, 360, 360},
			.expected_size = {1843200, 460800, 460800},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {4096, 2048, 1920},
			.expected_scanline = {2048, 1024, 540},
			.expected_size = {8388608, 2097152, 1036800},
			.stride_align = {1024, 512},
			.scanline_align = {1024, 512},
			.size_align = {1024, 512},
		},
		END_TEST_ARRAY,
	}};
struct test_case i420_10_16be = {
	.fmt = &vdef_i420_10_16be,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {16000, 8000, 8000},
			.expected_scanline = {6000, 3000, 3000},
			.expected_size = {96000000, 24000000, 24000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {3840, 1920, 1920},
			.expected_scanline = {1080, 540, 540},
			.expected_size = {4147200, 1036800, 1036800},
		},
		{
			.res = {1280, 720},
			.expected_stride = {2560, 1280, 1280},
			.expected_scanline = {720, 360, 360},
			.expected_size = {1843200, 460800, 460800},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512, 0},
			.expected_stride = {4096, 2048, 1920},
			.scanline_align = {1024, 512, 0},
			.expected_scanline = {2048, 1024, 540},
			.size_align = {1024, 512, 0},
			.expected_size = {8388608, 2097152, 1036800},
		},
		END_TEST_ARRAY,
	}};
struct test_case i420_10_16le_high = {
	.fmt = &vdef_i420_10_16le_high,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {16000, 8000, 8000},
			.expected_scanline = {6000, 3000, 3000},
			.expected_size = {96000000, 24000000, 24000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {3840, 1920, 1920},
			.expected_scanline = {1080, 540, 540},
			.expected_size = {4147200, 1036800, 1036800},
		},
		{
			.res = {1280, 720},
			.expected_stride = {2560, 1280, 1280},
			.expected_scanline = {720, 360, 360},
			.expected_size = {1843200, 460800, 460800},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512, 0},
			.expected_stride = {4096, 2048, 1920},
			.scanline_align = {1024, 512, 0},
			.expected_scanline = {2048, 1024, 540},
			.size_align = {1024, 512, 0},
			.expected_size = {8388608, 2097152, 1036800},
		},
		END_TEST_ARRAY,
	}};
struct test_case i420_10_16be_high = {
	.fmt = &vdef_i420_10_16be_high,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {16000, 8000, 8000},
			.expected_scanline = {6000, 3000, 3000},
			.expected_size = {96000000, 24000000, 24000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {3840, 1920, 1920},
			.expected_scanline = {1080, 540, 540},
			.expected_size = {4147200, 1036800, 1036800},
		},
		{
			.res = {1280, 720},
			.expected_stride = {2560, 1280, 1280},
			.expected_scanline = {720, 360, 360},
			.expected_size = {1843200, 460800, 460800},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512, 0},
			.expected_stride = {4096, 2048, 1920},
			.scanline_align = {1024, 512, 0},
			.expected_scanline = {2048, 1024, 540},
			.size_align = {1024, 512, 0},
			.expected_size = {8388608, 2097152, 1036800},
		},
		END_TEST_ARRAY,
	}};
struct test_case yv12 = {
	.fmt = &vdef_yv12,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {8000, 4000, 4000},
			.expected_scanline = {6000, 3000, 3000},
			.expected_size = {48000000, 12000000, 12000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {1920, 960, 960},
			.expected_scanline = {1080, 540, 540},
			.expected_size = {2073600, 518400, 518400},
		},
		{
			.res = {1280, 720},
			.expected_stride = {1280, 640, 640},
			.expected_scanline = {720, 360, 360},
			.expected_size = {921600, 230400, 230400},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512, 0},
			.expected_stride = {2048, 1024, 960},
			.scanline_align = {1024, 512, 0},
			.expected_scanline = {2048, 1024, 540},
			.size_align = {1024, 512, 0},
			.expected_size = {4194304, 1048576, 518400},
		},
		END_TEST_ARRAY,
	}};
struct test_case yv12_10_16le = {
	.fmt = &vdef_yv12_10_16le,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {16000, 8000, 8000},
			.expected_scanline = {6000, 3000, 3000},
			.expected_size = {96000000, 24000000, 24000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {3840, 1920, 1920},
			.expected_scanline = {1080, 540, 540},
			.expected_size = {4147200, 1036800, 1036800},
		},
		{
			.res = {1280, 720},
			.expected_stride = {2560, 1280, 1280},
			.expected_scanline = {720, 360, 360},
			.expected_size = {1843200, 460800, 460800},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512, 0},
			.expected_stride = {4096, 2048, 1920},
			.scanline_align = {1024, 512, 0},
			.expected_scanline = {2048, 1024, 540},
			.size_align = {1024, 512, 0},
			.expected_size = {8388608, 2097152, 1036800},
		},
		END_TEST_ARRAY,
	}};
struct test_case yv12_10_16be = {
	.fmt = &vdef_yv12_10_16be,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {16000, 8000, 8000},
			.expected_scanline = {6000, 3000, 3000},
			.expected_size = {96000000, 24000000, 24000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {3840, 1920, 1920},
			.expected_scanline = {1080, 540, 540},
			.expected_size = {4147200, 1036800, 1036800},
		},
		{
			.res = {1280, 720},
			.expected_stride = {2560, 1280, 1280},
			.expected_scanline = {720, 360, 360},
			.expected_size = {1843200, 460800, 460800},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512, 0},
			.expected_stride = {4096, 2048, 1920},
			.scanline_align = {1024, 512, 0},
			.expected_scanline = {2048, 1024, 540},
			.size_align = {1024, 512, 0},
			.expected_size = {8388608, 2097152, 1036800},
		},
		END_TEST_ARRAY,
	}};
struct test_case yv12_10_16le_high = {
	.fmt = &vdef_yv12_10_16le_high,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {16000, 8000, 8000},
			.expected_scanline = {6000, 3000, 3000},
			.expected_size = {96000000, 24000000, 24000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {3840, 1920, 1920},
			.expected_scanline = {1080, 540, 540},
			.expected_size = {4147200, 1036800, 1036800},
		},
		{
			.res = {1280, 720},
			.expected_stride = {2560, 1280, 1280},
			.expected_scanline = {720, 360, 360},
			.expected_size = {1843200, 460800, 460800},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512, 0},
			.expected_stride = {4096, 2048, 1920},
			.scanline_align = {1024, 512, 0},
			.expected_scanline = {2048, 1024, 540},
			.size_align = {1024, 512, 0},
			.expected_size = {8388608, 2097152, 1036800},
		},
		END_TEST_ARRAY,
	}};
struct test_case yv12_10_16be_high = {
	.fmt = &vdef_yv12_10_16be_high,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {16000, 8000, 8000},
			.expected_scanline = {6000, 3000, 3000},
			.expected_size = {96000000, 24000000, 24000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {3840, 1920, 1920},
			.expected_scanline = {1080, 540, 540},
			.expected_size = {4147200, 1036800, 1036800},
		},
		{
			.res = {1280, 720},
			.expected_stride = {2560, 1280, 1280},
			.expected_scanline = {720, 360, 360},
			.expected_size = {1843200, 460800, 460800},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512, 0},
			.expected_stride = {4096, 2048, 1920},
			.scanline_align = {1024, 512, 0},
			.expected_scanline = {2048, 1024, 540},
			.size_align = {1024, 512, 0},
			.expected_size = {8388608, 2097152, 1036800},
		},
		END_TEST_ARRAY,
	}};
struct test_case nv12 = {.fmt = &vdef_nv12,
			 .tests = {
				 {
					 .res = {8000, 6000},
					 .expected_stride = {8000, 8000},
					 .expected_scanline = {6000, 3000},
					 .expected_size = {48000000, 24000000},
				 },
				 {
					 .res = {1920, 1080},
					 .expected_stride = {1920, 1920},
					 .expected_scanline = {1080, 540},
					 .expected_size = {2073600, 1036800},
				 },
				 {
					 .res = {1280, 720},
					 .expected_stride = {1280, 1280},
					 .expected_scanline = {720, 360},
					 .expected_size = {921600, 460800},
				 },
				 {
					 .res = {1920, 1080},
					 .stride_align = {1024, 512},
					 .expected_stride = {2048, 2048},
					 .scanline_align = {1024, 512},
					 .expected_scanline = {2048, 1024},
					 .size_align = {1024, 512},
					 .expected_size = {4194304, 2097152},
				 },
				 END_TEST_ARRAY,
			 }};
struct test_case nv12_10_packed = {
	.fmt = &vdef_nv12_10_packed,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {10000, 10000},
			.expected_scanline = {6000, 3000},
			.expected_size = {60000000, 30000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {2400, 2400},
			.expected_scanline = {1080, 540},
			.expected_size = {2592000, 1296000},
		},
		{
			.res = {1280, 720},
			.expected_stride = {1600, 1600},
			.expected_scanline = {720, 360},
			.expected_size = {1152000, 576000},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512},
			.expected_stride = {3072, 2560},
			.scanline_align = {1024, 512},
			.expected_scanline = {2048, 1024},
			.size_align = {1024, 512},
			.expected_size = {6291456, 2621440},
		},
		END_TEST_ARRAY,
	}};
struct test_case nv12_10_16be = {
	.fmt = &vdef_nv12_10_16be,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {16000, 16000},
			.expected_scanline = {6000, 3000},
			.expected_size = {96000000, 48000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {3840, 3840},
			.expected_scanline = {1080, 540},
			.expected_size = {4147200, 2073600},
		},
		{
			.res = {1280, 720},
			.expected_stride = {2560, 2560},
			.expected_scanline = {720, 360},
			.expected_size = {1843200, 921600},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512},
			.expected_stride = {4096, 4096},
			.scanline_align = {1024, 512},
			.expected_scanline = {2048, 1024},
			.size_align = {1024, 512},
			.expected_size = {8388608, 4194304},
		},
		END_TEST_ARRAY,
	}};
struct test_case nv12_10_16le = {
	.fmt = &vdef_nv12_10_16le,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {16000, 16000},
			.expected_scanline = {6000, 3000},
			.expected_size = {96000000, 48000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {3840, 3840},
			.expected_scanline = {1080, 540},
			.expected_size = {4147200, 2073600},
		},
		{
			.res = {1280, 720},
			.expected_stride = {2560, 2560},
			.expected_scanline = {720, 360},
			.expected_size = {1843200, 921600},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512},
			.expected_stride = {4096, 4096},
			.scanline_align = {1024, 512},
			.expected_scanline = {2048, 1024},
			.size_align = {1024, 512},
			.expected_size = {8388608, 4194304},
		},
		END_TEST_ARRAY,
	}};
struct test_case nv12_10_16be_high = {
	.fmt = &vdef_nv12_10_16be_high,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {16000, 16000},
			.expected_scanline = {6000, 3000},
			.expected_size = {96000000, 48000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {3840, 3840},
			.expected_scanline = {1080, 540},
			.expected_size = {4147200, 2073600},
		},
		{
			.res = {1280, 720},
			.expected_stride = {2560, 2560},
			.expected_scanline = {720, 360},
			.expected_size = {1843200, 921600},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512},
			.expected_stride = {4096, 4096},
			.scanline_align = {1024, 512},
			.expected_scanline = {2048, 1024},
			.size_align = {1024, 512},
			.expected_size = {8388608, 4194304},
		},
		END_TEST_ARRAY,
	}};
struct test_case nv12_10_16le_high = {
	.fmt = &vdef_nv12_10_16le_high,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {16000, 16000},
			.expected_scanline = {6000, 3000},
			.expected_size = {96000000, 48000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {3840, 3840},
			.expected_scanline = {1080, 540},
			.expected_size = {4147200, 2073600},
		},
		{
			.res = {1280, 720},
			.expected_stride = {2560, 2560},
			.expected_scanline = {720, 360},
			.expected_size = {1843200, 921600},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512},
			.expected_stride = {4096, 4096},
			.scanline_align = {1024, 512},
			.expected_scanline = {2048, 1024},
			.size_align = {1024, 512},
			.expected_size = {8388608, 4194304},
		},
		END_TEST_ARRAY,
	}};
struct test_case nv21 = {.fmt = &vdef_nv21,
			 .tests = {
				 {
					 .res = {8000, 6000},
					 .expected_stride = {8000, 8000},
					 .expected_scanline = {6000, 3000},
					 .expected_size = {48000000, 24000000},
				 },
				 {
					 .res = {1920, 1080},
					 .expected_stride = {1920, 1920},
					 .expected_scanline = {1080, 540},
					 .expected_size = {2073600, 1036800},
				 },
				 {
					 .res = {1280, 720},
					 .expected_stride = {1280, 1280},
					 .expected_scanline = {720, 360},
					 .expected_size = {921600, 460800},
				 },
				 {
					 .res = {1920, 1080},
					 .stride_align = {1024, 512},
					 .expected_stride = {2048, 2048},
					 .scanline_align = {1024, 512},
					 .expected_scanline = {2048, 1024},
					 .size_align = {1024, 512},
					 .expected_size = {4194304, 2097152},
				 },
				 END_TEST_ARRAY,
			 }};
struct test_case nv21_10_packed = {
	.fmt = &vdef_nv21_10_packed,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {10000, 10000},
			.expected_scanline = {6000, 3000},
			.expected_size = {60000000, 30000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {2400, 2400},
			.expected_scanline = {1080, 540},
			.expected_size = {2592000, 1296000},
		},
		{
			.res = {1280, 720},
			.expected_stride = {1600, 1600},
			.expected_scanline = {720, 360},
			.expected_size = {1152000, 576000},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512},
			.expected_stride = {3072, 2560},
			.scanline_align = {1024, 512},
			.expected_scanline = {2048, 1024},
			.size_align = {1024, 512},
			.expected_size = {6291456, 2621440},
		},
		END_TEST_ARRAY,
	}};
struct test_case nv21_10_16be = {
	.fmt = &vdef_nv21_10_16be,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {16000, 16000},
			.expected_scanline = {6000, 3000},
			.expected_size = {96000000, 48000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {3840, 3840},
			.expected_scanline = {1080, 540},
			.expected_size = {4147200, 2073600},
		},
		{
			.res = {1280, 720},
			.expected_stride = {2560, 2560},
			.expected_scanline = {720, 360},
			.expected_size = {1843200, 921600},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512},
			.expected_stride = {4096, 4096},
			.scanline_align = {1024, 512},
			.expected_scanline = {2048, 1024},
			.size_align = {1024, 512},
			.expected_size = {8388608, 4194304},
		},
		END_TEST_ARRAY,
	}};
struct test_case nv21_10_16le = {
	.fmt = &vdef_nv21_10_16le,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {16000, 16000},
			.expected_scanline = {6000, 3000},
			.expected_size = {96000000, 48000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {3840, 3840},
			.expected_scanline = {1080, 540},
			.expected_size = {4147200, 2073600},
		},
		{
			.res = {1280, 720},
			.expected_stride = {2560, 2560},
			.expected_scanline = {720, 360},
			.expected_size = {1843200, 921600},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512},
			.expected_stride = {4096, 4096},
			.scanline_align = {1024, 512},
			.expected_scanline = {2048, 1024},
			.size_align = {1024, 512},
			.expected_size = {8388608, 4194304},
		},
		END_TEST_ARRAY,
	}};
struct test_case nv21_10_16be_high = {
	.fmt = &vdef_nv21_10_16be_high,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {16000, 16000},
			.expected_scanline = {6000, 3000},
			.expected_size = {96000000, 48000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {3840, 3840},
			.expected_scanline = {1080, 540},
			.expected_size = {4147200, 2073600},
		},
		{
			.res = {1280, 720},
			.expected_stride = {2560, 2560},
			.expected_scanline = {720, 360},
			.expected_size = {1843200, 921600},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512},
			.expected_stride = {4096, 4096},
			.scanline_align = {1024, 512},
			.expected_scanline = {2048, 1024},
			.size_align = {1024, 512},
			.expected_size = {8388608, 4194304},
		},
		END_TEST_ARRAY,
	}};
struct test_case nv21_10_16le_high = {
	.fmt = &vdef_nv21_10_16le_high,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {16000, 16000},
			.expected_scanline = {6000, 3000},
			.expected_size = {96000000, 48000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {3840, 3840},
			.expected_scanline = {1080, 540},
			.expected_size = {4147200, 2073600},
		},
		{
			.res = {1280, 720},
			.expected_stride = {2560, 2560},
			.expected_scanline = {720, 360},
			.expected_size = {1843200, 921600},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512},
			.expected_stride = {4096, 4096},
			.scanline_align = {1024, 512},
			.expected_scanline = {2048, 1024},
			.size_align = {1024, 512},
			.expected_size = {8388608, 4194304},
		},
		END_TEST_ARRAY,
	}};
struct test_case i444 = {
	.fmt = &vdef_i444,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {8000, 8000, 8000},
			.expected_scanline = {6000, 6000, 6000},
			.expected_size = {48000000, 48000000, 48000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {1920, 1920, 1920},
			.expected_scanline = {1080, 1080, 1080},
			.expected_size = {2073600, 2073600, 2073600},
		},
		{
			.res = {1280, 720},
			.expected_stride = {1280, 1280, 1280},
			.expected_scanline = {720, 720, 720},
			.expected_size = {921600, 921600, 921600},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512, 0},
			.expected_stride = {2048, 2048, 1920},
			.scanline_align = {1024, 512, 0},
			.expected_scanline = {2048, 1536, 1080},
			.size_align = {1024, 512, 0},
			.expected_size = {4194304, 3145728, 2073600},
		},
		END_TEST_ARRAY,
	}};
struct test_case i444_semi = {
	.fmt = &vdef_i444_semi,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {8000, 16000},
			.expected_scanline = {6000, 6000},
			.expected_size = {48000000, 96000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {1920, 3840},
			.expected_scanline = {1080, 1080},
			.expected_size = {2073600, 4147200},
		},
		{
			.res = {1280, 720},
			.expected_stride = {1280, 1280 * 2},
			.expected_scanline = {720, 720},
			.expected_size = {921600, 921600 * 2},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512},
			.expected_stride = {2048, 4096},
			.scanline_align = {1024, 512},
			.expected_scanline = {2048, 1536},
			.size_align = {1024, 512},
			.expected_size = {4194304, 6291456},
		},
		END_TEST_ARRAY,
	}};
struct test_case rgb = {.fmt = &vdef_rgb,
			.tests = {
				{
					.res = {8000, 6000},
					.expected_stride = {24000},
					.expected_scanline = {6000},
					.expected_size = {144000000},
				},
				{
					.res = {1920, 1080},
					.expected_stride = {5760},
					.expected_scanline = {1080},
					.expected_size = {6220800},
				},
				{
					.res = {1280, 720},
					.expected_stride = {3840},
					.expected_scanline = {720},
					.expected_size = {2764800},
				},
				{
					.res = {1920, 1080},
					.stride_align = {1024},
					.expected_stride = {6144},
					.scanline_align = {1024},
					.expected_scanline = {2048},
					.size_align = {1024},
					.expected_size = {12582912},
				},
				END_TEST_ARRAY,
			}};
struct test_case rgb_planar = {
	.fmt = &vdef_rgb_planar,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {8000, 8000, 8000},
			.expected_scanline = {6000, 6000, 6000},
			.expected_size = {48000000, 48000000, 48000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {1920, 1920, 1920},
			.expected_scanline = {1080, 1080, 1080},
			.expected_size = {2073600, 2073600, 2073600},
		},
		{
			.res = {1280, 720},
			.expected_stride = {1280, 1280, 1280},
			.expected_scanline = {720, 720, 720},
			.expected_size = {921600, 921600, 921600},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512, 0},
			.expected_stride = {2048, 2048, 1920},
			.scanline_align = {1024, 512, 0},
			.expected_scanline = {2048, 1536, 1080},
			.size_align = {1024, 512, 0},
			.expected_size = {4194304, 3145728, 2073600},
		},
		END_TEST_ARRAY,
	}};
struct test_case bgr = {.fmt = &vdef_bgr,
			.tests = {
				{
					.res = {8000, 6000},
					.expected_stride = {24000},
					.expected_scanline = {6000},
					.expected_size = {144000000},
				},
				{
					.res = {1920, 1080},
					.expected_stride = {5760},
					.expected_scanline = {1080},
					.expected_size = {6220800},
				},
				{
					.res = {1280, 720},
					.expected_stride = {3840},
					.expected_scanline = {720},
					.expected_size = {2764800},
				},
				{
					.res = {1920, 1080},
					.stride_align = {1024},
					.expected_stride = {6144},
					.scanline_align = {1024},
					.expected_scanline = {2048},
					.size_align = {1024},
					.expected_size = {12582912},
				},
				END_TEST_ARRAY,
			}};
struct test_case rgba = {.fmt = &vdef_rgba,
			 .tests = {
				 {
					 .res = {8000, 6000},
					 .expected_stride = {32000},
					 .expected_scanline = {6000},
					 .expected_size = {192000000},
				 },
				 {
					 .res = {1920, 1080},
					 .expected_stride = {7680},
					 .expected_scanline = {1080},
					 .expected_size = {8294400},
				 },
				 {
					 .res = {1280, 720},
					 .expected_stride = {5120},
					 .expected_scanline = {720},
					 .expected_size = {3686400},
				 },
				 {
					 .res = {1920, 1080},
					 .stride_align = {1024},
					 .expected_stride = {8192},
					 .scanline_align = {1024},
					 .expected_scanline = {2048},
					 .size_align = {1024},
					 .expected_size = {16777216},
				 },
				 END_TEST_ARRAY,
			 }};
struct test_case rgba_planar = {
	.fmt = &vdef_rgba_planar,
	.tests = {
		{
			.res = {8000, 6000},
			.expected_stride = {8000, 8000, 8000, 8000},
			.expected_scanline = {6000, 6000, 6000, 6000},
			.expected_size =
				{48000000, 48000000, 48000000, 48000000},
		},
		{
			.res = {1920, 1080},
			.expected_stride = {1920, 1920, 1920, 1920},
			.expected_scanline = {1080, 1080, 1080, 1080},
			.expected_size = {2073600, 2073600, 2073600, 2073600},
		},
		{
			.res = {1280, 720},
			.expected_stride = {1280, 1280, 1280, 1280},
			.expected_scanline = {720, 720, 720, 720},
			.expected_size = {921600, 921600, 921600, 921600},
		},
		{
			.res = {1920, 1080},
			.stride_align = {1024, 512, 512},
			.expected_stride = {2048, 2048, 2048, 1920},
			.scanline_align = {1024, 512, 512},
			.expected_scanline = {2048, 1536, 1536, 1080},
			.size_align = {1024, 512, 512},
			.expected_size = {4194304, 3145728, 3145728, 2073600},
		},
		END_TEST_ARRAY,
	}};
struct test_case bgra = {.fmt = &vdef_bgra,
			 .tests = {
				 {
					 .res = {8000, 6000},
					 .expected_stride = {32000},
					 .expected_scanline = {6000},
					 .expected_size = {192000000},
				 },
				 {
					 .res = {1920, 1080},
					 .expected_stride = {7680},
					 .expected_scanline = {1080},
					 .expected_size = {8294400},
				 },
				 {
					 .res = {1280, 720},
					 .expected_stride = {5120},
					 .expected_scanline = {720},
					 .expected_size = {3686400},
				 },
				 {
					 .res = {1920, 1080},
					 .stride_align = {1024},
					 .expected_stride = {8192},
					 .scanline_align = {1024},
					 .expected_scanline = {2048},
					 .size_align = {1024},
					 .expected_size = {16777216},
				 },
				 END_TEST_ARRAY,
			 }};
struct test_case abgr = {.fmt = &vdef_abgr,
			 .tests = {
				 {
					 .res = {8000, 6000},
					 .expected_stride = {32000},
					 .expected_scanline = {6000},
					 .expected_size = {192000000},
				 },
				 {
					 .res = {1920, 1080},
					 .expected_stride = {7680},
					 .expected_scanline = {1080},
					 .expected_size = {8294400},
				 },
				 {
					 .res = {1280, 720},
					 .expected_stride = {5120},
					 .expected_scanline = {720},
					 .expected_size = {3686400},
				 },
				 {
					 .res = {1920, 1080},
					 .stride_align = {1024},
					 .expected_stride = {8192},
					 .scanline_align = {1024},
					 .expected_scanline = {2048},
					 .size_align = {1024},
					 .expected_size = {16777216},
				 },
				 END_TEST_ARRAY,
			 }};


struct test_case *all_tests[] = {
	/* RAW formats */
	&raw8,
	&raw10_packed,
	&raw10,
	&raw12_packed,
	&raw12,
	&raw14_packed,
	&raw14,
	&raw16,
	&raw16_be,
	&raw32,
	&raw32_be,
	/* GRAY formats */
	&gray,
	&gray16,
	/* YUV420 planar formats */
	&i420,
	&i420_10_16le,
	&i420_10_16be,
	&i420_10_16le_high,
	&i420_10_16be_high,
	&yv12,
	&yv12_10_16le,
	&yv12_10_16be,
	&yv12_10_16le_high,
	&yv12_10_16be_high,
	/* YUV420 semi-planar formats */
	&nv12,
	&nv12_10_packed,
	&nv12_10_16be,
	&nv12_10_16le,
	&nv12_10_16be_high,
	&nv12_10_16le_high,
	&nv21,
	&nv21_10_packed,
	&nv21_10_16le,
	&nv21_10_16be,
	&nv21_10_16le_high,
	&nv21_10_16be_high,
	/* YUV444 planar formats */
	&i444,
	&i444_semi,
	/* RGB24 formats */
	&rgb,
	&rgb_planar,
	&bgr,
	/* RGBA32 formats */
	&rgba,
	&rgba_planar,
	&bgra,
	&abgr,
	NULL,
};


static void test_calc_raw_frame_size(void)
{
	int idx = 0;
	struct test_case *t;
	while ((t = all_tests[idx++]) != NULL) {
		run_test_case(t);
	}
}


static void test_calc_raw_contiguous_frame_size(void)
{
	char *str = vdef_raw_format_to_str(i420.fmt);
	printf("Testing %s\n", str);
	free(str);
	int idx = 0;
	struct single_test *st;
	while (true) {
		st = &(i420.tests[idx++]);
		if (st->res.width == 0)
			break;
		run_test_contiguous(i420.fmt, st);
	}
}

CU_TestInfo g_vdef_test_calc[] = {
	{FN("vdef-calc-raw-contiguous-frame-size"),
	 &test_calc_raw_contiguous_frame_size},
	{FN("calc-frame-size"), &test_calc_raw_frame_size},

	CU_TEST_INFO_NULL,
};
