// -*- C++ -*-

// Author: Koji Miwa for E13

#include "Fera3377.hh"

#include <algorithm>

#include "std_ostream.hh"
#include "HexDump.hh"
#include <stdio.h>
namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type Fera3377::k_type = "Fera3377";

//______________________________________________________________________________
Fera3377::Fera3377(const unpacker_type& type)
  : UnpackerImpl(type)
{
}

//______________________________________________________________________________
Fera3377::~Fera3377()
{
}

//______________________________________________________________________________
void
Fera3377::decode()
{
  const uint32_t* buf = reinterpret_cast<uint32_t*>(&*m_data_first);

  unsigned int NofEvent = m_data_size;
  // unsigned int vsn=0;
  int wordtype;
  //std::cout << "------------3377------\n" ;
  for (unsigned int i=0; i<NofEvent; i++) {
    //std::cout << i << " : " << std::hex << buf[i] << std::endl << std::dec;
    wordtype = (buf[i]>>k_bitshift_wordtype) & 0x1;
    if (1 == wordtype) {
      // Header
      // vsn = buf[i] & k_VSN_MASK;
    } else {
      //std::cout << i+1 << " : " << std::hex << buf[i+1] << std::endl << std::dec;
      // Data
      // First Data
      uint32_t first_data = buf[i];
      uint32_t second_data;

      if (i+1 < NofEvent)
	second_data = buf[i+1];
      else {
	cout << "#E Fera3377::decode  second data of 3377 over DataSize i+1 = " << i+1 << ", NofEvent = " << NofEvent << "\n";
	for (unsigned int j=0; j<NofEvent; j++) {
	  std::cout << j << " : " << std::hex << buf[j] << std::endl << std::dec;
	}

	return;
      }

      // Consistency check of First data and Second data
      unsigned int msb_byte = (first_data >> k_bitshift_FS) & 0x1;
      unsigned int lsb_byte = (second_data >> k_bitshift_FS) & 0x1;
      if (msb_byte != 1 || lsb_byte != 0) {
	cout << "#E Fera3377::decode  MSB or LSB is different 1st_data " << std::hex << first_data << ", 2nd_data " << second_data << "\n" << std::dec;
	return;
      }
      
      unsigned int first_ch  = (first_data >> k_bitshift_channel) & k_CHANNEL_MASK;
      unsigned int second_ch = (second_data >> k_bitshift_channel) & k_CHANNEL_MASK;
      if (first_ch != second_ch) {
	cout << "#E Fera3377::decode  ch(1st_data) and ch(2nd_data) is different" << std::hex << first_data << ", 2nd_data " << second_data << "\n" << std::dec;
	return;
      }

      unsigned int first_lt  = (first_data >> k_bitshift_RT) & 0x1;
      unsigned int second_lt = (second_data >> k_bitshift_RT) & 0x1;

      if (first_lt != second_lt) {
	cout << "#E Fera3377::decode  LT(1st_data) and LT(2nd_data) is different" << std::hex << first_data << ", 2nd_data " << second_data << "\n" << std::dec;
	getchar();
	return;
      }

      unsigned int data_msb  = first_data & k_DATA_MASK;
      unsigned int data_lsb = second_data & k_DATA_MASK;
      
      unsigned int data        = (data_msb << 8) | data_lsb;
      
      fill(first_ch, first_lt, data);
      
      i++; // for second data 
    }
  }

  return;
}

//______________________________________________________________________________
void
Fera3377::resize_fe_data()
{
  m_fe_data.resize(k_n_ch);
  for (unsigned int i=0; i<k_n_ch; ++i) {
    m_fe_data[i].resize(k_n_data_type);
  }

  return;
}

}
}
