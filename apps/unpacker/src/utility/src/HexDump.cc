// -*- C++ -*-

// Author: Tomonori Takahashi

#include "HexDump.hh"

// #include <iostream>
#include "std_ostream.hh"

namespace hddaq
{

//______________________________________________________________________________
HexDump::HexDump()
  : m_count(0)
{
  cout << std::hex;
}

//______________________________________________________________________________
HexDump::~HexDump()
{
  cout << std::dec << std::endl;
}

//______________________________________________________________________________
void 
HexDump::operator()(unsigned int data)
{
  if (0 == m_count)
    {
      cout.width(8);
      cout.fill('0');
      cout << m_count;
      cout << " : ";
    }
  cout.width(8);
  cout.fill('0');
  cout << data << " ";
  ++m_count;
  if (0 == m_count%8) 
    {
      cout << "\n";
      cout.width(8);
      cout.fill('0');
      cout <<  m_count;
      cout << " : ";
    }
  return;
}


//______________________________________________________________________________
void 
HexDump::operator()(unsigned short data)
{
  if (0 == m_count)
    {
      cout.width(8);
      cout.fill('0');
      cout << m_count;
      cout << " : ";
    }
  cout.width(4);
  cout.fill('0');
  cout << data << " ";
  ++m_count;
  if (0 == m_count%8) 
    {
      cout << "\n";
      cout.width(8);
      cout.fill('0');
      cout <<  m_count;
      cout << " : ";
    }
  return;
}

}
