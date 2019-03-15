#include <absl/container/inlined_vector.h>
#include <array>
#include "discard.h"
#include <benchmark/benchmark.h>
#include <boost/container/small_vector.hpp>
#include <boost/container/static_vector.hpp>
#include <llvm/ADT/SmallVector.h>
#include <vector>

#include <boost/random/taus88.hpp>
#include <boost/random/lagged_fibonacci.hpp>
boost::random::lagged_fibonacci2281 random_gen;
// boost::random::taus88 random_gen;

template <size_t SIZE> struct bigthing {
  double x00;
  double x01;
  double x02;
  double x03;
  double x04;
  double x05;
  double x06;
  double x07;

  double x10;
  double x11;
  double x12;
  double x13;
  double x14;
  double x15;
  double x16;
  double x17;

  std::array<double, SIZE> xxx;
};

struct smallthing1 {
  double x00;
  double x01;
};
struct smallthing2 {
  double x00;
  double x01;
};
struct smallthing3 {
  double x00;
  double x01;
};
struct smallthing4 {
  double x00;
  double x01;
};
struct smallthing5 {
  double x10;
  double x11;
};
struct smallthing6 {
  double x10;
  double x11;
};
struct smallthing7 {
  double x10;
  double x11;
};
struct smallthing8 {
  double x10;
  double x11;
};

template <typename T, size_t N> std::vector<T> makething() {
  std::vector<T> t;
  for (size_t i = 0; i < N; ++i) {
    auto &back = t.emplace_back();
    back.x00 = random_gen();
  }
  return t;
}

template <typename T, size_t N, size_t M>
std::vector<std::vector<T>> makethings() {
  std::vector<std::vector<T>> t;
  for (size_t i = 0; i < M; ++i) {
    t.emplace_back(makething<T, N>());
  }
  return t;
}

template <typename T, size_t ELEMENTS, size_t CONTAINERS>
static void generate_multi_dno(benchmark::State &state) {
  auto things = makethings<T, ELEMENTS, CONTAINERS>();
  for (auto _ : state) {
    const auto &container = things[(int)(random_gen() * things.size())];
    for (const auto &element : container) {
      benchmark::DoNotOptimize(element.x00);
    }
  }
}

template <typename T, size_t ELEMENTS, size_t CONTAINERS>
static void generate_multi_clob(benchmark::State &state) {
  auto things = makethings<T, ELEMENTS, CONTAINERS>();
  for (auto _ : state) {
    const auto &container = things[(int)(random_gen() * things.size())];
    for (const auto &element : container) {
      [[maybe_unused]] auto x = element.x00;
      benchmark::ClobberMemory();
    }
  }
}

template <typename T, size_t ELEMENTS, size_t CONTAINERS>
static void generate_multi_dno_clob(benchmark::State &state) {
  auto things = makethings<T, ELEMENTS, CONTAINERS>();
  for (auto _ : state) {
    const auto &container = things[(int)(random_gen() * things.size())];
    for (const auto &element : container) {
      benchmark::DoNotOptimize(element.x00);
      benchmark::ClobberMemory();
    }
  }
}

template <typename T, size_t ELEMENTS, size_t CONTAINERS>
static void generate_multi_discard(benchmark::State &state) {
  auto things = makethings<T, ELEMENTS, CONTAINERS>();
  for (auto _ : state) {
    const auto &container = things[(int)(random_gen() * things.size())];
    for (const auto &element : container) {
      discard(element.x00);
    }
  }
}

template <typename T, size_t ELEMENTS, size_t CONTAINERS>
static void generate_multi_dno_discard(benchmark::State &state) {
  auto things = makethings<T, ELEMENTS, CONTAINERS>();
  for (auto _ : state) {
    const auto &container = things[(int)(random_gen() * things.size())];
    for (const auto &element : container) {
      benchmark::DoNotOptimize(discard(element.x00));
    }
  }
}

auto compute_min = [](const std::vector<double> &v) -> double {
  return *(std::min_element(std::begin(v), std::end(v)));
};

// clang-format off
// BENCHMARK_TEMPLATE(generate_multi        ,     smallthing1,   500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 4) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_discard,     smallthing1,   500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 4) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi        , bigthing<  10>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 4) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_discard, bigthing<  10>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 4) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi        , bigthing< 100>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 4) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 100>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 4) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi        , bigthing< 500>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 4) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 500>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 4) ->UseRealTime();

BENCHMARK_TEMPLATE(generate_multi_discard, bigthing<   0>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing<  10>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing<  20>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing<  30>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing<  40>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing<  50>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing<  60>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing<  70>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing<  80>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing<  90>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 100>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 110>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 120>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 130>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 140>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 150>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 160>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 170>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 180>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 190>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 200>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 210>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 220>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 230>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 240>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 250>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 260>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 270>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 280>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 290>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 300>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_discard, bigthing< 310>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();



BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing<   0>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing<  10>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing<  20>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing<  30>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing<  40>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing<  50>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing<  60>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing<  70>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing<  80>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing<  90>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 100>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 110>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 120>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 130>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 140>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 150>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 160>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 170>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 180>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 190>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 200>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 210>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 220>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 230>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 240>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 250>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 260>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 270>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 280>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 290>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 300>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno    , bigthing< 310>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();





BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing<   0>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing<  10>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing<  20>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing<  30>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing<  40>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing<  50>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing<  60>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing<  70>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing<  80>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing<  90>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 100>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 110>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 120>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 130>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 140>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 150>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 160>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 170>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 180>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 190>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 200>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 210>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 220>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 230>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 240>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 250>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 260>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 270>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 280>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 290>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 300>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_discard, bigthing< 310>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();



BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing<   0>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing<  10>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing<  20>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing<  30>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing<  40>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing<  50>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing<  60>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing<  70>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing<  80>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing<  90>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 100>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 110>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 120>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 130>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 140>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 150>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 160>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 170>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 180>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 190>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 200>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 210>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 220>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 230>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 240>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 250>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 260>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 270>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 280>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 290>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 300>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_dno_clob, bigthing< 310>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();


BENCHMARK_TEMPLATE(generate_multi_clob, bigthing<   0>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing<  10>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing<  20>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing<  30>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing<  40>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing<  50>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing<  60>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing<  70>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing<  80>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing<  90>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 100>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 110>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 120>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 130>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 140>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 150>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 160>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 170>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 180>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 190>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 200>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 210>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 220>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 230>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 240>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 250>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 260>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 270>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 280>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 290>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 300>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_clob, bigthing< 310>,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();



BENCHMARK_MAIN();
// clang-format on
