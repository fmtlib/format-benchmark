#if defined(__linux__) && defined(__clang__)
// Workaround for bug in gcc 4.4 standard library headers when compling with
// clang in C++11 mode.
namespace std { class type_info; }
#endif

#include <stdexcept>
#include <climits>
#include <cfloat>
#include <cstddef>

#ifdef SPEED_TEST
#ifdef HAVE_FORMAT
# include "fmt/format.h"
# include "fmt/compile.h"
#endif
#ifdef HAVE_BOOST
# include <boost/format.hpp>
#endif
#ifdef HAVE_FOLLY
# include <folly/Format.h>
#endif
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"
#include <iomanip>
#include <stdio.h>
#endif

// Throw instead of abort() so we can test error conditions.
#define TINYFORMAT_ERROR(reason) \
    throw std::runtime_error(reason);

#include "tinyformat.h"
#include <cassert>

#if 0
// Compare result of tfm::format() to C's sprintf().
template<typename... Args>
void compareSprintf(const Args&... args)
{
    std::string tfmResult = tfm::format(args...);
    char sprintfResult[200];
    sprintf(sprintfResult, args...);
    if(tfmResult != sprintfResult)
    {
        std::cout << tfmResult << std::endl;
        std::cout << sprintfResult << std::endl;
        assert(0 && "results didn't match, see above.");
    }
}
#endif

#define EXPECT_ERROR(expression)                            \
{                                                           \
    try { expression; assert(0 && "expected exception in "  \
                             #expression); }                \
    catch(std::runtime_error&) {}                           \
}

#define CHECK_EQUAL(a, b)                                  \
if(!((a) == (b)))                                          \
{                                                          \
    std::cout << "test failed, line " << __LINE__ << "\n"; \
    std::cout << (a) << " != " << (b) << "\n";             \
    std::cout << "[" #a ", " #b "]\n";                     \
    ++nfailed;                                             \
}

#ifdef FMT_PROFILE
# include <gperftools/profiler.h>
// Make sure the profiler library is linked in.
static int profiling_enabled = ProfilingIsEnabledForAllThreads();
#endif

#ifdef SPEED_TEST
void speedTest(const std::string& which)
{
    // Following is required so that we're not limited by per-character
    // buffering.
    std::ios_base::sync_with_stdio(false);
    const long maxIter = 2000000L;
    if(which == "printf")
    {
        // libc version
        for(long i = 0; i < maxIter; ++i)
            printf("%0.10f:%04d:%+g:%s:%p:%c:%%\n",
                1.234, 42, 3.13, "str", (void*)1000, (int)'X');
    }
    else if(which == "iostreams")
    {
        // Std iostreams version.  What a mess!!
        for(long i = 0; i < maxIter; ++i)
            std::cout << std::setprecision(10) << std::fixed << 1.234 << ':'
                << std::resetiosflags(std::ios::floatfield)
                << std::setw(4) << std::setfill('0') << 42 << std::setfill(' ') << ':'
                << std::setiosflags(std::ios::showpos) << 3.13 << std::resetiosflags(std::ios::showpos) << ':'
                << "str" << ':'
                << (void*)1000 << ':'
                << 'X' << ":%\n";
    }
    else if(which == "tinyformat")
    {
        // tinyformat version.
        for(long i = 0; i < maxIter; ++i)
            tfm::printf("%0.10f:%04d:%+g:%s:%p:%c:%%\n",
                        1.234, 42, 3.13, "str", (void*)1000, (int)'X');
    }
#ifdef HAVE_FORMAT
    else if(which == "format")
    {
        // fmt version.
        for(long i = 0; i < maxIter; ++i)
            fmt::print("{:.10f}:{:04}:{:+}:{}:{}:{}:%\n",
                1.234, 42, 3.13, "str", (void*)1000, 'X');
    }
    else if(which == "fmt::compile")
    {
        // fmt version (compiled).
        for(long i = 0; i < maxIter; ++i)
        {
            char buf[100];
            //fmt::memory_buffer buf;
            auto finished_at = fmt::format_to(
                buf, FMT_COMPILE("{:.10f}:{:04}:{:+}:{}:{}:{}:%\n"),
                1.234, 42, 3.13, "str", (void*)1000, 'X');
            *finished_at = '\0';
            std::puts(buf);
        }
    }
#endif
    else if(which == "folly")
    {
#ifdef HAVE_FOLLY
        // folly::format version
        for(long i = 0; i < maxIter; ++i)
            std::cout << folly::format("{:.10f}:{:04}:{:+}:{}:{}:{}:%\n",
                1.234, 42, 3.13, "str", (void*)1000, 'X');
#else
        fprintf(stderr, "folly is not available\n");
#endif
    }
    else if(which == "boost")
    {
#ifdef HAVE_BOOST
        // boost::format version
        for(long i = 0; i < maxIter; ++i)
            std::cout << boost::format("%0.10f:%04d:%+g:%s:%p:%c:%%\n")
                % 1.234 % 42 % 3.13 % "str" % (void*)1000 % (int)'X';
#else
        fprintf(stderr, "boost is not available\n");
#endif
    }
    else if(which == "stb_sprintf")
    {
        char buf[100];
        // stb_sprintf version
        for(long i = 0; i < maxIter; ++i) {
            stbsp_sprintf(buf, "%0.10f:%04d:%+g:%s:%p:%c:%%\n",
                1.234, 42, 3.13, "str", (void*)1000, (int)'X');
            fputs(buf, stdout);
        }
    }
    else
    {
        assert(0 && "speed test for which version?");
    }
}
#endif


int main(int argc, char* argv[])
{
#ifdef SPEED_TEST
    if(argc >= 2)
        speedTest(argv[1]);
    return 0;
#else
    return unitTests();
#endif
}
