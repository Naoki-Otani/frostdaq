// -*- C++ -*-

// Author: S.H. Hayakawa

#include "VmeInputReg.hh"

#include <algorithm>
#include <string>
#include <sstream>

#include "std_ostream.hh"
#include "defines.hh"
#include "Unpacker.hh"

namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type VmeInputReg::k_type = "VmeInputReg";

//______________________________________________________________________________
VmeInputReg::VmeInputReg(const unpacker_type& type)
  : VmeModule(type)
{
}

//______________________________________________________________________________
VmeInputReg::~VmeInputReg()
{
}

//______________________________________________________________________________
void
VmeInputReg::check_data_format()
{
  VmeModule::check_data_format();
  return;
}

//______________________________________________________________________________
void
VmeInputReg::decode()
{
  iterator_list::const_iterator f_begin = m_first_list.begin();
  iterator_list::const_iterator f_end   = m_first_list.end();
  for (iterator_list::const_iterator f = f_begin; f!=f_end; ++f) {
    iterator first = *f;
    m_vme_header = reinterpret_cast<VmeModule::Header*>(&*first);
    m_module_data_first = first + VmeModule::k_header_size;
    
    const uint32_t* buf = reinterpret_cast<uint32_t*>(&*m_module_data_first);
    unsigned int data_size = m_vme_header->m_data_size - VmeModule::k_header_size;
    
    for (unsigned int i=0; i<data_size; i++) {
      for(unsigned int j=0; j<k_n_ch; j++) {
	uint32_t ch   = j & k_channel_mask;
	uint32_t data = ((buf[i]>>j) & k_data_mask);
	fill(ch, k_reset_flag, data);
      }//for(j)
    }//for(i)
  }//for(f)
  return;
}

//______________________________________________________________________________
void
VmeInputReg::resize_fe_data()
{
  m_fe_data.resize(k_n_ch, FECh(k_n_data));
  return;
}

}
}
