#include <absl/container/inlined_vector.h>
#include <benchmark/benchmark.h>
#include <boost/container/small_vector.hpp>
#include <boost/container/static_vector.hpp>
#include <llvm/ADT/SmallVector.h>
#include <vector>

#include <boost/random/taus88.hpp>
boost::random::taus88 random_gen;

constexpr static int CONTAINERSIZE = 13;
constexpr static int FILLING_OFFSET = 8;
typedef int DATATYPE;

enum filling { EXACTFULL, UNDERFULL, OVERFULL };

constexpr bool RESERVE_STORAGE      = true;
constexpr bool DONT_RESERVE_STORAGE = false;

inline size_t get_loopend( filling PICKED_FILLING )
{
  if ( EXACTFULL == PICKED_FILLING ) return CONTAINERSIZE;
  if ( UNDERFULL == PICKED_FILLING ) return CONTAINERSIZE - random_gen() % FILLING_OFFSET;
  if ( OVERFULL  == PICKED_FILLING ) return CONTAINERSIZE + random_gen() % FILLING_OFFSET;
  return 0;
}

template <typename CONTAINER, bool RESERVE>
inline void reserve_on_compile_demand( typename std::enable_if<RESERVE, CONTAINER>::type& asdf, size_t loopend )
{
  asdf.reserve( loopend );
}

template <typename CONTAINER, bool RESERVE>
inline void reserve_on_compile_demand( typename std::enable_if<!RESERVE, CONTAINER>::type&, int )
{
}

template <typename CONTAINER, bool RESERVE, filling PICKED_FILLING>
static void create_and_push( benchmark::State& state )
{
  for ( auto _ : state ) {
    CONTAINER asdf;
    size_t loopend = get_loopend( PICKED_FILLING );
    reserve_on_compile_demand<CONTAINER, RESERVE>( asdf, loopend );
    for ( size_t i = 0; i < loopend; i++ ) {
      asdf.push_back( random_gen() );
    }
    benchmark::ClobberMemory();
  }
}

template <bool RESERVE, filling PICKED_FILLING>
static void reserve( benchmark::State& state )
{
  for ( auto _ : state ) {
    std::vector<DATATYPE> asdf;
    asdf.reserve( CONTAINERSIZE );
    size_t loopend = get_loopend( PICKED_FILLING );
    reserve_on_compile_demand<decltype(asdf), RESERVE>( asdf, loopend );
    for ( size_t i = 0; i < loopend; i++ ) {
      asdf.push_back( random_gen() );
    }
    benchmark::ClobberMemory();
  }
}

auto compute_min = []( const std::vector<double>& v ) -> double {
  return *( std::min_element( std::begin( v ), std::end( v ) ) );
};

BENCHMARK_TEMPLATE( create_and_push, std::vector                    <DATATYPE>                , DONT_RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( reserve                                                                   , DONT_RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, absl::InlinedVector            <DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, llvm::SmallVector              <DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, boost::container::small_vector <DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, boost::container::static_vector<DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();

BENCHMARK_TEMPLATE( create_and_push, std::vector                    <DATATYPE>                ,      RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( reserve                                                                   ,      RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, absl::InlinedVector            <DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, llvm::SmallVector              <DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, boost::container::small_vector <DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, boost::container::static_vector<DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();

BENCHMARK_TEMPLATE( create_and_push, std::vector                    <DATATYPE>                , DONT_RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( reserve                                                                   , DONT_RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, absl::InlinedVector            <DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, llvm::SmallVector              <DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, boost::container::small_vector <DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, boost::container::static_vector<DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();

BENCHMARK_TEMPLATE( create_and_push, std::vector                    <DATATYPE>                ,      RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( reserve                                                                   ,      RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, absl::InlinedVector            <DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, llvm::SmallVector              <DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, boost::container::small_vector <DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, boost::container::static_vector<DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();

BENCHMARK_TEMPLATE( create_and_push, std::vector                    <DATATYPE>                , DONT_RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( reserve                                                                   , DONT_RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, absl::InlinedVector            <DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, llvm::SmallVector              <DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, boost::container::small_vector <DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
//BENCHMARK_TEMPLATE( create_and_push, boost::container::static_vector<DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();

BENCHMARK_TEMPLATE( create_and_push, std::vector                    <DATATYPE>                ,      RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( reserve                                                                   ,      RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, absl::InlinedVector            <DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, llvm::SmallVector              <DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, boost::container::small_vector <DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
//BENCHMARK_TEMPLATE( create_and_push, boost::container::static_vector<DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();


BENCHMARK_MAIN();
