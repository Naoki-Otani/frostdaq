// -*- C++ -*-

// Author: Koji Miwa for E13

#include "VmeUmemAD413A.hh"

#include <algorithm>

#include "std_ostream.hh"
#include "HexDump.hh"

namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type VmeUmemAD413A::k_type = "VmeUmemAD413A";

//______________________________________________________________________________
VmeUmemAD413A::VmeUmemAD413A(const unpacker_type& type)
  : VmeModule(type)
{
}

//______________________________________________________________________________
VmeUmemAD413A::~VmeUmemAD413A()
{
}

//______________________________________________________________________________
void
VmeUmemAD413A::decode()
{
  //iterator_list::const_iterator f_begin = m_first_list.begin();
  //iterator_list::const_iterator f_end   = m_first_list.end();
  //for (iterator_list::const_iterator f = f_begin;  f!=f_end; ++f)
    {
      //iterator first = *f;
      iterator first = *(m_first_list.begin());
      m_vme_header = reinterpret_cast<VmeModule::Header*>(&*first);
      m_module_data_first = first + VmeModule::k_header_size;
      
      const uint32_t* buf = reinterpret_cast<uint32_t*>(&*m_module_data_first);

      unsigned int NofEvent = m_vme_header->m_data_size - VmeModule::k_header_size;
      unsigned int vsn=0;
      int wordtype;
      for (unsigned int i=0; i<NofEvent; i++) {
	wordtype = (buf[i]>>k_bitshift_wordtype) & 0x1;
	if (1 == wordtype) {
	  // Header
	  vsn = buf[i] & k_VSN_MASK;
	} else {
	  // Data
	  unsigned int  subaddr = (buf[i] >> k_bitshift_subaddr) & k_SUBADDR_MASK;
	  unsigned int  adc = buf[i] & k_DATA_MASK;
	  unsigned int  ch  = (vsn << 2) | subaddr;
	  
	  fill(ch, 0, adc);
	}
      }
    }
  return;
}

//______________________________________________________________________________
void
VmeUmemAD413A::resize_fe_data()
{
  m_fe_data.resize(k_n_ch);
  for (unsigned int i=0; i<k_n_ch; ++i) {
    m_fe_data[i].resize(k_n_data_type);
  }

  return;
}

}
}
