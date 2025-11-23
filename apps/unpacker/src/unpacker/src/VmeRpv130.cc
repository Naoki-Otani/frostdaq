// -*- C++ -*-

// Author: Tomonori Takahashi

#include "VmeRpv130.hh"

#include <algorithm>

#include "std_ostream.hh"
#include "HexDump.hh"

namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type VmeRpv130::k_type = "VmeRpv130";

//______________________________________________________________________________
VmeRpv130::VmeRpv130(const unpacker_type& type)
  : VmeModule(type)
{
}

//______________________________________________________________________________
VmeRpv130::~VmeRpv130()
{
}

//______________________________________________________________________________
void
VmeRpv130::decode()
{
  
  iterator_list::const_iterator f_begin = m_first_list.begin();
  iterator_list::const_iterator f_end   = m_first_list.end();
  for (iterator_list::const_iterator f = f_begin; f!=f_end; ++f)
    {
      iterator first = *f;
      m_vme_header = reinterpret_cast<VmeModule::Header*>(&*first);
      m_module_data_first = first + VmeModule::k_header_size;
      
      const uint32_t* buf = reinterpret_cast<uint32_t*>(&*m_module_data_first);
      for (int data_type=0; data_type<k_n_data_type; ++data_type)
	{
	  const unsigned int data  = *buf & k_data_mask;
	  for (unsigned int ch=0; ch<k_n_ch; ++ch)
	    {
	      if (((data>>ch) & 1) == 1)
		fill(ch, data_type, 1);
	    }
	}
    }
  return;
}

//______________________________________________________________________________
const std::map<std::string, int>&
VmeRpv130::get_data_id() const
{
  static std::map<std::string, int> s_data_ref;
  if (s_data_ref.empty())
    {
      s_data_ref["latch1"]   = k_latch1;
      s_data_ref["latch2"]   = k_latch2;
      s_data_ref["flipflop"] = k_flipflop;
      s_data_ref["through"]  = k_through;
    }
  return s_data_ref; 
}

//______________________________________________________________________________
void
VmeRpv130::resize_fe_data()
{
  m_fe_data.resize(k_n_ch, FECh(k_n_data_type));
  return;
}

}
}
