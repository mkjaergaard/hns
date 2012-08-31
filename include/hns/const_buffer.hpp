#pragma once

#include <hns/raw_buffer.hpp>

namespace hns
{

class const_buffer : public raw_buffer
{
public:
  const_buffer(size_t len, shared_buffer next = shared_buffer()) :
    raw_buffer(new char[len], len, next)
  {
  }

  ~const_buffer()
  {
    delete pbase();
  }

};

}
