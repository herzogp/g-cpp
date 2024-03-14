#pragma once

#include <streambuf>
#include <unistd.h>

std::streamsize const input_area_size = 4000;
std::streamsize const output_area_size = 2000;

class Netbuf : public std::streambuf {
public:
  Netbuf(int socket);
  ~Netbuf() = default;

protected:
  bool send_output();

private:
  std::streambuf::int_type underflow();
  std::streamsize showmanyc();

  std::streambuf::int_type overflow(Netbuf::int_type ch);
  int sync();

  // disable copy and assignment
  Netbuf(const Netbuf &);
  Netbuf & operator= (const Netbuf &);

private:
  // open filedes
  int socket; 

  // dont let egptr() point to an address we might not own
  char input_area[input_area_size + 1]; 

  // dont let epptr() point to an address we might not own
  char output_area[output_area_size + 1];
};
