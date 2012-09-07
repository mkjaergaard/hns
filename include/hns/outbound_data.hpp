#pragma once

#include <boost/utility.hpp>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <hns/buffer.hpp>

namespace hns
{

// SPLIT
struct boost_serializer
{
  template<typename T>
  static void pack(shared_buffer& buffer, const T& data)
  {
    std::ostream os(buffer->streambuf());
    boost::archive::binary_oarchive oarchive(os);

    oarchive << data;
  }

  template<typename T>
  static void unpack(shared_buffer& buffer, T& data)
  {
    std::istream is(buffer->streambuf());
    boost::archive::binary_iarchive iarchive(is);

    iarchive >> data;
  }
};

// SPLIT
class outbound_data_base
{
public:
  virtual ~outbound_data_base()
  {
  }

  virtual void pack(shared_buffer& buffer) const = 0;
};

// SPLIT
template<typename S, typename T>
class outbound_data : public outbound_data_base, boost::noncopyable
{
protected:
  const T& data_;

public:
  outbound_data(const T& data) :
    data_(data)
  {
  }

  virtual void pack(shared_buffer& buffer) const
  {
    S::template pack<T>(buffer, data_);
  }
};

// SPLIT
class outbound_pair : public outbound_data_base
{
protected:
  const outbound_data_base& first_;
  const outbound_data_base& second_;

public:
  outbound_pair(const outbound_data_base& first, const outbound_data_base& second) :
    first_(first),
    second_(second)
  {
  }

  virtual void pack(shared_buffer& buffer) const
  {
    first_.pack(buffer);
    second_.pack(buffer);
  }

};

// SPLIT
template<typename S, typename T>
class inbound_data
{
protected:
  T value_;

public:
  inbound_data(shared_buffer& buffer)
  {
    S::unpack(buffer, value_);
  }

  T& get()
  {
    return value_;
  }

};

}
