#include <absl/container/inlined_vector.h>
#include <benchmark/benchmark.h>
#include <boost/container/small_vector.hpp>
#include <boost/container/static_vector.hpp>
#include <llvm/ADT/SmallVector.h>
#include <vector>

#include <boost/container/pmr/monotonic_buffer_resource.hpp>
#include <boost/container/pmr/polymorphic_allocator.hpp>

#include <boost/random/taus88.hpp>
boost::random::taus88 random_gen;

constexpr static int CONTAINERSIZE = 13;
constexpr static int FILLING_OFFSET = 8;
typedef int INNER_DATATYPE;
constexpr static int N_BIGTHINGS = 100;

template <typename BIGTHING_TYPE>
using OUTER_CONTAINER_TYPE = std::vector<BIGTHING_TYPE>;

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
struct bigthing {
  bigthing(size_t N) {
    size_t loopend = get_loopend( PICKED_FILLING );
    reserve_on_compile_demand<CONTAINER, RESERVE>( m_collection, loopend );
    for ( size_t i = 0; i < loopend; i++ ) {
      m_collection.push_back(N%(1+i));
    }
  }
  CONTAINER m_collection;
};


template <typename INNER_CONTAINER, bool RESERVE, filling PICKED_FILLING>
using OUTER_CONTAINER = OUTER_CONTAINER_TYPE<bigthing<INNER_CONTAINER, RESERVE, PICKED_FILLING>>;


template <typename INNER_CONTAINER, bool RESERVE, filling PICKED_FILLING>
static void create_and_push( benchmark::State& state )
{
  for ( auto _ : state ) {
    OUTER_CONTAINER<INNER_CONTAINER, RESERVE, PICKED_FILLING> asdf;
    asdf.reserve(N_BIGTHINGS);
    for ( size_t i = 0; i < N_BIGTHINGS ; i++ ) {
      asdf.push_back( i % CONTAINERSIZE );
    }
    benchmark::DoNotOptimize( asdf );
  }
}

template <typename CONTAINER, bool RESERVE, filling PICKED_FILLING>
struct bigthing_custom_alloc {
  template <typename ALLOC>
  bigthing_custom_alloc(size_t N, ALLOC alloc) : m_collection{alloc} {
    size_t loopend = get_loopend( PICKED_FILLING );
    reserve_on_compile_demand<CONTAINER, RESERVE>( m_collection, loopend );
    for ( size_t i = 0; i < loopend; i++ ) {
      m_collection.push_back(N%(1+i));
    }
  }
  CONTAINER m_collection;
};


template <typename INNER_CONTAINER, bool RESERVE, filling PICKED_FILLING>
using OUTER_CONTAINER_WITH_ALLOCATOR = OUTER_CONTAINER_TYPE<bigthing_custom_alloc<INNER_CONTAINER, RESERVE, PICKED_FILLING>>;


template <typename INNER_CONTAINER, bool RESERVE, filling PICKED_FILLING>
static void create_and_push_with_allocator( benchmark::State& state )
{
  using alloc_type = typename INNER_CONTAINER::allocator_type;
  INNER_DATATYPE storage[N_BIGTHINGS * (CONTAINERSIZE + FILLING_OFFSET)];
  boost::container::pmr::monotonic_buffer_resource resource(
      &storage, N_BIGTHINGS * (CONTAINERSIZE + FILLING_OFFSET));
  alloc_type
    local_alloc; // needs N_BIGTHINGS * ( CONTAINERSIZE + FILLING_OFFSET )
  for ( auto _ : state ) {
    OUTER_CONTAINER_WITH_ALLOCATOR<INNER_CONTAINER, RESERVE, PICKED_FILLING> asdf;
    asdf.reserve(N_BIGTHINGS);
    for ( size_t i = 0; i < N_BIGTHINGS ; i++ ) {
      asdf.push_back( i % CONTAINERSIZE, local_alloc );
    }
    benchmark::DoNotOptimize( asdf );
  }
}

template <bool RESERVE, filling PICKED_FILLING>
struct reservething {
  reservething(size_t N) {
    m_collection.reserve( CONTAINERSIZE );
    size_t loopend = get_loopend( PICKED_FILLING );
    reserve_on_compile_demand<decltype(m_collection), RESERVE>( m_collection, loopend );
    for ( size_t i = 0; i < loopend; i++ ) {
      m_collection.push_back(N%(1+i));
    }
  }
  std::vector<INNER_DATATYPE> m_collection;
};


template <bool RESERVE, filling PICKED_FILLING>
using reservething_CONTAINER = OUTER_CONTAINER_TYPE<reservething<RESERVE, PICKED_FILLING>>;


template <bool RESERVE, filling PICKED_FILLING>
static void create_and_push_and_reserve( benchmark::State& state )
{
  for ( auto _ : state ) {
    reservething_CONTAINER<RESERVE, PICKED_FILLING> asdf;
    asdf.reserve(N_BIGTHINGS);
    for ( size_t i = 0; i < N_BIGTHINGS; i++ ) {
      asdf.push_back( i % CONTAINERSIZE );
    }
    benchmark::DoNotOptimize( asdf );
  }
}

auto compute_min = []( const std::vector<double>& v ) -> double {
  return *( std::min_element( std::begin( v ), std::end( v ) ) );
};

