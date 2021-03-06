/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio_ext.h>

#include <gtest/gtest.h>

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wchar.h>
#include <locale.h>

#include "TemporaryFile.h"

TEST(stdio_ext, __fbufsize) {
  FILE* fp = fopen("/proc/version", "r");

  char buf[128];

  ASSERT_EQ(0, setvbuf(fp, buf, _IOFBF, 1));
  ASSERT_EQ(1U, __fbufsize(fp));

  ASSERT_EQ(0, setvbuf(fp, buf, _IOFBF, 8));
  ASSERT_EQ(8U, __fbufsize(fp));

  fclose(fp);
}

TEST(stdio_ext, __flbf) {
  FILE* fp = fopen("/proc/version", "r");

  ASSERT_FALSE(__flbf(fp));

  char buf[128];
  ASSERT_EQ(0, setvbuf(fp, buf, _IOLBF, sizeof(buf)));

  ASSERT_TRUE(__flbf(fp));

  fclose(fp);
}

TEST(stdio_ext, __fpending) {
  FILE* fp = fopen("/dev/null", "w");
  ASSERT_EQ(0U, __fpending(fp));
  ASSERT_EQ('x', fputc('x', fp));
  ASSERT_EQ(1U, __fpending(fp));
  ASSERT_EQ('y', fputc('y', fp));
  ASSERT_EQ(2U, __fpending(fp));
  fflush(fp);
  ASSERT_EQ(0U, __fpending(fp));
  fclose(fp);
}

TEST(stdio_ext, __fpurge) {
  FILE* fp = tmpfile();

  ASSERT_EQ('a', fputc('a', fp));
  ASSERT_EQ(1U, __fpending(fp));
  __fpurge(fp);
  ASSERT_EQ(0U, __fpending(fp));

  ASSERT_EQ('b', fputc('b', fp));
  ASSERT_EQ('\n', fputc('\n', fp));
  ASSERT_EQ(2U, __fpending(fp));

  rewind(fp);

  char buf[16];
  char* s = fgets(buf, sizeof(buf), fp);
  ASSERT_TRUE(s != NULL);
  ASSERT_STREQ("b\n", s);

  fclose(fp);
}

TEST(stdio_ext, _flushlbf) {
  FILE* fp = fopen("/dev/null", "w");

  char buf[128];
  ASSERT_EQ(0, setvbuf(fp, buf, _IOLBF, sizeof(buf)));

  ASSERT_EQ('a', fputc('a', fp));
  ASSERT_EQ(1U, __fpending(fp));

  _flushlbf();

  ASSERT_EQ(0U, __fpending(fp));

  fclose(fp);
}

TEST(stdio_ext, __freadable__fwritable) {
  FILE* fp = fopen("/dev/null", "r");
  ASSERT_TRUE(__freadable(fp));
  ASSERT_FALSE(__fwritable(fp));
  fclose(fp);

  fp = fopen("/dev/null", "w");
  ASSERT_FALSE(__freadable(fp));
  ASSERT_TRUE(__fwritable(fp));
  fclose(fp);

  fp = fopen("/dev/null", "w+");
  ASSERT_TRUE(__freadable(fp));
  ASSERT_TRUE(__fwritable(fp));
  fclose(fp);
}

TEST(stdio_ext, __fsetlocking) {
  FILE* fp = fopen("/proc/version", "r");
  // Android doesn't actually support the other modes.
  ASSERT_EQ(FSETLOCKING_INTERNAL, __fsetlocking(fp, FSETLOCKING_QUERY));
  fclose(fp);
}
