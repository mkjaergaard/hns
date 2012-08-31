#pragma once

#include <hns/buffer.hpp>
#include <streambuf>

namespace hns
{

class raw_buffer : public std::streambuf, public hns::buffer
{
public:
  raw_buffer(char* data, size_t len, shared_buffer next = shared_buffer()) :
    buffer(next)
  {
    setp(data, data+len);
    setg(data, data, data+len);
  }

  size_t free_size()
  {
    return pptr() - pbase();
  }

  virtual std::streambuf * streambuf()
  {
    return this;
  }

};

}
