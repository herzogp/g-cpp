#include <sys/socket.h>
#include <cstdio>
#include "netbuf.h"

Netbuf::Netbuf(int socket) : socket(socket), input_area{0}, output_area{0} {
  // input_area = {0};
  // output_area = {0};
  char *input_end = input_area + input_area_size;
  setg(input_end, input_end, input_end);

  char *output_start = output_area;
  char *output_end = output_area + (output_area_size - 1); // leave a little extra for overflow()
  setp(output_start, output_end);
}

std::streambuf::int_type 
Netbuf::underflow() {
  if (gptr() < egptr()) {
    return traits_type::to_int_type(*gptr());
  }

  char *start = this->input_area;
  size_t count = input_area_size;
  ssize_t nbx = ::read(this->socket, start, count);
  if (nbx <= 0) {
    return traits_type::eof();
  }

  setg(start, start, start + nbx);
  return traits_type::to_int_type(*gptr());
 }
 
std::streamsize 
Netbuf::showmanyc() {
  //return static_cast<std::streamsize>(egptr() - gptr());
  return egptr() - gptr();
}

std::streambuf::int_type
Netbuf::overflow(Netbuf::int_type ch) {
  if (ch != traits_type::eof()) {
    *pptr() = ch;
    pbump(1);
    bool did_send = send_output();
    if (did_send) {
      setp(output_area, output_area + (output_area_size - 1));
      return ch;
    }
  }
  return traits_type::eof();
}

int 
Netbuf::sync() {
  bool did_send = send_output();
  return did_send ? 0 : -1;
}

bool
Netbuf::send_output() {
  char *start = pbase();
  int bytes_sent = 0;
  int bytes_remaining = epptr() - pbase();
  while (bytes_remaining > 0) {
    int nbx = ::send(socket, start + bytes_sent, bytes_remaining, 0);
    if (nbx < 0) {
      return false;
    }
    bytes_sent += nbx;
    bytes_remaining -= nbx;
  }
  return true;
}
