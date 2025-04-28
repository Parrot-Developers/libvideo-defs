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


static void test_framerate_from_str(void)
{
	enum vdef_framerate rate;

	rate = vdef_framerate_from_str(NULL);
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_UNKNOWN);

	rate = vdef_framerate_from_str("");
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_UNKNOWN);

	rate = vdef_framerate_from_str("invalid_value");
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_UNKNOWN);

	rate = vdef_framerate_from_str("30_");
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_UNKNOWN);

	rate = vdef_framerate_from_str("30fps");
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_UNKNOWN);

	rate = vdef_framerate_from_str("30FPS");
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_UNKNOWN);

	rate = vdef_framerate_from_str("0");
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_UNKNOWN);

	rate = vdef_framerate_from_str("30");
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_30);

	rate = vdef_framerate_from_str("24000/1001");
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_24);

	rate = vdef_framerate_from_str("30/1");
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_30_1);

	rate = vdef_framerate_from_str("50/1");
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_50);

	rate = vdef_framerate_from_str("120000/1001");
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_120);
}


static void test_framerate_to_str(void)
{
	const char *rate;

	rate = vdef_framerate_to_str(VDEF_FRAMERATE_UNKNOWN);
	CU_ASSERT_STRING_EQUAL(rate, "UNKNOWN");

	rate = vdef_framerate_to_str(VDEF_FRAMERATE_MAX);
	CU_ASSERT_STRING_EQUAL(rate, "UNKNOWN");

	rate = vdef_framerate_to_str(VDEF_FRAMERATE_48);
	CU_ASSERT_STRING_EQUAL(rate, "48");

	rate = vdef_framerate_to_str(VDEF_FRAMERATE_30_1);
	CU_ASSERT_STRING_EQUAL(rate, "30/1");

	rate = vdef_framerate_to_str(VDEF_FRAMERATE_120);
	CU_ASSERT_STRING_EQUAL(rate, "120");

	rate = vdef_framerate_to_str(VDEF_FRAMERATE_25);
	CU_ASSERT_STRING_EQUAL(rate, "25");

	rate = vdef_framerate_to_str(VDEF_FRAMERATE_60_7);
	CU_ASSERT_STRING_EQUAL(rate, "60/7");
}


static void test_framerate_from_frac(void)
{
	enum vdef_framerate rate;
	struct vdef_frac frac = {0};

	rate = vdef_framerate_from_frac(NULL);
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_UNKNOWN);

	rate = vdef_framerate_from_frac(&frac);
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_UNKNOWN);

	/* Invalid resolutions */
	frac.num = 60;
	frac.den = 0;
	rate = vdef_framerate_from_frac(&frac);
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_UNKNOWN);

	frac.num = 0;
	frac.den = 1001;
	rate = vdef_framerate_from_frac(&frac);
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_UNKNOWN);

	/* Unknown resolution */
	frac.num = 60;
	frac.den = 1;
	rate = vdef_framerate_from_frac(&frac);
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_UNKNOWN);

	/* Known resolutions */
	frac.num = 30000;
	frac.den = 1001;
	rate = vdef_framerate_from_frac(&frac);
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_30);

	frac.num = 30;
	frac.den = 1;
	rate = vdef_framerate_from_frac(&frac);
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_30_1);

	frac.num = 60000;
	frac.den = 1001;
	rate = vdef_framerate_from_frac(&frac);
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_60);

	frac.num = 25;
	frac.den = 1;
	rate = vdef_framerate_from_frac(&frac);
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_25);

	/* Special resolution */
	frac.num = 60;
	frac.den = 7;
	rate = vdef_framerate_from_frac(&frac);
	CU_ASSERT_EQUAL(rate, VDEF_FRAMERATE_60_7);
}


static void test_framerate_to_frac(void)
{
	int ret;
	struct vdef_frac frac;

	ret = vdef_framerate_to_frac(VDEF_FRAMERATE_UNKNOWN, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);

	ret = vdef_framerate_to_frac(VDEF_FRAMERATE_MAX, &frac);
	CU_ASSERT_EQUAL(ret, -ENOENT);

	ret = vdef_framerate_to_frac(VDEF_FRAMERATE_30, &frac);
	CU_ASSERT_EQUAL(ret, 0);
	CU_ASSERT_TRUE(frac.num == 30000 && frac.den == 1001);

	ret = vdef_framerate_to_frac(VDEF_FRAMERATE_60_7, &frac);
	CU_ASSERT_EQUAL(ret, 0);
	CU_ASSERT_TRUE(frac.num == 60 && frac.den == 7);

	ret = vdef_framerate_to_frac(VDEF_FRAMERATE_30_1, &frac);
	CU_ASSERT_EQUAL(ret, 0);
	CU_ASSERT_TRUE(frac.num == 30 && frac.den == 1);

	ret = vdef_framerate_to_frac(VDEF_FRAMERATE_96, &frac);
	CU_ASSERT_EQUAL(ret, 0);
	CU_ASSERT_TRUE(frac.num == 96000 && frac.den == 1001);
}


CU_TestInfo g_vdef_test_framerate[] = {
	{FN("framerate-from-str"), &test_framerate_from_str},
	{FN("framerate-to-str"), &test_framerate_to_str},
	{FN("framerate-from-frac"), &test_framerate_from_frac},
	{FN("framerate-to-frac"), &test_framerate_to_frac},

	CU_TEST_INFO_NULL,
};
