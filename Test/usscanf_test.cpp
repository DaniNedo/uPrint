#include "CppUTest/TestHarness.h"

#include "uprint.h"

#define BUFFER_SIZE 100

TEST_GROUP(SscanfTestGroup)
{
   int items = 0;
};

TEST(SscanfTestGroup, ScanChar)
{
   char c = '\0';
   items = usscanf("H","%c", &c);
   CHECK_EQUAL('H', c);
   LONGS_EQUAL(1, items);
}

TEST(SscanfTestGroup, ScanString)
{
   char s[] = "Hello!";
   char b[20] = {0};
   items = usscanf(s,"%s", &b);
   STRCMP_EQUAL(s, b);
   LONGS_EQUAL(1, items);
}

TEST(SscanfTestGroup, ScanUnsignedInt)
{
   char str[] = "1234";
   unsigned int n = 0;
   items = usscanf(str,"%u", &n);
   LONGS_EQUAL(1234, n);
   LONGS_EQUAL(1, items);

   char str_leading_space[] = "    1234";
   n = 0;
   items = usscanf(str_leading_space,"%u", &n);
   LONGS_EQUAL(1234, n);
   LONGS_EQUAL(1, items);

   char str_trailing_space[] = "1234    ";
   n = 0;
   items = usscanf(str_trailing_space,"%u", &n);
   LONGS_EQUAL(1234, n);
   LONGS_EQUAL(1, items);

   char str_with_chars[] = "acb1234def";
   n = 0;
   items = usscanf(str_with_chars,"%u", &n);
   LONGS_EQUAL(1234, n);
   LONGS_EQUAL(1, items);
}

TEST(SscanfTestGroup, ScanLongUnsignedInt)
{
   char str[] = "1234";
   long unsigned int n = 0;
   items = usscanf(str,"%lu", &n);
   LONGS_EQUAL(1234, n);
   LONGS_EQUAL(1, items);
}

TEST(SscanfTestGroup, ScanLongLongUnsignedInt)
{
   char str[] = "1234";
   long long unsigned int n = 0;
   items = usscanf(str,"%llu", &n);
   LONGS_EQUAL(1234, n);
   LONGS_EQUAL(1, items);
}

TEST(SscanfTestGroup, ScanHex)
{
   char str[] = "1234";
   unsigned int n = 0;
   items = usscanf(str,"%x", &n);
   LONGS_EQUAL(0x1234, n);
   LONGS_EQUAL(1, items);
}
