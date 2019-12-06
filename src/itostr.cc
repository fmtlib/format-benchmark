/* Copyright Richard Benjamin Voigt */
/* All rights reserved.             */
/* contact richardvoigt@gmail.com for a cleaned-up version under a BSD license
 */

#include <climits>
#include <string>

template <typename T> struct assert_integral {
  enum { value = (T)0.5 };
  char test[1 - 4 * value];
};

template <typename T, bool is_signed, size_t bits> struct itostr_impl {};

template <typename T, bool is_signed> struct itostr_impl<T, is_signed, 8> {
  static std::string cvt(T val) {
    std::string retval(5, '\0');
    int i = 0;
    char ch = 0;

    if (is_signed) {
      if (val < 0) {
        retval[i] = '-';
        ++i;
        if (val <= -100) {
          ch = '1';
          val += 100;
        }
        val = -val;
      } else if (val >= 100) {
        ch |= '1';
        val -= 100;
      }
    } else {
      if (val >= 200) {
        ch |= '2';
        val -= 200;
      } else if (val >= 100) {
        ch |= '1';
        val -= 100;
      }
    }
    if (ch) {
      retval[i] = ch;
      ++i;
      ch = '0';
    }

    if (val >= 80) {
      ch |= '8';
      val -= 80;
    } else if (val >= 40) {
      ch |= '4';
      val -= 40;
    }
    if (val >= 20) {
      ch |= '2';
      val -= 20;
    }
    if (val >= 10) {
      ch |= '1';
      val -= 10;
    }
    if (ch) {
      retval[i] = ch;
      ++i;
    }

    retval[i] = '0' + val;
    retval.resize(i + 1);

    return retval;
  }
};

template <typename T, bool is_signed> struct itostr_impl<T, is_signed, 16> {
  static std::string cvt(T val) {
    std::string retval(7, '\0');
    int i = 0;
    char ch = 0;

    if (is_signed) {
      if (val < 0) {
        retval[i] = '-';
        ++i;
        if (val <= -20000) {
          ch = '2';
          val += 20000;
        }
        val = -val;
      } else if (val >= 20000) {
        ch |= '2';
        val -= 20000;
      }
    } else {
      if (val >= 40000) {
        ch |= '4';
        val -= 40000;
      } else if (val >= 20000) {
        ch |= '2';
        val -= 20000;
      }
    }
    if (val >= 10000) {
      ch |= '1';
      val -= 10000;
    }

    if (ch) {
      retval[i] = ch;
      ++i;
      ch = '0';
    }

    if (val >= 8000) {
      ch |= '8';
      val -= 8000;
    } else if (val >= 4000) {
      ch |= '4';
      val -= 4000;
    }
    if (val >= 2000) {
      ch |= '2';
      val -= 2000;
    }
    if (val >= 1000) {
      ch |= '1';
      val -= 1000;
    }
    if (ch) {
      retval[i] = ch;
      ++i;
      ch = '0';
    }

    if (val >= 800) {
      ch |= '8';
      val -= 800;
    } else if (val >= 400) {
      ch |= '4';
      val -= 400;
    }
    if (val >= 200) {
      ch |= '2';
      val -= 200;
    }
    if (val >= 100) {
      ch |= '1';
      val -= 100;
    }
    if (ch) {
      retval[i] = ch;
      ++i;
      ch = '0';
    }

    if (val >= 80) {
      ch |= '8';
      val -= 80;
    } else if (val >= 40) {
      ch |= '4';
      val -= 40;
    }
    if (val >= 20) {
      ch |= '2';
      val -= 20;
    }
    if (val >= 10) {
      ch |= '1';
      val -= 10;
    }
    if (ch) {
      retval[i] = ch;
      ++i;
    }

    retval[i] = '0' + val;
    retval.resize(i + 1);

    return retval;
  }
};

const char digit_pair_table[201] = {
    "00010203040506070809"
    "10111213141516171819"
    "20212223242526272829"
    "30313233343536373839"
    "40414243444546474849"
    "50515253545556575859"
    "60616263646566676869"
    "70717273747576777879"
    "80818283848586878889"
    "90919293949596979899"};

template <typename T, bool is_signed> struct itostr_impl<T, is_signed, 32> {
  static std::string cvt(T val) {
    char buf[11], ch = 0;
    char* start = buf + 1;
    char* p = start;
    bool neg = val < 0;
    int digit;

    if (is_signed) {
      if (neg) {
        if (val <= -2000000000) {
          ch = '2';
          val += 2000000000;
        }
        val = -val;
      } else if (val >= 2000000000) {
        ch = '2';
        val -= 2000000000;
      }
    } else {
      if (val >= 4000000000U) {
        ch |= '4';
        val -= 4000000000U;
      } else if (val >= 2000000000) {
        ch |= '2';
        val -= 2000000000;
      }
    }
    if (val >= 1000000000) {
      ch |= '1';
      val -= 1000000000;
    }

    if (ch) {
      *p = ch;
      ++p;
      ch = '0';
    } else if (val < 1000) {
      if (val < 10) goto d1;
      if (val < 1000) goto d10;
    } else {
      if (val < 100000) goto d1000;
      if (val < 10000000) goto d100000;
    }

#define DO_PAIR(n)                          \
  d##n : digit = val / n;                   \
  *(p++) = digit_pair_table[digit * 2];     \
  *(p++) = digit_pair_table[digit * 2 + 1]; \
  val -= n * digit;

    DO_PAIR(10000000);
    DO_PAIR(100000);
    DO_PAIR(1000);
    DO_PAIR(10);

  d1:
    *p = '0' | val;

    if (p > start && *start == '0') ++start;

    if (is_signed && neg) *--start = '-';

    return std::string(start, p + 1 - start);
  }
};

template <typename T> std::string itostr(T val) {
  (void)sizeof(assert_integral<T>);
  return itostr_impl<T, ((T)-1) < 0, sizeof(T) * CHAR_BIT>::cvt(val);
}
