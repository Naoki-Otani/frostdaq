// -*- C++ -*-

// Author: Tomonori Takahashi

#include "VmeCaenV830.hh"

#include <algorithm>

#include "std_ostream.hh"
#include "HexDump.hh"
#include "defines.hh"

namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type VmeCaenV830::k_type = "VmeCaenV830";

//______________________________________________________________________________
VmeCaenV830::VmeCaenV830(const unpacker_type& type)
  : VmeModule(type)
{
}

//______________________________________________________________________________
VmeCaenV830::~VmeCaenV830()
{
}

//______________________________________________________________________________
void
VmeCaenV830::check_data_format()
{
  uint32_t header = *m_module_data_first; 
  if ((header & k_header_identifier_bit)==0)
    m_error_state.set(defines::k_header_bit);
  
  return;
}

//______________________________________________________________________________
void
VmeCaenV830::decode()
{
  iterator_list::const_iterator f_begin = m_first_list.begin();
  iterator_list::const_iterator f_end   = m_first_list.end();
  for (iterator_list::const_iterator f  = f_begin;  f!=f_end; ++f)
    {
      iterator first = *f;
      m_vme_header = reinterpret_cast<VmeModule::Header*>(&*first);
      m_module_data_first = first + VmeModule::k_header_size;
      
      const uint32_t* buf 
	= reinterpret_cast<uint32_t*>(&*m_module_data_first) + 1;
      
      for (unsigned int ch=0; ch<k_n_ch; ++ch)
	fill(ch, 0, buf[ch]);

      uint32_t header = *m_module_data_first;
      fill(32, 0, (header & k_header_trigger_number_mask));
      fill(33, 0, (header & k_header_TS_mask)>>k_header_TS_shift);
      fill(34, 0, (header & k_header_NWORDS_mask)>>k_header_NWORDS_shift);
      fill(35, 0, ((header & k_header_identifier_bit)!=0));
      fill(36, 0, (header & k_header_GEO_address_mask)>>k_header_GEO_address_shift);
    }
  return;
}

//______________________________________________________________________________
void
VmeCaenV830::resize_fe_data()
{
  m_fe_data.resize(k_n_ch+5, FECh(k_n_data_type)); 
  return;
}

}
}
