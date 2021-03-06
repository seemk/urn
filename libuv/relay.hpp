#pragma once

/**
 * \file libuv/relay.hpp
 *
 * Notes:
 *  - No proper termination / cleanup
 *  - No maintenance invocations to relay
 */

#include <urn/intrusive_stack.hpp>
#include <urn/relay.hpp>
#include <uv.h>
#include <chrono>
#include <cstdlib>
#include <iostream>


//
// Linux:
//  - SO_REUSEPORT
//  - ethtool -n eth0 rx-flow-hash udp4
//  - ethtool -N eth0 rx-flow-hash udp4 sdfn
//  - watch "ethtool -S eth0 | grep 'rx_queue_[[:digit:]]*_packets'"
//  - watch "netstat -s --udp"
//  - htop
//


namespace urn_libuv {


constexpr bool have_mmsg = urn::is_linux_build;


inline void die_on_error (int code, const char *fn, const char *file, int line)
{
  if (code < 0)
  {
    std::cout
      << fn
      << ": "
      << uv_strerror(code)
      << " ("
      << uv_err_name(code)
      << ") at "
      << file
      << ':'
      << line
      << '\n';
    abort();
  }
}

#define libuv_call(F, ...) die_on_error(F(__VA_ARGS__), #F, __FILE__, __LINE__)


struct config //{{{1
{
  static constexpr std::chrono::seconds statistics_print_interval{5};

  struct
  {
    uint16_t port = 3478;
  } client{};

  struct
  {
    uint16_t port = 3479;
  } peer{};

  uint16_t threads;

  config (int argc, const char *argv[]);
};


struct libuv //{{{1
{
  using endpoint = sockaddr;
  struct packet;
  struct client;
  struct peer;
  struct session;
};


struct libuv::packet //{{{1
  : uv_buf_t
{
  packet () = default;

  packet (const uv_buf_t &buf, size_t len) noexcept
    : uv_buf_t(uv_buf_init(buf.base, (int)len))
  { }

  const std::byte *data () const noexcept
  {
    return reinterpret_cast<const std::byte *>(uv_buf_t::base);
  }

  size_t size () const noexcept
  {
    return uv_buf_t::len;
  }
};


struct libuv::client //{{{1
{
  void start_receive () noexcept
  { }
};


struct libuv::peer //{{{1
{
  void start_receive () noexcept
  { }
};


struct libuv::session //{{{1
{
  const endpoint client_endpoint;

  session (const endpoint &client_endpoint) noexcept
    : client_endpoint(client_endpoint)
  { }

  void start_send (const libuv::packet &packet) noexcept;
};


class relay //{{{1
{
public:

  relay (const urn_libuv::config &conf) noexcept;

  int run () noexcept;


  const urn_libuv::config &config () const noexcept
  {
    return config_;
  }


  const sockaddr &alloc_address () const noexcept
  {
    return alloc_address_;
  }


  void on_thread_start (uint16_t thread_index)
  {
    logic_.on_thread_start(thread_index);
  }


  void on_client_received (const libuv::endpoint &src, const libuv::packet &packet)
  {
    logic_.on_client_received(src, packet);
  }


  bool on_peer_received (const libuv::endpoint &src, libuv::packet &packet)
  {
    return logic_.on_peer_received(src, packet);
  }


  void on_session_sent (libuv::session &session, const libuv::packet &packet)
  {
    logic_.on_session_sent(session, packet);
  }


  void on_statistics_tick () noexcept
  {
    logic_.print_statistics(config_.statistics_print_interval);
  }


  static void alloc_buffer (uv_handle_t *, size_t, uv_buf_t *buf) noexcept;


private:

  libuv::client client_{};
  libuv::peer peer_{};

  const urn_libuv::config config_;
  const sockaddr alloc_address_;
  urn::relay<libuv, true> logic_;
};


} // namespace urn_libuv
