# my system defaults
LDFLAGS  = -lstdc++ -m64 -g -march=native -flto
CPPFLAGS = -march=native -std=c++14 -m64 -O3 -g -Wextra -Wall -Wshadow



CPPFLAGS += -isystem /home/pseyfert/coding/benchmark/install/include
CPPFLAGS += -isystem /usr/include/llvm-3.9/
CPPFLAGS += -isystem /home/pseyfert/coding/abseil-cpp

CPPFLAGS += -fopenmp

LDLIBS   += -L/home/pseyfert/coding/benchmark/install/lib -lbenchmark
LDLIBS   += -L/usr/lib/llvm-3.9/lib -lLLVM

# under development
# tbb_bench: CPPFLAGS+= -isystem /home/pseyfert/coding/tbb/include
# tbb_bench: LDLIBS  += -L/home/pseyfert/coding/tbb/build/linux_intel64_gcc_cc6.3.0_libc2.24_kernel4.9.0_release -ltbb
# tbb_bench: tbb_bench.cc
