// -*- C++ -*-

// Author: Tomonori Takahashi

#include "VVmeCoinReg.hh"

#include <algorithm>

#include "std_ostream.hh"
#include "HexDump.hh"
#include "defines.hh"

namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type VVmeCoinReg::k_type = "VVmeCoinReg";

//______________________________________________________________________________
VVmeCoinReg::VVmeCoinReg(const unpacker_type& type)
  : VmeModule(type)
{
}

//______________________________________________________________________________
VVmeCoinReg::~VVmeCoinReg()
{
}

//______________________________________________________________________________
void
VVmeCoinReg::check_data_format()
{
  //uint32_t header = *m_module_data_first; 

  //if (((header >> k_data_type_shift) & k_data_type_mask) != k_HEADER_MAGIC)
  //m_error_state.set(defines::k_header_bit);
  
  return;
}

//______________________________________________________________________________
void
VVmeCoinReg::decode()
{
  iterator_list::const_iterator f_begin = m_first_list.begin();
  iterator_list::const_iterator f_end   = m_first_list.end();
  for (iterator_list::const_iterator f  = f_begin;  f!=f_end; ++f)
    {
      iterator first = *f;
      m_vme_header = reinterpret_cast<VmeModule::Header*>(&*first);
      m_module_data_first = first + VmeModule::k_header_size;

      const uint32_t* buf = reinterpret_cast<uint32_t*>(&*m_module_data_first);

      unsigned int data_size = m_vme_header->m_data_size-VmeModule::k_header_size;;

      if (data_size == 1) {
	  uint32_t data = ((buf[0] >> k_data_shift) & k_data_mask);
	  //cout << "CoinReg data : " << data << "\n";	  
	  for (unsigned int i=0; i<k_n_ch; i++) {
	    uint32_t hitflag=(data>>i) & 0x1;
	    if (hitflag == 1) {
	      fill(i, 0, data);
	    }
	  }
	}
      else {
	for  (unsigned int i=0; i<data_size; i++) {
	  std::cerr << "VVmeCoinReg::decode unknown data type : " << std::hex 
		    << "(" << (buf[i]) << ")" << "\n" << std::dec;
	
	}
      }
    }
  return;
}

//______________________________________________________________________________
void
VVmeCoinReg::resize_fe_data()
{
  m_fe_data.resize(k_n_ch);
  for(unsigned int i = 0; i<k_n_ch; ++i){
    m_fe_data[i].resize(k_n_data_type);
  }

  return;
}

}
}
