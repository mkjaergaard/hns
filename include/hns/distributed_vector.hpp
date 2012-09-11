#pragma once

#include <vector>
#include <hns/distributed_vector_base.hpp>
#include <boost/asio/buffer.hpp>
#include <hns/id.hpp>
#include <hns/asio_buffer.hpp>
#include <hns/distributed_header.hpp>
#include <boost/serialization/set.hpp>
#include <hns/outbound_data.hpp>

#include <llog/logger.hpp>

namespace hns
{

template<typename T>
class distributed_vector : public distributed_vector_base
{
protected:
  typedef std::set<T> list_type;
  typedef std::pair<typename std::set<T>::iterator, bool> insert_type;

  list_type list_;
  list_type added_list_;
  list_type removed_list_;

  uint32_t state_index_;
  uint32_t last_update_sent_;

  typedef std::set<client_id_type> subscriber_list_type;
  subscriber_list_type subscriber_list_;

  server_id_type id_;

public:
  distributed_vector() :
    state_index_(0),
    last_update_sent_(0)
  {
  }

  void insert(const T& item)
  {
    insert_type result = list_.insert(item);
    if(result.second)
    {
      if(removed_list_.erase(item) == 0)
      {
	added_list_.insert(item);
      }
      state_index_++;
    }
  }

  void erase(const T& item)
  {
    if(list_.erase(item) > 0)
    {
      if(added_list_.erase(item) == 0)
      {
	removed_list_.insert(item);
      }
      state_index_++;
    }
  }

  void send_all(const client_id_type& client_id)
  {
    update msg;
    msg.type = update::complete;
    msg.num_entries = list_.size();
    msg.start_index = 0;
    msg.end_index = state_index_;

    outbound_data<boost_serializer, update> o_msg(msg);
    outbound_data<boost_serializer, std::set<T> > o_list(list_);
    outbound_pair o_pair(o_msg, o_list);

    // send
    send_to_instance(client_id, header::update, o_pair);
  }

  void send_updates(const client_id_type& client_id)
  {
    update msg;
    msg.type = update::partial;
    msg.num_entries = list_.size();
    msg.start_index = last_update_sent_;
    msg.end_index = state_index_;

    outbound_data<boost_serializer, update> o_msg(msg);
    outbound_data<boost_serializer, std::set<T> > o_list1(added_list_);
    outbound_data<boost_serializer, std::set<T> > o_list2(removed_list_);
    outbound_pair o_list(o_list1, o_list2);
    outbound_pair o_pair(o_msg, o_list);

    // send
    send_to_instance(client_id, header::update, o_pair);
  }

  void flush()
  {
    if(state_index_ > last_update_sent_)
    {
      for(subscriber_list_type::iterator it = subscriber_list_.begin();
	  it != subscriber_list_.end();
	  it++)
      {
	send_updates(*it);
      }
      last_update_sent_ = state_index_;
      added_list_.clear();
      removed_list_.clear();
    }
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
