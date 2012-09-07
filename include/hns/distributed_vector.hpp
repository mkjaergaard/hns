#pragma once

#include <vector>
#include <hns/distributed_vector_base.hpp>
#include <boost/asio/buffer.hpp>
#include <hns/id.hpp>
#include <hns/asio_buffer.hpp>
#include <hns/distributed_header.hpp>
#include <boost/serialization/vector.hpp>
#include <hns/outbound_data.hpp>

#include <llog/logger.hpp>

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

  void insert(const T& item)
  {
    buffer_.push_back(item);
  }

  void send_all(const client_id_type& client_id)
  {
    update msg;
    msg.type = update::complete;
    msg.num_entries = list_.size();
    msg.index = count_ + 10;

    outbound_data<boost_serializer, update> o_msg(msg);
    outbound_data<boost_serializer, std::vector<T> > o_list(list_);
    outbound_pair o_pair(o_msg, o_list);

    // send
    send_to_instance(client_id, header::update, o_pair);
  }

  void send_updates(const client_id_type& client_id)
  {
    update msg;
    msg.type = update::partial;
    msg.num_entries = list_.size();
    msg.index = count_ + 1;

    outbound_data<boost_serializer, update> o_msg(msg);
    outbound_data<boost_serializer, std::vector<T> > o_list(buffer_);
    outbound_pair o_pair(o_msg, o_list);

    // send
    send_to_instance(client_id, header::update, o_list);

  }

  void flush()
  {
    for(subscriber_list_type::iterator it = subscriber_list_.begin();
	it != subscriber_list_.end();
	it++)
    {
      send_updates(*it);
    }
    list_.insert(list_.end(), buffer_.begin(), buffer_.end());
    buffer_.clear();
    ++count_;
  }

  void recv(const header& hdr, hns::shared_buffer data)
  {
    assert(hdr.payload_type == header::control);

    inbound_data<boost_serializer, control> i_control(data);

    handle_control_message(i_control.get());
  }

  void handle_control_message(const control& msg)
  {
    llog::llog<llog::Severity::Trace>(
      "DistributedVector",
      "Command", llog::Argument<int>(msg.command));

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
