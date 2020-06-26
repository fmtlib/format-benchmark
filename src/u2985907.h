#include <stdint.h>

// The integer to string conversion method by StackOverflow user
// https://stackoverflow.com/users/2985907/user2985907 sometimes incorrectly
// attributed to jiaendu: https://stackoverflow.com/a/19944488/471164
// Includes fixes from
// https://gist.github.com/cpei-avalara/8aedf14f5618852be2cff4de267d497c.
namespace so_u2985907 {

   inline int ufast_utoa10(unsigned int value, char* str)
   {
      #define JOIN(N)                                                      \
      N "0", N "1", N "2", N "3", N "4", N "5", N "6", N "7", N "8", N "9" \

      #define JOIN2(N)                                                     \
      JOIN(N "0"), JOIN(N "1"), JOIN(N "2"), JOIN(N "3"), JOIN(N "4"),     \
      JOIN(N "5"), JOIN(N "6"), JOIN(N "7"), JOIN(N "8"), JOIN(N "9")      \

      #define JOIN3(N)                                                      \
      JOIN2(N "0"), JOIN2(N "1"), JOIN2(N "2"), JOIN2(N "3"), JOIN2(N "4"), \
      JOIN2(N "5"), JOIN2(N "6"), JOIN2(N "7"), JOIN2(N "8"), JOIN2(N "9")  \

      #define JOIN4                                               \
      JOIN3("0"), JOIN3("1"), JOIN3("2"), JOIN3("3"), JOIN3("4"), \
      JOIN3("5"), JOIN3("6"), JOIN3("7"), JOIN3("8"), JOIN3("9")  \

      #define JOIN5(N)                                                \
      JOIN(N), JOIN(N "1"), JOIN(N "2"), JOIN(N "3"), JOIN(N "4"),    \
      JOIN(N "5"), JOIN(N "6"), JOIN(N "7"), JOIN(N "8"), JOIN(N "9") \

      #define JOIN6                                              \
      JOIN5(""), JOIN2("1"), JOIN2("2"), JOIN2("3"), JOIN2("4"), \
      JOIN2("5"), JOIN2("6"), JOIN2("7"), JOIN2("8"), JOIN2("9") \

      #define F(N)  ((N) >= 100 ? 3 : (N) >= 10 ? 2 : 1)

      #define F10(N)                                   \
      F(N), F(N + 1), F(N + 2), F(N + 3), F(N + 4),    \
      F(N + 5), F(N + 6), F(N + 7), F(N + 8), F(N + 9) \

      #define F100(N)                                    \
      F10(N), F10(N + 10), F10(N + 20), F10(N + 30),     \
      F10(N + 40), F10(N + 50), F10(N + 60), F10(N + 70),\
      F10(N + 80), F10(N + 90)                           \

      static const short offsets[] = {
                                       F100(  0), F100(100), F100(200), F100(300), F100(400),
                                       F100(500), F100(600), F100(700), F100(800), F100(900)
                                     };

      static const char table1[][4] = { JOIN ("") };
      static const char table2[][4] = { JOIN2("") };
      static const char table3[][4] = { JOIN3("") };
      static const char table4[][8] = { JOIN4 };
      static const char table5[][4] = { JOIN6 };

      #undef JOIN
      #undef JOIN2
      #undef JOIN3
      #undef JOIN4
      #undef F
      #undef F10
      #undef F100

      char *wstr;
      #if (_WIN64 || __x86_64__ || __ppc64__)
         uint64_t remains[2];
      #else
         uint32_t remains[2];
      #endif
      unsigned int v2;

      if (value >= 100000000)
      {
         #if (_WIN64 || __x86_64__ || __ppc64__)
            remains[0] = (((uint64_t)value * (uint64_t)3518437209) >> 45);
            remains[1] = (((uint64_t)value * (uint64_t)2882303762) >> 58);
         #else
            remains[0] = value / 10000;
            remains[1] = value / 100000000;
         #endif
         v2 = remains[1];
         remains[1] = remains[0] - remains[1] * 10000;
         remains[0] = value - remains[0] * 10000;
         if (v2 >= 10)
         {
            memcpy(str,table5[v2],2);
            str += 2;
            memcpy(str,table4[remains[1]],4);
            str += 4;
            memcpy(str,table4[remains[0]],4);
            return 10;
         }
         else
         {
            *(char *) str = v2 + '0';
            str += 1;
            memcpy(str,table4[remains[1]],4);
            str += 4;
            memcpy(str,table4[remains[0]],4);
            return 9;
         }
      }
      else if (value >= 10000)
      {
         #if (_WIN64 || __x86_64__ || __ppc64__)
            v2 = (((uint64_t)value * (uint64_t)3518437209 ) >> 45);
         #else
            v2 = value / 10000;
         #endif
         remains[0] = value - v2 * 10000;
         if (v2 >= 1000)
         {
            memcpy(str,table4[v2],4);
            str += 4;
            memcpy(str,table4[remains[0]],4);
            return 8;
         }
         else
         {
            wstr = str;
            memcpy(wstr,table5[v2],4);
            wstr += offsets[v2];
            memcpy(wstr,table4[remains[0]],4);
            wstr += 4;
            return (wstr - str);
         }
      }
      else
      {
         if (value >= 1000)
         {
            memcpy(str,table4[value],4);
            return 4;
         }
         else if (value >= 100)
         {
            memcpy(str,table3[value],3);
            return 3;
         }
         else if (value >= 10)
         {
            memcpy(str,table2[value],2);
            return 2;
         }
         else
         {
            *(char *) str = *(char *) table1[value];
            return 1;
         }
      }
   }

int ufast_itoa10(int value, char* str) {
  if (value < 0) { *(str++) = '-'; 
    return ufast_utoa10(-value, str) + 1; 
  }
  else return ufast_utoa10(value, str);
}
}  // namespace so_u2985907