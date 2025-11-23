// -*- C++ -*-

// Author: Tomonori Takahashi

#include "BitDump.hh"

#include <bitset>

#include "std_ostream.hh"

namespace hddaq
{

//______________________________________________________________________________
BitDump::BitDump()
  : m_count(0)
{
  cout << "#D n-th  : data (MSB --- LSB)" << std::hex << std::endl;
}

//______________________________________________________________________________
BitDump::~BitDump()
{
  cout << std::dec << std::endl;
}

//______________________________________________________________________________
void
BitDump::operator()(unsigned int data)
{
  std::bitset<32> bits(data);
//   cout << std::hex;
  cout.width(8);
  cout.fill('0');
  cout << m_count << " : ";// << std::dec;

  cout << bits << "   ";
  cout.width(8);
  cout.fill('0');
  cout << data << "\n";
  ++m_count;
  return;
}

//______________________________________________________________________________
void
BitDump::operator()(unsigned short data)
{
  std::bitset<16> bits(data);
  cout.width(8);
  cout.fill('0');
  cout << m_count << " : ";

  cout << bits << "   ";
  cout.width(4);
  cout.fill('0');
  cout << data << "\n";
  return;
}


}

