// -*- C++ -*-

// Author: Koji Miwa for E13

#include "VmeUmem.hh"

#include <algorithm>

#include "std_ostream.hh"
#include "HexDump.hh"
#include "Unpacker.hh"
#include "lexical_cast.hh"
#include "defines.hh"

namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type VmeUmem::k_type = "VmeUmem";

//______________________________________________________________________________
VmeUmem::VmeUmem(const unpacker_type& type)
  : VmeModule(type), m_fera(k_n_fera)
{
}

//______________________________________________________________________________
VmeUmem::~VmeUmem()
{
}

//______________________________________________________________________________

bool
VmeUmem::set_fera_data(Unpacker* u,
		     unsigned int ma,
		     const iterator& data_first,
		     const iterator& data_last)
{
  //  cout << "#D " << m_name << " ::set_fera_data()" << std::endl;

  //  std::for_each(data_first, data_last, HexDump());
    
  if (!u)
    {
      cerr << "\n#E " << k_type << "::unpack()\n" << m_name
	   << " got unknown data of FERA MA = " 
	   << ma << ", data : ";
      std::for_each(data_first, data_last, HexDump());
      return false;
    }

  u->set_data_size(std::distance(data_first, data_last));
  u->set_data(data_first, data_last);

  u->unpack();
  return true;
}

//______________________________________________________________________________
void
VmeUmem::sort_child()
{
  for (int i=0; i<k_n_fera; ++i)
    m_fera[i] = m_child_list[i];
  return;
}

//______________________________________________________________________________
bool
VmeUmem::unpack()
{
//   hoge();
  VmeModule::unpack();

  m_vme_header = reinterpret_cast<VmeModule::Header*>(&*m_data_first);
  unsigned int NofEvent = m_vme_header->m_data_size - VmeModule::k_header_size;

  m_fera_begin = m_data_first+VmeModule::k_header_size;
  m_fera_end   = m_data_first+VmeModule::k_header_size+NofEvent;


  iterator first = m_fera_begin;
  iterator last  = m_fera_end;
  iterator itr   = first;

  const std::vector<Unpacker*>& fera = m_fera;
  unsigned int vsn = 0U;
  int header_num   = 0;
  bool ret         = false;

  int count_header[k_n_fera];
  for (int i=0; i<k_n_fera; i++)
    count_header[i]=0;
  bool multi_event_flag=false;

  for (; itr!=last; ++itr)
    {
      //cout << std::hex << *itr << "\n" << std::dec;
      int wordtype = (*itr >> k_bitshift_wordtype) & 0x1;      
      if (1 == wordtype) {
	// Header
	if (header_num != 0) {
	  if(false == (ret = set_fera_data(fera[vsn], vsn, first, itr))){
	    cerr << "#E VmeUmem::unpack this event data is broken. exit unpack." << std::endl;
	    break;
	  }
	  first = itr;
	}
	vsn = *itr & k_VSN_MASK;
	header_num++;

	if (count_header[vsn] != 0) {
	  multi_event_flag=true;
	}
	count_header[vsn]++;
      }
      if (itr == last-1) {
	if(false == (ret = set_fera_data(fera[vsn], vsn, first, itr+1))){
	  cerr << "#E VmeUmem::unpack this event data is broken. exit unpack." << std::endl;
	  break;
	}
      }
    }

  if (multi_event_flag) {
    cout << "#E VmeUmem::unpack, two events are stored\n";
    std::for_each(m_fera_begin, last, HexDump());
  }
  return ret;
}

//______________________________________________________________________________
void
VmeUmem::decode()
{
  return;
}

//______________________________________________________________________________
void
VmeUmem::resize_fe_data()
{
  return;
}

}
}
