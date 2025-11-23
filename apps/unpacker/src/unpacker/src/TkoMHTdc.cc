// -*- C++ -*-

// Author: Tomonori Takahashi

#include "TkoMHTdc.hh"

#include <bitset>

#include "defines.hh"
#include "std_ostream.hh"
#include "lexical_cast.hh"

namespace hddaq
{
  namespace unpacker
  {

    const defines::unpacker_type TkoMHTdc::k_type = "TkoMHTdc";

//______________________________________________________________________________
TkoMHTdc::TkoMHTdc(const unpacker_type& type)
  : Tko(type)
{
}

//______________________________________________________________________________
TkoMHTdc::~TkoMHTdc()
{
}

//______________________________________________________________________________
void
TkoMHTdc::decode()
{
  unsigned int word_low = 0;
  const_iterator first = m_data_first;
  const_iterator last  = m_data_last;
  for (const_iterator word = first; word!=last; ++word)
    {
      unsigned int data = *word;
      if (m_word_count%2==0)
	{
	  // case : word low
	  word_low = data & k_SMP_DATA_mask;
	}
      else
	{
	  // case : word high
	  const unsigned int packet_type = get_packet_type(word_low);
	  const unsigned int tdc_id      = get_tdc_id(word_low);
	  const unsigned int tdc_ch      = get_tdc_ch(word_low);
	  const unsigned int data_type   = get_data_type(word_low);
	  
	  const int measurement_data = get_measurement_data(word_low, data);
	  
	  // tdc_id==0 is used as processor unit
	  unsigned int input_ch = get_input_ch(tdc_id, tdc_ch);
	  
// 	  cout << "#D " << m_type << "::decode() " << m_name << "\n"
// 	       << " data = " << std::hex << std::showbase
// 	       << (word_low|data)
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
  if (1U==m_word_count)
    {
      const unsigned int measurement_data = word_low & k_SMP_DATA_mask;
      fill(k_ch_trigger_error, k_trigger_error, measurement_data);
    }

      return;
}

//______________________________________________________________________________
void
TkoMHTdc::dump_in_binary() const
{
  cout << "#D " << m_type << " " << m_name << "\n";
  cout << "#D  n-th : p   | id | ch  |te| data"  << std::hex <<std::endl;
  int count=0;
  int msb=0;
  const_iterator first = m_data_first;
  const_iterator last  = m_data_last;
  for (const_iterator i =first; i!=last; ++i)
    {
      if (count%2==0)
	{
	  const std::bitset<4> packet_type(get_packet_type(*i));
	  const std::bitset<4> tdc_id((*i & k_TDC_ID_mask) >> k_TDC_ID_shift);
	  const std::bitset<5> tdc_ch((*i & k_TDC_CH_mask) >> k_TDC_CH_shift);
	  const std::bitset<2> data_type
	    ((*i & k_DATA_TYPE_mask)>>k_DATA_TYPE_shift);

	  msb = (*i & k_MSB_mask);

	  cout.width(8);
	  cout.fill('0');
	  cout << (count/2) << " : ";
	  
	  cout << packet_type << " "
	       << tdc_id << " "
	       << tdc_ch << " "
	       << data_type << " ";
	  //	       << msb;
	}
      else
	{
	  const unsigned int measurement_data 
	    = (msb << k_MSB_shift) | (*i & k_SMP_DATA_mask);
	  std::bitset<17> data(measurement_data);
	  cout << data << "   ";
	  cout.width(9);
	  cout.fill('0');
	  cout << measurement_data << "\n";
	}
      ++count;
    }
  cout << std::endl;
  return;
}

//______________________________________________________________________________
const std::map<std::string, int>&
TkoMHTdc::get_data_id() const
{
  static std::map<std::string, int> s_data_ref;
  if (s_data_ref.empty())
    {
      s_data_ref["trailing"]              = k_trailing;
      s_data_ref["trailing_error"]        = k_trailing_error;
      s_data_ref["leading"]               = k_leading;
      s_data_ref["leading_error"]         = k_leading_error;
      s_data_ref["invalid_packet_type"]   = k_invalid_packet_type;
      s_data_ref["invalid_packet_tdc_ch"] = k_invalid_packet_tdc_ch;
      s_data_ref["invalid_packet_tdc_id"] = k_invalid_packet_tdc_id;
      s_data_ref["invalid_packet_data"]   = k_invalid_packet_data;
      s_data_ref["trigger_error"]         = k_trigger_error;
    }
  return s_data_ref;
}

//______________________________________________________________________________
unsigned int
TkoMHTdc::get_data_type(unsigned int word_low) const
{
  return ((word_low & k_DATA_TYPE_mask) >> k_DATA_TYPE_shift);
}

//______________________________________________________________________________
int
TkoMHTdc::get_input_ch(unsigned int tdc_id,
		       unsigned int tdc_ch) const
{
  return (k_N_TDC_CH -1 - tdc_ch + k_N_TDC_CH * (tdc_id - 1U));
}

//______________________________________________________________________________
unsigned int
TkoMHTdc::get_measurement_data(unsigned int word_low,
			       unsigned int word_high) const 
{
  return (((word_low & k_MSB_mask) << k_MSB_shift) 
	  | (word_high & k_SMP_DATA_mask));
}

//______________________________________________________________________________
unsigned int
TkoMHTdc::get_packet_type(unsigned int word_low) const
{
  return (word_low & k_PACKET_TYPE_mask) >> k_PACKET_TYPE_shift;
}

//______________________________________________________________________________
unsigned int
TkoMHTdc::get_tdc_ch(unsigned int word_low) const
{
  return ((word_low & k_TDC_CH_mask) >> k_TDC_CH_shift);
}

//______________________________________________________________________________
unsigned int
TkoMHTdc::get_tdc_id(unsigned int word_low) const
{
  return ((word_low & k_TDC_ID_mask) >> k_TDC_ID_shift);
}

//______________________________________________________________________________
bool
TkoMHTdc::is_tdc_ch_valid(unsigned int tdc_ch) const
{
  return ((0U <= tdc_ch) && (tdc_ch < k_N_TDC_CH));
}

//______________________________________________________________________________
bool
TkoMHTdc::is_tdc_id_valid(unsigned int tdc_id) const
{
  return ((0U < tdc_id) && (tdc_id < k_N_TDC));
}

//______________________________________________________________________________
void
TkoMHTdc::resize_fe_data()
{
  m_fe_data.resize(k_n_ch);
//   hoge(i2a(k_n_ch));
//   hoge(i2a(k_n_data_type_valid));
//   hoge(i2a(k_n_data_type_invalid));
  for (unsigned int i=0; i<k_n_ch_valid; ++i)
    m_fe_data[i].resize(k_n_data_type_valid);
  m_fe_data[k_ch_trigger_error].resize(k_n_data_type_invalid);
  m_fe_data[k_ch_invalid].resize(k_n_data_type_invalid);
  return;
}

  }
}
