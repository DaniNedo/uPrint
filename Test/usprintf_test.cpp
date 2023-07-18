#include "CppUTest/TestHarness.h"

#include "uprint.h"

#define BUFFER_SIZE 100

TEST_GROUP(SprintfTestGroup)
{
   int length = 0;
   char buffer[BUFFER_SIZE] = {1}; // Fill with stuff to test the null terminator

   int test_usprintf(char* test_buffer, const char* test_format, ...) {
      va_list test_va;
      va_start(test_va, test_format);
      const int test_ret = uvsnprintf(test_buffer, test_format, test_va);
      va_end(test_va);
      return test_ret;
   }
};

TEST(SprintfTestGroup, PrintVaList)
{
   length = test_usprintf(buffer, "Hello, world!");
   STRCMP_EQUAL("Hello, world!", buffer);
   LONGS_EQUAL(13, length);
}

TEST(SprintfTestGroup, PrintSimpleText)
{
   length = usprintf(buffer, "Hello, world!");
   STRCMP_EQUAL("Hello, world!", buffer);
   LONGS_EQUAL(13, length);
}

TEST(SprintfTestGroup, PrintChar)
{
   char c = 'A';
   length = usprintf(buffer, "%c", c);
   STRCMP_EQUAL("A", buffer);
   LONGS_EQUAL(1, length);
}

TEST(SprintfTestGroup, PrintString)
{
   char str[] = "Hello, world!";
   length = usprintf(buffer, "%s", str);
   STRCMP_EQUAL(str, buffer);
   LONGS_EQUAL(13, length);
}

TEST(SprintfTestGroup, PrintUnsignedInt)
{
   unsigned int zero = 0;
   length = usprintf(buffer, "%u", zero);
   STRCMP_EQUAL("0", buffer);
   LONGS_EQUAL(1, length);

   unsigned int pos = 1230;
   length = usprintf(buffer, "%u", pos);
   STRCMP_EQUAL("1230", buffer);
   LONGS_EQUAL(4, length);

   unsigned int uint_max = UINT_MAX;
   length = usprintf(buffer, "%u", uint_max);
   STRCMP_EQUAL("4294967295", buffer);
   LONGS_EQUAL(10, length);
}

TEST(SprintfTestGroup, PrintInt)
{
   int pos = 1234;
   length = usprintf(buffer, "%d", pos);
   STRCMP_EQUAL("1234", buffer);
   LONGS_EQUAL(4, length);

   int neg = -1234;
   length = usprintf(buffer, "%d", neg);
   STRCMP_EQUAL("-1234", buffer);
   LONGS_EQUAL(5, length);

   int int_max = INT_MAX;
   length = usprintf(buffer, "%d", int_max);
   STRCMP_EQUAL("2147483647", buffer);
   LONGS_EQUAL(10, length);

   int int_min = INT_MIN;
   length = usprintf(buffer, "%d", int_min);
   STRCMP_EQUAL("-2147483648", buffer);
   LONGS_EQUAL(11, length);
}

TEST(SprintfTestGroup, PrintHex)
{
   unsigned num = 0xDEADBEEF;
   length = usprintf(buffer, "%X", num);
   STRCMP_EQUAL("DEADBEEF", buffer);
   LONGS_EQUAL(8, length);

   length = usprintf(buffer, "%x", num);
   STRCMP_EQUAL("deadbeef", buffer);
   LONGS_EQUAL(8, length);   
}

TEST(SprintfTestGroup, PrintFloat)
{
   float pos = 3.14f;
   length = usprintf(buffer, "%f", pos);
   STRCMP_EQUAL("3.140000", buffer);
   LONGS_EQUAL(8, length);

   float neg = -1.234f;
   length = usprintf(buffer, "%f", neg);
   STRCMP_EQUAL("-1.234000", buffer);
   LONGS_EQUAL(9, length);

   float pre = 1.2345678f;
   length = usprintf(buffer, "%.7f", pre); // More than 7 decimals starts to produce errors
   STRCMP_EQUAL("1.2345678", buffer);
   LONGS_EQUAL(9, length);

   length = usprintf(buffer, "%.4f", pre);
   STRCMP_EQUAL("1.2345", buffer); // Need to fix the precision round up
   LONGS_EQUAL(6, length);
}
