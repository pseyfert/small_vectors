LDFLAGS  = -lstdc++ -m64 -g -march=native -lLLVM-9
CPPFLAGS = -march=native -std=c++17 -m64 -O3 -g -Wextra -Wall -Wshadow

CPPFLAGS += -isystem /home/pseyfert/.local/include
CPPFLAGS += -fopenmp

LDLIBS   += -L/home/pseyfert/.local/lib -lbenchmark discard.o
LDLIBS   += -lboost_container
