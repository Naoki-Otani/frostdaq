// -*- C++ -*-

// Author: Shuhei Hayakawa

#include "VmeEmc.hh"

#include <algorithm>

#include "std_ostream.hh"
#include "HexDump.hh"
#include "defines.hh"

namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type VmeEmc::k_type = "VmeEmc";

//______________________________________________________________________________
VmeEmc::VmeEmc(const unpacker_type& type)
  : VmeModule(type)
{
}

//______________________________________________________________________________
VmeEmc::~VmeEmc()
{
}

//______________________________________________________________________________
void
VmeEmc::check_data_format()
{
  uint32_t header = *m_module_data_first;
  if (((header>>k_word_type_shift)&k_word_type_mask) != k_header_magic)
    m_error_state.set(defines::k_header_bit);
  return;
}

//______________________________________________________________________________
void
VmeEmc::decode()
{
  iterator_list::const_iterator f_begin = m_first_list.begin();
  iterator_list::const_iterator f_end   = m_first_list.end();
  for (iterator_list::const_iterator f=f_begin;  f!=f_end; ++f) {
    iterator first = *f;
    m_vme_header = reinterpret_cast<VmeModule::Header*>(&*first);
    m_module_data_first = first + VmeModule::k_header_size;

    const uint32_t* buf = reinterpret_cast<uint32_t*>(&*m_module_data_first);
    unsigned int data_size = m_vme_header->m_data_size - VmeModule::k_header_size;

    for (unsigned int i=0; i<data_size; i++) {
      uint32_t word_type = ((buf[i]>>k_word_type_shift)&k_word_type_mask);
      switch(word_type){
      case k_header_magic:
	break;
      case k_serial_magic:
      case k_xpos_magic:
      case k_ypos_magic:
      case k_state_magic:
      case k_utime_magic:
      case k_ltime_magic:
	{
	  uint32_t data = ( (buf[i]>>k_data_shift) & k_data_mask );
	  fill(0, word_type, data);
	}
	break;
      case k_footer_magic:
	break;
      default:
	cerr<<"#E VmeEmc::decode() unknown word type: "
	    <<std::hex<<word_type<<" ("<<buf[i]<<")"<<std::endl<<std::dec;
      }//switch(word_type)
    }//for(i)
  }//for(f)
  return;
}

//______________________________________________________________________________
void
VmeEmc::resize_fe_data()
{
  m_fe_data.resize(k_n_channel);
  for(unsigned int i = 0; i<k_n_channel; ++i){
    m_fe_data[i].resize(k_n_data_type);
  }

  return;
}

}
}
