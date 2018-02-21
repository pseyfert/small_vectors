#include <absl/container/inlined_vector.h>
#include <benchmark/benchmark.h>
#include <boost/container/small_vector.hpp>
#include <boost/container/static_vector.hpp>
#include <llvm/ADT/SmallVector.h>
#include <vector>

#define make_full 1
#define always_reserve 1
#undef  overfill
#undef  underfill

constexpr static int CONTAINERSIZE = 30;
constexpr static int FILLING_OFFSET = 8;
typedef int INNER_DATATYPE;
constexpr static int N_BIGTHINGS = 100;

template <typename BIGTHING_TYPE>
using OUTER_CONTAINER_TYPE = std::vector<BIGTHING_TYPE>;




auto compute_min = []( const std::vector<double>& v ) -> double {
  return *( std::min_element( std::begin( v ), std::end( v ) ) );
};

template <typename CONTAINER>
struct bigthing {
  bigthing(size_t N) {
#ifdef make_full
    constexpr int loopend = CONTAINERSIZE;
#endif
#ifdef overfill
    constexpr int loopend = CONTAINERSIZE + FILLING_OFFSET;
#endif
#ifdef underfill
    constexpr int loopend = CONTAINERSIZE - FILLING_OFFSET;
#endif
#ifdef always_reserve
    m_collection.reserve(loopend);
#endif
    for ( size_t i = 0; i < loopend; i++ ) {
      m_collection.push_back(N%(1+i));
    }
  }
  CONTAINER m_collection;
};


template <typename INNER_CONTAINER>
using OUTER_CONTAINER = OUTER_CONTAINER_TYPE<bigthing<INNER_CONTAINER>>;


template <typename INNER_CONTAINER>
static void create_and_push( benchmark::State& state )
{
  for ( auto _ : state ) {
    OUTER_CONTAINER<INNER_CONTAINER> asdf;
    asdf.reserve(N_BIGTHINGS);
    for ( size_t i = 0; i < N_BIGTHINGS ; i++ ) {
      asdf.push_back( i % CONTAINERSIZE );
    }
    benchmark::ClobberMemory();
  }
}



template <typename CONTAINER>
struct reservething {
  reservething(size_t N) {
    m_collection.reserve( CONTAINERSIZE );
#ifdef make_full
    constexpr int loopend = CONTAINERSIZE;
#endif
#ifdef overfill
    constexpr int loopend = CONTAINERSIZE + FILLING_OFFSET;
#endif
#ifdef underfill
    constexpr int loopend = CONTAINERSIZE - FILLING_OFFSET;
#endif
#ifdef always_reserve
    m_collection.reserve(loopend);
#endif
    for ( size_t i = 0; i < loopend; i++ ) {
      m_collection.push_back(N%(1+i));
    }
  }
  CONTAINER m_collection;
};

static void create_and_push_and_reserve( benchmark::State& state )
{
  for ( auto _ : state ) {
    OUTER_CONTAINER_TYPE<reservething<std::vector<INNER_DATATYPE>>> asdf;
    asdf.reserve(N_BIGTHINGS);
    for ( size_t i = 0; i < N_BIGTHINGS; i++ ) {
      asdf.push_back( i % CONTAINERSIZE );
    }
    benchmark::ClobberMemory();
  }
}

BENCHMARK_TEMPLATE( create_and_push, std::vector                    <INNER_DATATYPE               > )->ComputeStatistics( "min", compute_min )->ThreadRange(1,4);
BENCHMARK_TEMPLATE( create_and_push, boost::container::small_vector <INNER_DATATYPE, CONTAINERSIZE> )->ComputeStatistics( "min", compute_min )->ThreadRange(1,4);
#ifndef overfill
BENCHMARK_TEMPLATE( create_and_push, boost::container::static_vector<INNER_DATATYPE, CONTAINERSIZE> )->ComputeStatistics( "min", compute_min )->ThreadRange(1,4);
#endif
BENCHMARK_TEMPLATE( create_and_push, llvm::SmallVector              <INNER_DATATYPE, CONTAINERSIZE> )->ComputeStatistics( "min", compute_min )->ThreadRange(1,4);
BENCHMARK_TEMPLATE( create_and_push, absl::InlinedVector            <INNER_DATATYPE, CONTAINERSIZE> )->ComputeStatistics( "min", compute_min )->ThreadRange(1,4);
BENCHMARK(          create_and_push_and_reserve                                                     )->ComputeStatistics( "min", compute_min )->ThreadRange(1,4);

BENCHMARK_MAIN();
