#include <absl/container/inlined_vector.h>
#include <benchmark/benchmark.h>
#include <boost/container/small_vector.hpp>
#include <boost/container/static_vector.hpp>
#include <llvm/ADT/SmallVector.h>
#include <vector>

constexpr int rbound         = 1e8;
constexpr int CONTAINERSIZE  = 30;
typedef int DATATYPE;

static void RESERVE()  [[maybe_unused]]
{
#pragma omp parallel
  {
    for ( size_t r = 0; r < rbound; r++ ) {
      std::vector<int> asdf;
      asdf.reserve( CONTAINERSIZE );
      for ( size_t i = 0; i < CONTAINERSIZE; i++ ) {
        asdf.push_back( i );
      }
      asm volatile( "" :: "m"( asdf ) );
    }
  }
}

template <typename CONTAINER>
static void create_and_push_often() [[maybe_unused]]
{
#pragma omp parallel
  {
    for ( size_t r = 0; r < rbound; r++ ) {
      CONTAINER asdf;
      for ( size_t i = 0; i < CONTAINERSIZE; i++ ) {
        asdf.push_back( i );
      }
      // original version, kept for documentation purposes
      // asm volatile( "" :: "m"( asdf ) );
      benchmark::ClobberMemory();
    }
  }
}

int main()
{
  // RESERVE();
  //create_and_push_often<std::vector<DATATYPE>>();
  create_and_push_often<boost::container::small_vector<DATATYPE, CONTAINERSIZE>>();
  //create_and_push_often<boost::container::static_vector<DATATYPE, CONTAINERSIZE>>();
  //create_and_push_often<llvm::SmallVector<DATATYPE, CONTAINERSIZE>>();
  //create_and_push_often<absl::InlinedVector<DATATYPE, CONTAINERSIZE>>();
  return 0;
}
