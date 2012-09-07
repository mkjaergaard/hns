#pragma once

#include <hns/fsm.hpp>
#include <hns/id.hpp>
#include <hns/distributed_vector_base.hpp>
#include <llog/logger.hpp>

namespace hns
{

template<typename T>
class distributed_vector_client : public fsm<distributed_vector_client<T> >, public distributed_vector_base
{
protected:
  typedef fsm<distributed_vector_client<T> > base;
  friend class fsm<distributed_vector_client<T> >;

  //******
  // Stuff
  std::vector<T> list_;

  ID server_location_id_;
  ID server_instance_id_;

  //**********
  // Events
  //**********
  struct ev_update_complete
  {
    update u;
    hns::shared_buffer data;
  };

  struct ev_update_partial
  {
    bool x;
    update u;
    hns::shared_buffer data;
  };

  struct ev_sub_req
  {
    ID location_id;
    ID instance_id;
  };

  //**********
  // States
  //**********
  typedef state<0> UNINIT;
  typedef state<1> PEND_SUBSCRIPTION;
  typedef state<2> UPDATED;
  typedef state<3> PEND_COMPLETE;

  //**********
  // Actions
  //**********
  void req_sub(const ev_sub_req& e)
  {
    hns::control ctrl;
    ctrl.command = hns::control::subscribe;
    ctrl.client_id = id_;

    hns::outbound_data<hns::boost_serializer, hns::control> o_ctrl(ctrl);

    server_location_id_ = e.location_id;
    server_instance_id_ = e.instance_id;

    send_to_location(e.location_id,
		     e.instance_id,
		     header::control,
		     o_ctrl);
  }

  void req_full()
  {
    hns::control ctrl;
    ctrl.command = hns::control::resend;
    ctrl.client_id = id_;

    hns::outbound_data<hns::boost_serializer, hns::control> o_ctrl(ctrl);

    send_to_location(server_location_id_,
		     server_instance_id_,
		     header::control,
		     o_ctrl);
  }

  void popu_partial(ev_update_partial& e)
  {
    inbound_data<boost_serializer, std::vector<T> > i_list(e.data);

    list_.insert(list_.begin(), i_list.get().begin(), i_list.get().end());
  }

  void popu_complete(ev_update_complete& e)
  {
    inbound_data<boost_serializer, std::vector<T> > i_list(e.data);

    list_.clear();
    list_.insert(list_.begin(), i_list.get().begin(), i_list.get().end());
  }

  void stash_partial(ev_update_partial& e)
  {
    std::cout << "player::start_playback\n";
  }

  //**********
  // Default Handler
  //**********
  void handle(const UNINIT&,
	      ev_sub_req& event)
  {
    llog::llog<llog::Severity::Trace>("UNINIT, sub_req");
    base::trans(PEND_SUBSCRIPTION());

    req_sub(event);
  }

  void handle(const PEND_SUBSCRIPTION&,
	      ev_update_complete& event)
  {
    llog::llog<llog::Severity::Trace>("P_S, u_c");

    popu_complete(event);
    base::trans(UPDATED());
  }

  void handle(const UPDATED&,
	      ev_update_partial& event)
  {
    llog::llog<llog::Severity::Trace>("UPDATED, ev_up");

    popu_partial(event);
  }

  void handle(const PEND_COMPLETE&,
	      ev_update_complete& event)
  {
    llog::llog<llog::Severity::Trace>("PEND_COMPLETE, ev_update");

    popu_complete(event);
    base::trans(UPDATED());
  }

  void handle(const PEND_COMPLETE&,
	      ev_update_partial& event)
  {
    llog::llog<llog::Severity::Trace>("PEND_COMPLETE, u_p");

    stash_partial(event);
  }

  template<typename State, typename Event>
  void handle(const State&, Event& event)
  {
    int state_copy = State::value;
    llog::llog<llog::Severity::Warning>(
      "Unhandled Event",
      "State", llog::Argument<int>(state_copy),
      "Event", llog::Argument<std::string>(typeid(Event).name()));
  }


public:
  //******
  // Public stuff
  void subscribe(const ID& loc, const ID& ins)
  {
    llog::llog<llog::Severity::Trace>("subscribe");

    ev_sub_req e;
    e.location_id = loc;
    e.instance_id = ins;

    base::post_event(e);
  }

  void recv(const header& hdr, hns::shared_buffer data)
  {
    assert(hdr.payload_type == header::update);

    inbound_data<boost_serializer, update> i_update(data);

    llog::llog<llog::Severity::Trace>(
      "recv",
      "type", llog::Argument<int>(i_update.get().type),
      "num", llog::Argument<int>(i_update.get().num_entries));

    switch(i_update.get().type)
    {
    case hns::update::complete:
    {
      ev_update_complete e;
      e.u = i_update.get();
      e.data = data;
      base::post_event(e);
    }
    break;
    case hns::update::partial:
    {
      ev_update_partial e;
      e.u = i_update.get();
      e.data = data;
      base::post_event(e);
    }
    break;
    }
  }

};

}
