#pragma once

#include <utils/Types.hpp>

template <typename T, std::size_t Alignment> struct AlignedAllocator {
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using void_pointer = void *;
  using const_void_pointer = const void *;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  template <typename U> struct rebind {
    using other = AlignedAllocator<U, Alignment>;
  };

  AlignedAllocator() noexcept = default;

  template <typename U>
  constexpr AlignedAllocator(const AlignedAllocator<U, Alignment> &) noexcept {
  }

  pointer allocate(size_type n) {
    void *ptr = nullptr;
    if (posix_memalign(&ptr, Alignment, n * sizeof(T)) != 0) {
      throw std::bad_alloc();
    }
    return static_cast<pointer>(ptr);
  }

  void deallocate(pointer p, size_type) noexcept {
    free(p);
  }
};