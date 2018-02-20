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

static void test_std_vector         ( benchmark::State& state ) { create_and_push<std::vector<DATATYPE>>( state ); }
static void test_boost_small_vector ( benchmark::State& state ) { create_and_push<boost::container::small_vector<DATATYPE, CONTAINERSIZE>>( state ); }
static void test_boost_static_vector( benchmark::State& state ) { create_and_push<boost::container::static_vector<DATATYPE, CONTAINERSIZE>>( state ); }
static void test_llvm_small_vector  ( benchmark::State& state ) { create_and_push<llvm::SmallVector<DATATYPE, CONTAINERSIZE>>( state ); }
static void test_absl_inline_vector ( benchmark::State& state ) { create_and_push<absl::InlinedVector<DATATYPE, CONTAINERSIZE>>( state ); }

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

BENCHMARK( test_std_vector         )
    ->ComputeStatistics( "min", []( const std::vector<double>& v ) -> double {
      return *( std::min_element( std::begin( v ), std::end( v ) ) );
    } );
BENCHMARK( test_boost_small_vector )
    ->ComputeStatistics( "min", []( const std::vector<double>& v ) -> double {
      return *( std::min_element( std::begin( v ), std::end( v ) ) );
    } );
BENCHMARK( test_boost_static_vector)
    ->ComputeStatistics( "min", []( const std::vector<double>& v ) -> double {
      return *( std::min_element( std::begin( v ), std::end( v ) ) );
    } );
BENCHMARK( test_llvm_small_vector  )
    ->ComputeStatistics( "min", []( const std::vector<double>& v ) -> double {
      return *( std::min_element( std::begin( v ), std::end( v ) ) );
    } );
BENCHMARK( test_absl_inline_vector )
    ->ComputeStatistics( "min", []( const std::vector<double>& v ) -> double {
      return *( std::min_element( std::begin( v ), std::end( v ) ) );
    } );
BENCHMARK( RESERVE )
    ->ComputeStatistics( "min", []( const std::vector<double>& v ) -> double {
      return *( std::min_element( std::begin( v ), std::end( v ) ) );
    } );

BENCHMARK_MAIN();
