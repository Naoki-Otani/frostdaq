// -*- C++ -*-

// Author: Koji Miwa for E13

#include "VmeAmt.hh"

#include <algorithm>

#include "std_ostream.hh"
#include "HexDump.hh"

namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type VmeAmt::k_type = "VmeAmt";

//______________________________________________________________________________
VmeAmt::VmeAmt(const unpacker_type& type)
  : VmeModule(type)
{
}

//______________________________________________________________________________
VmeAmt::~VmeAmt()
{
}

//______________________________________________________________________________
void
VmeAmt::decode()
{
  iterator_list::const_iterator f_begin = m_first_list.begin();
  iterator_list::const_iterator f_end   = m_first_list.end();
  for (iterator_list::const_iterator f = f_begin;  f!=f_end; ++f)
    {
      iterator first = *f;
      m_vme_header = reinterpret_cast<VmeModule::Header*>(&*first);
      m_module_data_first = first + VmeModule::k_header_size;
      
      const uint32_t* buf = reinterpret_cast<uint32_t*>(&*m_module_data_first);

      unsigned int data_size = m_vme_header->m_data_size;

      // Header check
      int wordtype = (buf[0]>>k_bitshift_wordtype) & 0x7;
      if (5 != wordtype) {
	cout << "#E VmeAmt : Header error " << std::hex << buf[0] << "\n";
	cout << std::dec;
	return;
      }
      unsigned int NofEvent = (buf[0] >> k_bitshift_tntotal) & k_TNTOTAL_MASK;
      if (NofEvent != data_size-VmeModule::k_header_size) {
	cout << "#E VmeAmt : NofEvent is NOT consistent  "<< "\n";
	return;
      }

      for (unsigned int i=1; i<NofEvent; i++) {
	wordtype = (buf[i]>>k_bitshift_wordtype) & 0x7;

	if (5 == wordtype) {
	  cout << "#E VmeAmt : Header again buf[ " << i << "] : " << std::hex << buf[i] << "\n";
	} else if (0 == wordtype) {
	  // AMT data
	  unsigned int ch    = (buf[i] >> k_bitshift_ch) & k_CHANNEL_MASK;
	  unsigned int ltbit = (buf[i] >> k_bitshift_lt) & k_LT_MASK;
	  unsigned int tdc   = buf[i] & k_TDC_MASK;

	  if (k_leading == ltbit) {
	    fill(ch, k_leading, tdc);
	  } else {
	    fill(ch, k_trailing, tdc);
	  }
	} else if (3 == wordtype) {
	  // AMT error
	  cout << "#E VmeAmt : AMT ERROR " << std::hex << buf[i] << "\n" << std::dec;
	} 
      }

    }
  return;
}

//______________________________________________________________________________
void
VmeAmt::resize_fe_data()
{
  m_fe_data.resize(k_n_ch);
  for (unsigned int i=0; i<k_n_ch; ++i) {
    m_fe_data[i].resize(k_n_data_type);
  }

  return;
}

}
}
