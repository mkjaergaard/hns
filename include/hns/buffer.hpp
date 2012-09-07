#pragma once

#include <hns/shared_buffer.hpp>
#include <streambuf>

namespace hns
{

class buffer
{
protected:
  shared_buffer next_;
  size_t start_data_count_;

public:
  buffer() :
    start_data_count_(0)
  {
  }

  buffer(shared_buffer next) :
    next_(next),
    start_data_count_(0)
  {
  }

  virtual std::streambuf * streambuf() = 0;

  virtual bool is_empty()
  {
    return data_count() == start_data_count_;
  };

  virtual size_t data_count() {
    return 0;
  }

  virtual void mark_empty()
  {
    start_data_count_ = data_count();
  }

  shared_buffer& next()
  {
    return next_;
  }


};

}
