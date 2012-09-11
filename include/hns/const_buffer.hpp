#pragma once

#include <hns/raw_buffer.hpp>

namespace hns
{

class const_buffer : public raw_buffer
{
public:
  const_buffer(size_t len) :
    raw_buffer(new char[len], len)
  {
  }

  ~const_buffer()
  {
    delete pbase();
  }

};

}
