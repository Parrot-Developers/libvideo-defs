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


static CU_SuiteInfo s_suites[] = {
	{FN("csv"), NULL, NULL, g_vdef_test_csv},
	{FN("calc"), NULL, NULL, g_vdef_test_calc},

	CU_SUITE_INFO_NULL,
};


static void run_automated()
{
	CU_automated_run_tests();
	CU_list_tests_to_file();
}


static void run_basic()
{
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
}


int main()
{
	const char *filename;

	CU_initialize_registry();
	CU_register_suites(s_suites);

	/* Set filename */
	filename = getenv("CUNIT_OUT_NAME");
	CU_set_output_filename(filename);

	/* Run tests */
	if (getenv("CUNIT_AUTOMATED") != NULL)
		run_automated();
	else
		run_basic();

	CU_cleanup_registry();
}
