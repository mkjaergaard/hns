#pragma once

#include <hns/shared_buffer.hpp>
#include <streambuf>

namespace hns
{

class buffer
{
protected:
  shared_buffer next_;

public:
  buffer()
  {
  }

  buffer(shared_buffer next) :
    next_(next)
  {
  }

  virtual std::streambuf * streambuf() = 0;

};

}
