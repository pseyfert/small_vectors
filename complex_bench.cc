#include <absl/container/inlined_vector.h>
#include <benchmark/benchmark.h>
#include <boost/container/small_vector.hpp>
#include <boost/container/static_vector.hpp>
#include <llvm/ADT/SmallVector.h>
#include <vector>

constexpr static int CONTAINERSIZE = 30;
constexpr static int N_BIGTHINGS = 100;
typedef int INNER_DATATYPE;
template <typename BIGTHING_TYPE>
using OUTER_CONTAINER_TYPE = std::vector<BIGTHING_TYPE>;









template <typename CONTAINER>
struct bigthing {
  bigthing(size_t N) {
    for (size_t ii = 0 ; ii < N ; ++ii) {
      m_collection.push_back(ii/2);
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

static void test_std_vector         ( benchmark::State& state ) { create_and_push<std::vector<INNER_DATATYPE>>( state ); }
static void test_boost_small_vector ( benchmark::State& state ) { create_and_push<boost::container::small_vector<INNER_DATATYPE, CONTAINERSIZE>>( state ); }
static void test_boost_static_vector( benchmark::State& state ) { create_and_push<boost::container::static_vector<INNER_DATATYPE, CONTAINERSIZE>>( state ); }
static void test_llvm_small_vector  ( benchmark::State& state ) { create_and_push<llvm::SmallVector<INNER_DATATYPE, CONTAINERSIZE>>( state ); }
static void test_absl_inline_vector ( benchmark::State& state ) { create_and_push<absl::InlinedVector<INNER_DATATYPE, CONTAINERSIZE>>( state ); }


template <typename CONTAINER>
struct reservething {
  reservething(size_t N) {
    m_collection.reserve(N);
    for (size_t ii = 0 ; ii < N ; ++ii) {
      m_collection.push_back(ii);
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

BENCHMARK( test_std_vector         )
    ->ComputeStatistics( "min", []( const std::vector<double>& v ) -> double {
      return *( std::min_element( std::begin( v ), std::end( v ) ) );
    } );
BENCHMARK( test_boost_small_vector )
    ->ComputeStatistics( "min", []( const std::vector<double>& v ) -> double {
      return *( std::min_element( std::begin( v ), std::end( v ) ) );
    } );

// BAD ALLOC

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
BENCHMARK( create_and_push_and_reserve )
    ->ComputeStatistics( "min", []( const std::vector<double>& v ) -> double {
      return *( std::min_element( std::begin( v ), std::end( v ) ) );
    } );

BENCHMARK_MAIN();
