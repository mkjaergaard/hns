#pragma once

#include <vector>
#include <hns/distributed_vector_base.hpp>
#include <boost/asio/buffer.hpp>
#include <hns/id.hpp>
#include <hns/asio_buffer.hpp>
#include <hns/distributed_header.hpp>
#include <boost/serialization/vector.hpp>

namespace hns
{

template<typename T>
class distributed_vector : public distributed_vector_base
{
protected:
  std::vector<T> list_;
  std::vector<T> buffer_;

  typedef std::set<client_id_type> subscriber_list_type;
  subscriber_list_type subscriber_list_;

  server_id_type id_;

public:
  distributed_vector()
  {
  }

  void send_all(const client_id_type& client_id)
  {
    update msg;
    msg.type = update::complete;
    msg.num_entries = list_.size();
    msg.index = count_;

    hns::shared_buffer data(new hns::asio_buffer());
    std::ostream os(data->streambuf());
    boost::archive::binary_oarchive a(os);

    a << msg;
    a << list_;

  }

  void send_updates(const client_id_type& client_id)
  {
    update msg;
    msg.type = update::partial;
    msg.num_entries = list_.size();
    msg.index = count_ + 1;

    hns::shared_buffer b(new boost::asio::streambuf());
    std::ostream os(b->streambuf());
    boost::archive::binary_oarchive a(os);

    a << msg;
    a << buffer_;

    list_.insert(list_.end(), buffer_.begin(), buffer_.end());
    buffer_.clear();
    ++count_;
  }

  void flush()
  {
    for(subscriber_list_type::iterator it = subscriber_list_.begin();
	it != subscriber_list_.end();
	it++)
    {
      send_updates(*it);
    }
  }

  std::size_t recv(const header& hdr, hns::shared_buffer data)
  {
    assert(hdr.payload_type == header::control);

    std::istream is(data->streambuf());
    boost::archive::binary_iarchive archive(is);

    control msg;
    archive >> msg;

    handle_control_message(msg);
    return 0;
  }

  void handle_control_message(const control& msg)
  {
    switch(msg.command)
    {
    case control::subscribe:
      subscriber_list_.insert(msg.client_id);
      send_all(msg.client_id);
      break;
    case control::unsubscribe:
      subscriber_list_.erase(msg.client_id);
      break;
    case control::resend:
      if(subscriber_list_.find(msg.client_id) != subscriber_list_.end())
      {
	send_all(msg.client_id);
      }
      break;
    default:
      break;
    }
  }



};

}
