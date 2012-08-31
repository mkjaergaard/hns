#pragma once

#include <streambuf>
#include <boost/asio/basic_streambuf.hpp>

namespace hns
{

class auto_buffer : public boost::asio::streambuf
{
public:
  buffer(size_t start_size)
  {
  }

  size_t free_size()
  {
    return pptr() - pbase();
  }


};

}
