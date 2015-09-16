//  Copyright (c) 2001-2010 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <climits>
#include <cstdlib>

#include <iostream>
#include <sstream>
#include <limits>       // std::numeric_limits
#include <algorithm>    // std::reverse
#include <boost/format.hpp>

#include "high_resolution_timer.hpp"

#include "cppformat/format.h"

//  This value specifies, how to unroll the integer string generation loop in
//  Karma.
//      Set this to some integer in between 0 (no unrolling) and max expected
//      integer string len (complete unrolling). If not specified, this value
//      defaults to 6.
#define BOOST_KARMA_NUMERICS_LOOP_UNROLL 6

#include <boost/spirit/include/karma.hpp>

namespace cppx {
    using std::numeric_limits;
    using std::reverse;

    typedef numeric_limits<long>    Long_info;
    int const long_digits   = Long_info::max_digits10;
    int const long_bufsize  = long_digits + 2;

    inline void unsigned_to_decimal( unsigned long number, char* buffer )
    {
        if( number == 0 )
        {
            *buffer++ = '0';
        }
        else
        {
            char* p_first = buffer;
            while( number != 0 )
            {
                *buffer++ = '0' + number % 10;
                number /= 10;
            }
            reverse( p_first, buffer );
        }
        *buffer = '\0';
    }

    inline auto decimal_from_unsigned( unsigned long number, char* buffer )
        -> char const*
    {
        unsigned_to_decimal( number, buffer );
        return buffer;
    }

    inline void to_decimal( long number, char* buffer )
    {
        if( number < 0 )
        {
            buffer[0] = '-';
            unsigned_to_decimal( -number, buffer + 1 );
        }
        else
        {
            unsigned_to_decimal( number, buffer );
        }
    }

    inline auto decimal_from( long number, char* buffer )
        -> char const*
    {
        to_decimal( number, buffer );
        return buffer;
    }
}  // namespace cppx

using namespace std;
using namespace boost::spirit;

#define MAX_ITERATION 10000000

///////////////////////////////////////////////////////////////////////////////
struct random_fill
{
    int operator()() const
    {
        int scale = std::rand() / 100 + 1;
        return (std::rand() * std::rand()) / scale;
    }
};

std::size_t first_size = ~0;
void Report(const char *method_name,
  const std::vector<int> &v, double time, std::size_t size)
{
  if (first_size == ~0)
    first_size = size;
  if (size != first_size) {
    cout << "Invalid result for " << method_name << endl;
    std::exit(1);
  }
  cout << method_name << ":\t" << time << " [s]" << endl;
}

extern "C" char *ltoa(long N, char *str, int base);

#include "itostr.cc"

