// -*- C++ -*-

// Author: Koji Miwa for E13

#include "VmeUmem3377.hh"

#include <algorithm>

#include "std_ostream.hh"
#include "HexDump.hh"

namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type VmeUmem3377::k_type = "VmeUmem3377";

//______________________________________________________________________________
VmeUmem3377::VmeUmem3377(const unpacker_type& type)
  : VmeModule(type)
{
}

//______________________________________________________________________________
VmeUmem3377::~VmeUmem3377()
{
}

//______________________________________________________________________________
void
VmeUmem3377::decode()
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
      // unsigned int vsn=0;
      int wordtype;
      for (unsigned int i=0; i<NofEvent; i++) {
	wordtype = (buf[i]>>k_bitshift_wordtype) & 0x1;
	if (1 == wordtype) {
	  // Header
	  // vsn = buf[i] & k_VSN_MASK;
	} else {
	  // Data
	  // First Data
	  uint32_t first_data = buf[i];
	  uint32_t second_data;

	  if (i+1 < NofEvent)
	    second_data = buf[i];
	  else {
	    cout << "#E VmeUmem3377::decode  second data of 3377 over DataSize i+1 = " << i+1 << ", NofEvent = " << NofEvent << "\n";
	    return;
	  }

	  // Consistency check of First data and Second data
	  unsigned int msb_byte = (first_data >> k_bitshift_FS) & 0x1;
	  unsigned int lsb_byte = (second_data >> k_bitshift_FS) & 0x1;
	  if (msb_byte != 1 || lsb_byte != 0) {
	    cout << "#E VmeUmem3377::decode  MSB or LSB is different 1st_data " << std::hex << first_data << ", 2nd_data " << second_data << "\n" << std::dec;
	    return;
	  }

	  unsigned int first_ch  = (first_data >> k_bitshift_channel) & k_CHANNEL_MASK;
	  unsigned int second_ch = (second_data >> k_bitshift_channel) & k_CHANNEL_MASK;
	  if (first_ch != second_ch) {
	    cout << "#E VmeUmem3377::decode  ch(1st_data) and ch(2nd_data) is different" << std::hex << first_data << ", 2nd_data " << second_data << "\n" << std::dec;
	    return;
	  }

	  unsigned int first_lt  = (first_data >> k_bitshift_RT) & 0x1;
	  unsigned int second_lt = (second_data >> k_bitshift_RT) & 0x1;

	  if (first_lt != second_lt) {
	    cout << "#E VmeUmem3377::decode  LT(1st_data) and LT(2nd_data) is different" << std::hex << first_data << ", 2nd_data " << second_data << "\n" << std::dec;
	    return;
	  }

	  unsigned int data_msb  = first_data & k_DATA_MASK;
	  unsigned int data_lsb = second_data & k_DATA_MASK;

	  unsigned int data        = (data_msb << 8) | data_lsb;
	  
	  fill(first_ch, first_lt, data);
	}
      }
    }
  return;
}

//______________________________________________________________________________
void
VmeUmem3377::resize_fe_data()
{
  m_fe_data.resize(k_n_ch);
  for (unsigned int i=0; i<k_n_ch; ++i) {
    m_fe_data[i].resize(k_n_data_type);
  }

  return;
}

}
}
