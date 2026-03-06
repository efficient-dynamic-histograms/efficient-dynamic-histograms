
#include <algorithms/Loda.hpp>
#include <cmath>
#include <gtest/gtest.h>

TEST(allocator, allignment) {
  for (int i = 0; i < 5; ++i) {
    std::vector<double, AlignedAllocator<double, 64>> v(
        16, 0.0); // allocate 16 doubles
    std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(v.data());
    std::cout << "Iteration " << i << " -> data() = 0x" << std::hex << addr
              << "   (addr % 64) = " << std::dec << (addr % 64) << "\n";
  }
}