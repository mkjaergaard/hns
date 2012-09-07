#pragma once

#include <boost/shared_ptr.hpp>

namespace hns
{

class buffer;

class shared_buffer : public boost::shared_ptr<hns::buffer>
{
public:
  shared_buffer()
  {
  }

  template<typename T>
  shared_buffer(boost::shared_ptr<T> buf) :
    boost::shared_ptr<hns::buffer>(buf)
  {
  }

  void flush();

};




}
