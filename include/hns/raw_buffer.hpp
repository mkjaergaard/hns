#pragma once

#include <hns/buffer.hpp>
#include <streambuf>

namespace hns
{

class raw_buffer : public std::streambuf, public hns::buffer
{
protected:
  void init(char* data, size_t len)
  {
    setp(data, data+len);
    setg(data, data, data+len);
  }

public:
  raw_buffer(char* data, size_t len)
  {
    init(data, len);
  }

  raw_buffer(char* data, size_t len, shared_buffer next) :
    buffer(next)
  {
    init(data, len);
  }

  virtual std::streambuf * streambuf()
  {
    return this;
  }

  virtual size_t data_count()
  {
    return pptr() - pbase();
  }


};

}