// clang-format off

BENCHMARK_TEMPLATE( create_and_push, std::vector                    <INNER_DATATYPE>                , DONT_RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push_and_reserve                                                     , DONT_RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
// BENCHMARK_TEMPLATE( create_and_push, absl::InlinedVector            <INNER_DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, llvm::SmallVector              <INNER_DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, boost::container::small_vector <INNER_DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, boost::container::static_vector<INNER_DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();

BENCHMARK_TEMPLATE( create_and_push, std::vector                    <INNER_DATATYPE>                ,      RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push_and_reserve                                                     ,      RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
// BENCHMARK_TEMPLATE( create_and_push, absl::InlinedVector            <INNER_DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, llvm::SmallVector              <INNER_DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, boost::container::small_vector <INNER_DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, boost::container::static_vector<INNER_DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, EXACTFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();

BENCHMARK_TEMPLATE( create_and_push, std::vector                    <INNER_DATATYPE>                , DONT_RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push_and_reserve                                                     , DONT_RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
// BENCHMARK_TEMPLATE( create_and_push, absl::InlinedVector            <INNER_DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, llvm::SmallVector              <INNER_DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, boost::container::small_vector <INNER_DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, boost::container::static_vector<INNER_DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();

BENCHMARK_TEMPLATE( create_and_push, std::vector                    <INNER_DATATYPE>                ,      RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push_and_reserve                                                     ,      RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
// BENCHMARK_TEMPLATE( create_and_push, absl::InlinedVector            <INNER_DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, llvm::SmallVector              <INNER_DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, boost::container::small_vector <INNER_DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, boost::container::static_vector<INNER_DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, UNDERFULL)->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();

BENCHMARK_TEMPLATE( create_and_push, std::vector                    <INNER_DATATYPE>                , DONT_RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push_and_reserve                                                     , DONT_RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
// BENCHMARK_TEMPLATE( create_and_push, absl::InlinedVector            <INNER_DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, llvm::SmallVector              <INNER_DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, boost::container::small_vector <INNER_DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
//BENCHMARK_TEMPLATE( create_and_push, boost::container::static_vector<INNER_DATATYPE, CONTAINERSIZE> , DONT_RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();

BENCHMARK_TEMPLATE( create_and_push, std::vector                    <INNER_DATATYPE>                ,      RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push_and_reserve                                                     ,      RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
// BENCHMARK_TEMPLATE( create_and_push, absl::InlinedVector            <INNER_DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, llvm::SmallVector              <INNER_DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
BENCHMARK_TEMPLATE( create_and_push, boost::container::small_vector <INNER_DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();
// // BENCHMARK_TEMPLATE( create_and_push, boost::container::static_vector<INNER_DATATYPE, CONTAINERSIZE> ,      RESERVE_STORAGE, OVERFULL )->ComputeStatistics( "min", compute_min )->ThreadRange( 1, 4 )->UseRealTime();

// clang-format on

BENCHMARK_TEMPLATE(
    create_and_push,
    std::vector<INNER_DATATYPE,
                boost::container::pmr::polymorphic_allocator<INNER_DATATYPE>>,
    RESERVE_STORAGE, UNDERFULL)
    ->ComputeStatistics("min", compute_min)
    ->ThreadRange(1, 4)
    ->UseRealTime();
BENCHMARK_TEMPLATE(
    create_and_push,
    std::vector<INNER_DATATYPE,
                boost::container::pmr::polymorphic_allocator<INNER_DATATYPE>>,
    RESERVE_STORAGE, EXACTFULL)
    ->ComputeStatistics("min", compute_min)
    ->ThreadRange(1, 4)
    ->UseRealTime();
BENCHMARK_TEMPLATE(
    create_and_push,
    std::vector<INNER_DATATYPE,
                boost::container::pmr::polymorphic_allocator<INNER_DATATYPE>>,
    RESERVE_STORAGE, OVERFULL)
    ->ComputeStatistics("min", compute_min)
    ->ThreadRange(1, 4)
    ->UseRealTime();
BENCHMARK_TEMPLATE(
    create_and_push,
    std::vector<INNER_DATATYPE,
                boost::container::pmr::polymorphic_allocator<INNER_DATATYPE>>,
    DONT_RESERVE_STORAGE, UNDERFULL)
    ->ComputeStatistics("min", compute_min)
    ->ThreadRange(1, 4)
    ->UseRealTime();
BENCHMARK_TEMPLATE(
    create_and_push,
    std::vector<INNER_DATATYPE,
                boost::container::pmr::polymorphic_allocator<INNER_DATATYPE>>,
    DONT_RESERVE_STORAGE, EXACTFULL)
    ->ComputeStatistics("min", compute_min)
    ->ThreadRange(1, 4)
    ->UseRealTime();
BENCHMARK_TEMPLATE(
    create_and_push,
    std::vector<INNER_DATATYPE,
                boost::container::pmr::polymorphic_allocator<INNER_DATATYPE>>,
    DONT_RESERVE_STORAGE, OVERFULL)
    ->ComputeStatistics("min", compute_min)
    ->ThreadRange(1, 4)
    ->UseRealTime();

BENCHMARK_MAIN();
