#include "SOAContainer/SOAContainer.h"
#include "SOAContainer/SOAField.h"
#include "discard.h"
#include <absl/container/inlined_vector.h>
#include <array>
#include <benchmark/benchmark.h>
#include <boost/container/small_vector.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <llvm/ADT/SmallVector.h>
#include <vector>

#include <boost/random/lagged_fibonacci.hpp>
#include <boost/random/taus88.hpp>
boost::random::lagged_fibonacci2281 random_gen;
// boost::random::taus88 random_gen;

template <size_t SIZE> struct bigthing {
  // double &x00() { return x02; }
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

struct smallthing {
  double &x00() { return m_x00; }
  double m_x00;
  double m_x01;
};

SOAFIELD_TRIVIAL(smallthing_field, smallthing_access, smallthing);
SOASKIN(smallthing_skin, smallthing_field) {
  SOASKIN_INHERIT_DEFAULT_METHODS(smallthing_skin);
  double &x00() { return this->smallthing_access().x00(); }
  smallthing_skin(double x) { this->smallthing_access().m_x00 = x; }
};

#define FIELD_DEFINE(z, n, data)                                               \
  SOAFIELD_TRIVIAL(                                                            \
      BOOST_PP_CAT(BOOST_PP_CAT(bigthing_, n), BOOST_PP_CAT(data, _field)),    \
      bigthing_access, bigthing<n>);                                           \
  SOASKIN(                                                                     \
      BOOST_PP_CAT(BOOST_PP_CAT(bt_, n), BOOST_PP_CAT(data, _skin)),           \
      BOOST_PP_CAT(BOOST_PP_CAT(bigthing_, n), BOOST_PP_CAT(data, _field))) {  \
    SOASKIN_INHERIT_DEFAULT_METHODS(                                           \
        BOOST_PP_CAT(BOOST_PP_CAT(bt_, n), BOOST_PP_CAT(data, _skin)));        \
    BOOST_PP_CAT(BOOST_PP_CAT(bt_, n), BOOST_PP_CAT(data, _skin))(double x) {  \
      this->bigthing_access().x02 = x;                                         \
    }                                                                          \
    double &x00() { return this->bigthing_access().x02; }                      \
  };                                                                           \
  SOASKIN(                                                                     \
      BOOST_PP_CAT(BOOST_PP_CAT(rbt_, n), BOOST_PP_CAT(data, _skin)),          \
      BOOST_PP_CAT(BOOST_PP_CAT(bigthing_, n), BOOST_PP_CAT(data, _field)),    \
      smallthing_field) {                                                      \
    SOASKIN_INHERIT_DEFAULT_METHODS(                                           \
        BOOST_PP_CAT(BOOST_PP_CAT(rbt_, n), BOOST_PP_CAT(data, _skin)));       \
    BOOST_PP_CAT(BOOST_PP_CAT(rbt_, n), BOOST_PP_CAT(data, _skin))(double x) { \
      this->smallthing_access().m_x00 = x;                                     \
    }                                                                          \
    double &x00() { return this->smallthing_access().m_x00; }                  \
  };

FIELD_DEFINE("foo", 0, )
BOOST_PP_REPEAT_FROM_TO(1, 32, FIELD_DEFINE, 0)

template <typename T, size_t N> std::vector<T> makething() {
  std::vector<T> t;
  for (size_t i = 0; i < N; ++i) {
    auto &back = t.emplace_back();
    back.x02 = random_gen();
  }
  return t;
}

template <template <class> class T, size_t N>
SOA::Container<std::vector, T> makesoathing() {
  SOA::Container<std::vector, T> t;
  for (size_t i = 0; i < N; ++i) {
    t.emplace_back(random_gen());
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

template <template <class> class T, size_t N, size_t M>
std::vector<SOA::Container<std::vector, T>> makesoathings() {
  std::vector<SOA::Container<std::vector, T>> t;
  for (size_t i = 0; i < M; ++i) {
    t.emplace_back(makesoathing<T, N>());
  }
  return t;
}

template <typename T, size_t ELEMENTS, size_t CONTAINERS>
static void generate_multi_dno(benchmark::State &state) {
  auto things = makethings<T, ELEMENTS, CONTAINERS>();
  for (auto _ : state) {
    const auto &container = things[(int)(random_gen() * things.size())];
    for (const auto &element : container) {
      benchmark::DoNotOptimize(element.x02 + 1.f);
    }
  }
}

template <template <class> class T, template <class> class S, size_t ELEMENTS,
          size_t CONTAINERS = 1000>
static void generate_multi_zip(benchmark::State &state) {
  auto b_things = makesoathings<T, ELEMENTS, CONTAINERS>();
  auto s_things = makesoathings<smallthing_skin, ELEMENTS, CONTAINERS>();
  for (auto _ : state) {
    auto &b_container = b_things[(int)(random_gen() * CONTAINERS)];
    auto &s_container = s_things[(int)(random_gen() * CONTAINERS)];
    auto container = zip(s_container, b_container).template view<S>();
    for (auto element : container) {
      benchmark::DoNotOptimize(element.x00() + 1.f);
    }
  }
}

template <typename x> struct warner;

template <template <class> class T, template <class> class S, size_t ELEMENTS,
          size_t CONTAINERS = 1000>
static void generate_multi_nozip(benchmark::State &state) {
  auto b_things = makesoathings<T, ELEMENTS, CONTAINERS>();
  auto s_things = makesoathings<smallthing_skin, ELEMENTS, CONTAINERS>();
  for (auto _ : state) {
    auto &s_container = s_things[(int)(random_gen() * CONTAINERS)];
    for (auto element : s_container) {
      benchmark::DoNotOptimize(element.x00() + 1.f);
    }
  }
}
template <template <class> class T, template <class> class S, size_t ELEMENTS,
          size_t CONTAINERS = 1000>
static void generate_multi_view(benchmark::State &state) {
  auto b_things = makesoathings<T, ELEMENTS, CONTAINERS>();
  auto s_things = makesoathings<smallthing_skin, ELEMENTS, CONTAINERS>();
  for (auto _ : state) {
    auto &s_container = s_things[(int)(random_gen() * CONTAINERS)];
    auto ss_container = s_container.template view<smallthing_skin>();
    for (auto element : ss_container) {
      benchmark::DoNotOptimize(element.x00() + 1.f);
    }
  }
}

auto compute_min = [](const std::vector<double> &v) -> double {
  return *(std::min_element(std::begin(v), std::end(v)));
};

// clang-format off
BENCHMARK_TEMPLATE(generate_multi_view , bt_0_skin, rbt_0_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_10_skin, rbt_10_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_20_skin, rbt_20_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_30_skin, rbt_30_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_40_skin, rbt_40_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_50_skin, rbt_50_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_60_skin, rbt_60_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_70_skin, rbt_70_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_80_skin, rbt_80_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_90_skin, rbt_90_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();

BENCHMARK_TEMPLATE(generate_multi_view , bt_100_skin, rbt_100_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_110_skin, rbt_110_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_120_skin, rbt_120_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_130_skin, rbt_130_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_140_skin, rbt_140_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_150_skin, rbt_150_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_160_skin, rbt_160_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_170_skin, rbt_170_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_180_skin, rbt_180_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_190_skin, rbt_190_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();

BENCHMARK_TEMPLATE(generate_multi_view , bt_200_skin, rbt_200_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_210_skin, rbt_210_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_220_skin, rbt_220_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_230_skin, rbt_230_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_240_skin, rbt_240_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_250_skin, rbt_250_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_260_skin, rbt_260_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_270_skin, rbt_270_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_280_skin, rbt_280_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_290_skin, rbt_290_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();

BENCHMARK_TEMPLATE(generate_multi_view , bt_300_skin, rbt_300_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_view , bt_310_skin, rbt_310_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_view , bt_320_skin, rbt_320_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_view , bt_330_skin, rbt_330_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_view , bt_340_skin, rbt_340_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_view , bt_350_skin, rbt_350_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_view , bt_360_skin, rbt_360_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_view , bt_370_skin, rbt_370_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_view , bt_380_skin, rbt_380_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_view , bt_390_skin, rbt_390_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();



BENCHMARK_TEMPLATE(generate_multi_nozip , bt_0_skin, rbt_0_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_10_skin, rbt_10_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_20_skin, rbt_20_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_30_skin, rbt_30_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_40_skin, rbt_40_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_50_skin, rbt_50_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_60_skin, rbt_60_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_70_skin, rbt_70_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_80_skin, rbt_80_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_90_skin, rbt_90_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();

BENCHMARK_TEMPLATE(generate_multi_nozip , bt_100_skin, rbt_100_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_110_skin, rbt_110_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_120_skin, rbt_120_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_130_skin, rbt_130_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_140_skin, rbt_140_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_150_skin, rbt_150_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_160_skin, rbt_160_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_170_skin, rbt_170_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_180_skin, rbt_180_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_190_skin, rbt_190_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();

BENCHMARK_TEMPLATE(generate_multi_nozip , bt_200_skin, rbt_200_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_210_skin, rbt_210_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_220_skin, rbt_220_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_230_skin, rbt_230_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_240_skin, rbt_240_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_250_skin, rbt_250_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_260_skin, rbt_260_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_270_skin, rbt_270_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_280_skin, rbt_280_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_290_skin, rbt_290_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();

BENCHMARK_TEMPLATE(generate_multi_nozip , bt_300_skin, rbt_300_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_nozip , bt_310_skin, rbt_310_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_nozip , bt_320_skin, rbt_320_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_nozip , bt_330_skin, rbt_330_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_nozip , bt_340_skin, rbt_340_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_nozip , bt_350_skin, rbt_350_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_nozip , bt_360_skin, rbt_360_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_nozip , bt_370_skin, rbt_370_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_nozip , bt_380_skin, rbt_380_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_nozip , bt_390_skin, rbt_390_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();

BENCHMARK_TEMPLATE(generate_multi_zip , bt_0_skin, rbt_0_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_10_skin, rbt_10_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_20_skin, rbt_20_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_30_skin, rbt_30_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_40_skin, rbt_40_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_50_skin, rbt_50_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_60_skin, rbt_60_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_70_skin, rbt_70_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_80_skin, rbt_80_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_90_skin, rbt_90_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();

BENCHMARK_TEMPLATE(generate_multi_zip , bt_100_skin, rbt_100_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_110_skin, rbt_110_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_120_skin, rbt_120_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_130_skin, rbt_130_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_140_skin, rbt_140_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_150_skin, rbt_150_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_160_skin, rbt_160_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_170_skin, rbt_170_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_180_skin, rbt_180_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_190_skin, rbt_190_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();

BENCHMARK_TEMPLATE(generate_multi_zip , bt_200_skin, rbt_200_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_210_skin, rbt_210_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_220_skin, rbt_220_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_230_skin, rbt_230_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_240_skin, rbt_240_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_250_skin, rbt_250_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_260_skin, rbt_260_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_270_skin, rbt_270_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_280_skin, rbt_280_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_290_skin, rbt_290_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();

BENCHMARK_TEMPLATE(generate_multi_zip , bt_300_skin, rbt_300_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
BENCHMARK_TEMPLATE(generate_multi_zip , bt_310_skin, rbt_310_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_zip , bt_320_skin, rbt_320_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_zip , bt_330_skin, rbt_330_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_zip , bt_340_skin, rbt_340_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_zip , bt_350_skin, rbt_350_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_zip , bt_360_skin, rbt_360_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_zip , bt_370_skin, rbt_370_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_zip , bt_380_skin, rbt_380_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
// BENCHMARK_TEMPLATE(generate_multi_zip , bt_390_skin, rbt_390_skin, 500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();



// BENCHMARK_TEMPLATE(generate_multi_dno    , smallthing    ,    500 , 1000) ->ComputeStatistics("min", compute_min) ->ThreadRange(1, 1) ->UseRealTime();
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


/**/

BENCHMARK_MAIN();
// clang-format on
