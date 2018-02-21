#include <absl/container/inlined_vector.h>
#include <benchmark/benchmark.h>
#include <boost/container/small_vector.hpp>
#include <boost/container/static_vector.hpp>
#include <llvm/ADT/SmallVector.h>
#include <vector>

constexpr static int CONTAINERSIZE = 30;
typedef int DATATYPE;

template <typename CONTAINER>
static void create_and_push( benchmark::State& state )
{
  for ( auto _ : state ) {
    CONTAINER asdf;
    for ( size_t i = 0; i < CONTAINERSIZE; i++ ) {
      asdf.push_back( i );
    }
    benchmark::ClobberMemory();
  }
}

static void RESERVE( benchmark::State& state )
{
  for ( auto _ : state ) {
    std::vector<DATATYPE> asdf;
    asdf.reserve( CONTAINERSIZE );
    for ( size_t i = 0; i < CONTAINERSIZE; i++ ) {
      asdf.push_back( i );
    }
    benchmark::ClobberMemory();
  }
}

auto compute_min = []( const std::vector<double>& v ) -> double {
  return *( std::min_element( std::begin( v ), std::end( v ) ) );
};

BENCHMARK_TEMPLATE( create_and_push, std::vector<DATATYPE> )->ComputeStatistics( "min", compute_min );
BENCHMARK_TEMPLATE( create_and_push, boost::container::small_vector<DATATYPE, CONTAINERSIZE> )
    ->ComputeStatistics( "min", compute_min );
BENCHMARK_TEMPLATE( create_and_push, boost::container::static_vector<DATATYPE, CONTAINERSIZE> )
    ->ComputeStatistics( "min", compute_min );
BENCHMARK_TEMPLATE( create_and_push, llvm::SmallVector<DATATYPE, CONTAINERSIZE> )
    ->ComputeStatistics( "min", compute_min );
BENCHMARK_TEMPLATE( create_and_push, absl::InlinedVector<DATATYPE, CONTAINERSIZE> )
    ->ComputeStatistics( "min", compute_min );

BENCHMARK( RESERVE )
    ->ComputeStatistics( "min", []( const std::vector<double>& v ) -> double {
      return *( std::min_element( std::begin( v ), std::end( v ) ) );
    } );

BENCHMARK_MAIN();