///////////////////////////////////////////////////////////////////////////////
int main()
{
    namespace karma = boost::spirit::karma;

    cout << "Converting " << MAX_ITERATION
         << " randomly generated int values to strings." << flush << endl;

    std::srand(0);
    std::vector<int> v (MAX_ITERATION);
    std::generate(v.begin(), v.end(), random_fill()); // randomly fill the vector
    std::size_t size = 0;

    // test ltoa
    {
        //[karma_int_performance_ltoa
        char buffer[65]; // we don't expect more than 64 bytes to be generated here
        size = 0;
        //<-
        util::high_resolution_timer t;
        //->
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            ltoa(v[i], buffer, 10);
            size += strlen(buffer);
        }
        //]
        double time = t.elapsed();
        Report("ltoa", v, time, size);
    }

    // test the C libraries sprintf function (the most low level function for
    // string conversion available)
    {
        //[karma_int_performance_sprintf
        char buffer[65]; // we don't expect more than 64 bytes to be generated here
        size = 0;
        //<-
        util::high_resolution_timer t;
        //->
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            sprintf(buffer, "%d", v[i]);
            size += strlen(buffer);
        }
        //]
        double time = t.elapsed();
        Report("sprintf", v, time, size);
    }

    // test the C libraries sprintf function (the most low level function for
    // string conversion available)
    {
        //[karma_int_performance_sprintf
        char buffer[65]; // we don't expect more than 64 bytes to be generated here
        size = 0;
        //<-
        util::high_resolution_timer t;
        //->
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            sprintf(buffer, "%d", v[i]);
            //<-
            std::string str = buffer;      // compensate for string ops in other benchmarks
            //->
            size += strlen(str.c_str());
        }
        //]
        double time = t.elapsed();
        Report("sprintf+std::string", v, time, size);
    }

    // test std::stringstream
    {
        //[karma_int_performance_stringstream
        std::stringstream str;
        size = 0;
        //<-
        util::high_resolution_timer t;
        //->
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            str.str("");
            str << v[i];
            size += strlen(str.str().c_str());
        }
        //]
        double time = t.elapsed();
        Report("std::stringstream", v, time, size);
    }

    // test the std::to_string library
    {
        //[karma_int_performance_to_string
        //<-
        size = 0;
        util::high_resolution_timer t;
        //->
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            size += strlen(std::to_string(v[i]).c_str());
        }
        //]
        double time = t.elapsed();
        Report("std::to_string", v, time, size);
    }

    // test the Boost.Format library
    {
        //[karma_int_performance_boost_format
        size = 0;
        boost::format int_format("%d");
        //<-
        util::high_resolution_timer t;
        //->
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            size += strlen(boost::str(int_format % v[i]).c_str());
        }
        //]
        double time = t.elapsed();
        Report("boost::format", v, time, size);
    }

    // test boost::lexical_cast
    {
        //[karma_int_performance_boost_lexical_cast
        size = 0;
        //<-
        util::high_resolution_timer t;
        //->
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            size += strlen(boost::lexical_cast<std::string>(v[i]).c_str());
        }
        //]
        double time = t.elapsed();
        Report("boost::lexical_cast", v, time, size);
    }

    // test karma::generate
    {
        size = 0;
        util::high_resolution_timer t;

        //[karma_int_performance_plain
        char buffer[65]; // we don't expect more than 64 bytes to be generated here
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            char *ptr = buffer;
            karma::generate(ptr, int_, v[i]);
            *ptr = '\0';
            size += strlen(buffer);
        }
        //]
        double time = t.elapsed();
        Report("karma::generate", v, time, size);
    }

    // test karma::generate
    {
        size = 0;
        util::high_resolution_timer t;

        //[karma_int_performance_plain
        char buffer[65]; // we don't expect more than 64 bytes to be generated here
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            char *ptr = buffer;
            karma::generate(ptr, int_, v[i]);
            *ptr = '\0';
            //<-
            std::string str = buffer;      // compensate for string ops in other benchmarks
            //->
            size += strlen(str.c_str());
        }
        //]
        double time = t.elapsed();
        Report("karma::generate+std::string", v, time, size);
    }

    // test fmt::Writer
    {
        size = 0;
        util::high_resolution_timer t;

        //[karma_int_performance_format_write
        fmt::MemoryWriter writer;
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            writer.clear();
            writer << v[i];
            size += strlen(writer.c_str());
        }
        //]
        double time = t.elapsed();
        Report("fmt::Writer", v, time, size);
    }

        // test fmt::Writer
    {
        size = 0;
        util::high_resolution_timer t;

        //[karma_int_performance_format_write
        fmt::MemoryWriter writer;
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            writer.clear();
            writer << v[i];
            //<-
            std::string str = writer.c_str();      // compensate for string ops in other benchmarks
            //->
            size += strlen(str.c_str());
        }
        //]
        double time = t.elapsed();
        Report("fmt::Writer+std::string", v, time, size);
    }

    // test fmt::format
    {
        util::high_resolution_timer t;
        size = 0;

        //[karma_int_performance_format_format
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            size += strlen(fmt::format("{}", v[i]).c_str());
        }
        //]
        double time = t.elapsed();
        Report("fmt::format", v, time, size);
    }

    // test fmt::FormatInt
    {
        size = 0;
        util::high_resolution_timer t;

        //[karma_int_performance_itoa
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            //<-
            size += strlen(fmt::FormatInt(v[i]).c_str());
            //->
        }
        //]
        double time = t.elapsed();
        Report("fmt::FormatInt", v, time, size);
    }

    // test cppx::decimal_from
    {
        //[karma_int_performance_ltoa
        char buffer[65]; // we don't expect more than 64 bytes to be generated here
        size = 0;
        //<-
        util::high_resolution_timer t;
        //->
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            cppx::decimal_from(v[i], buffer);
            size += strlen(buffer);
        }
        //]
        double time = t.elapsed();
        Report("cppx::decimal_from", v, time, size);
    }

    // test itostr
    {
        //[karma_int_performance_itostr
        size = 0;
        //<-
        util::high_resolution_timer t;
        //->
        for (int i = 0; i < MAX_ITERATION; ++i)
        {
            size += strlen(itostr(v[i]).c_str());
        }
        //]
        double time = t.elapsed();
        Report("itostr", v, time, size);
    }

    return 0;
}
