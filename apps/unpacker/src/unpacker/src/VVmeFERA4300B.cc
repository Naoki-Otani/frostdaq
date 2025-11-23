// -*- C++ -*-

// Author: Tomonori Takahashi

#include "VVmeFERA4300B.hh"

#include <algorithm>

#include "cstdio"
#include "std_ostream.hh"
#include "HexDump.hh"
#include "defines.hh"

namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type VVmeFERA4300B::k_type = "VVmeFERA4300B";

//______________________________________________________________________________
VVmeFERA4300B::VVmeFERA4300B(const unpacker_type& type)
  : VmeModule(type)
{
}

//______________________________________________________________________________
VVmeFERA4300B::~VVmeFERA4300B()
{
}

//______________________________________________________________________________
void
VVmeFERA4300B::check_data_format()
{
  uint32_t header = *m_module_data_first;


  if (((header >> k_data_type_shift) & k_data_type_mask) != k_HEADER_MAGIC)
    m_error_state.set(defines::k_header_bit);

  return;
}

//______________________________________________________________________________
void
VVmeFERA4300B::decode()
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

      for (unsigned int i=0; i<data_size; i++) {
	uint32_t data_type = ((buf[i] >> k_data_type_shift) & k_data_type_mask);
	if (data_type == k_HEADER_MAGIC)
	  ;
	else if (data_type == k_DATA_MAGIC) {
	  uint32_t ch   = ((buf[i] >> k_ch_shift) & k_ch_mask);
	  uint32_t data = ((buf[i] >> k_data_shift) & k_data_mask);
	  //cout << "ch-- : " << ch << ", data : " << data << "\n";
	  fill(ch, 0, data);
	}
	else {
	  cerr << "VVmeFERA4300B::decode unknown data type : " << std::hex << data_type
	       << "(" << (buf[i]) << ")" << "\n" << std::dec;

	}
      }

    }
  return;
}

//______________________________________________________________________________
void
VVmeFERA4300B::resize_fe_data()
{
  m_fe_data.resize(k_n_ch);
  for(unsigned int i = 0; i<k_n_ch; ++i){
    m_fe_data[i].resize(k_n_data_type);
  }

  return;
}

}
}
