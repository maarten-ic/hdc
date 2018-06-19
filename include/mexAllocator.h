#pragma once

// CUSTOM STL allocator using the Intel MKL memory management functions
#include <mex.h>
#include <memory>
/**
 * \brief Allocator to use MATLAB memory manager
 * 
 * A custom C++ Allocator to use MATLAB memory manager.
 * 
 * \note DO NOT USE THIS ALLOCATOR BEYOND SIMPLE DATA TYPES!!
 *       In general, the use of C-based memory management functions should
 *       not be used in C++ Allocator class. However, it works fine as long
 *       as it is managing a simple data type (int, double, char, etc.).
 *       There may be unforeseen consequence if this class is used to allocate
 *       memory for a struct or class.
 * 
 * \note Intended Application: Use this Allocator for a generic C++ template
 *       library which must dynamically allocate memory but also releases the
 *       allocated memory to the user, which is under a specific memory management
 *       regime (i.e., MATLAB) to avoid copying.
 */
template <typename T>
class mexAllocator : public std::allocator<T>
{
public:
  mexAllocator() {}
  mexAllocator &operator=(const mexAllocator &rhs)
  {
    std::allocator<T>::operator=(rhs);
    return *this;
  }

  T * allocate(std::size_t n, const void *hint = NULL)
  {
    T * p = NULL;
    if (!hint)
    {
      p = reinterpret_cast<T *>(mxCalloc(n, sizeof(T)));
      mexMakeMemoryPersistent(p);
    }
    else
    {
      p = reinterpret_cast<T *>(mxRealloc((void *)hint, n * sizeof(T)));
    }
    return p;
  }

  void deallocate(T * p, std::size_t n)
  {
    mxFree(p);
  }

  void construct(T * p, const T &val)
  {
    new (p) T(val);
  }

  void destroy(T * p)
  {
    p->~T();
  }

};
