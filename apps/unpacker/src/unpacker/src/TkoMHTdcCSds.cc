// -*- C++ -*-

// Author: Tomonori Takahashi

#include "TkoMHTdcCSds.hh"

#include <bitset>

#include "defines.hh"
#include "std_ostream.hh"
#include "lexical_cast.hh"

namespace hddaq
{
  namespace unpacker
  {

    const defines::unpacker_type TkoMHTdcCSds::k_type = "TkoMHTdcCSds";
//     static const unsigned int k_Single_measurement_data 
//     = TkoMHTdc::k_Single_measurement_data;

//______________________________________________________________________________
TkoMHTdcCSds::TkoMHTdcCSds(const unpacker_type& type)
  : TkoMHTdc(type)
{
}

//______________________________________________________________________________
TkoMHTdcCSds::~TkoMHTdcCSds()
{
}

//______________________________________________________________________________
void
TkoMHTdcCSds::decode()
{
  unsigned int word_low = 0;
  int n_word = std::distance(m_data_first, m_data_last);
//   cout << "#D n word = " << n_word << std::endl;
  const_iterator first = m_data_first;
  const_iterator last  = m_data_last;
  
  for (const_iterator word = first; word!=last; ++word)
    {
      unsigned int data = *word;
      if (n_word==1 && ((data & k_SMP_DATA_mask)==0))
	{
	  fill(k_ch_trigger_error, k_trigger_error, 0);
	  continue;
	}
      
      
      if (get_packet_type(data)==k_Single_measurement_data)
	{
	  // case : word low
	  word_low = data & k_SMP_DATA_mask;
	}
      else
	{
	  // case : word high

	  const unsigned int packet_type   = get_packet_type(word_low);
	  const unsigned int tdc_id        = get_tdc_id(word_low);
	  const unsigned int tdc_ch        = get_tdc_ch(word_low); 
	  const unsigned int data_type     = get_data_type(word_low);
	  const int measurement_data = get_measurement_data(word_low, data);
	  
	  // tdc_id==0 is used as processor unit
	  unsigned int input_ch = get_input_ch(tdc_id, tdc_ch);
	  
// 	  cout << "#D " //<< m_type << "::decode() " << m_name << "\n"
// 	       << " data = " << std::hex << std::showbase
// 	       << ((word_low<<k_MSB_shift)| (data) & k_SMP_DATA_mask)
// 	       << std::dec << std::noshowbase
// 	       << std::endl;
	  
	  if (   (k_Single_measurement_data == packet_type)
		 && is_tdc_ch_valid(tdc_ch) 
		 && is_tdc_id_valid(tdc_id)) 
	    fill(input_ch, data_type, measurement_data);
	  else
	    {
	      fill(k_ch_invalid, k_invalid_packet_type, packet_type);
	      fill(k_ch_invalid, k_invalid_packet_tdc_ch, tdc_ch);
	      fill(k_ch_invalid, k_invalid_packet_tdc_id, tdc_id);
	      fill(k_ch_invalid, k_invalid_packet_data, measurement_data);
	      if (gm_check_mode[defines::k_format])
		m_error_state.set(defines::k_bad_bit);
	    }		  
	}
      ++m_word_count;
    }
  return;
}

  }
}
