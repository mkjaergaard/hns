#pragma once

#include <streambuf>

namespace hns
{

class buffer
{
public:
  buffer()
  {
  }

  virtual std::streambuf * streambuf() = 0;

  virtual const char* data() const = 0;

};

}
