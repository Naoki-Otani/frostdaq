// -*- C++ -*-

// Author: Rintaro Kurata

#include "Rayraw.hh"

#include <iterator>

#include "std_ostream.hh"
#include "defines.hh"
#include "Unpacker.hh"

namespace hddaq
{

  namespace unpacker
  {

    const defines::unpacker_type Rayraw::k_type = "Rayraw";

    //______________________________________________________________________________
    Rayraw::Rayraw( const unpacker_type& uType )
      : DAQNode(uType),
	m_header(0),
	m_body_first()
    {
      Tag& orig = m_tag[k_tag_origin].back();
      orig.m_local = k_LOCAL_TAG_ORIGIN;

      Tag& max    = m_tag[k_tag_max].back();
      max.m_local = k_MAX_LOCAL_TAG;
      // max.m_event = k_MAX_EVENT_HRM_TAG;
      // max.m_spill = k_MAX_SPILL_HRM_TAG;
    }

    //______________________________________________________________________________
    Rayraw::~Rayraw( void )
    {
    }

    //______________________________________________________________________________
    void
    Rayraw::decode( void )
    {
      static const std::string func_name("["+k_type+"::"+__func__+"()]");

      int32_t over_flow = ((m_header->m_event_size >> k_OVERFLOW_SHIFT) & k_OVERFLOW_MASK);
      fill( 0, k_overflow, over_flow );

      const_iterator itr_body = m_body_first;

      // static uint32_t pre_ch=-1;
      // static uint32_t no_l[32] = {};
      // static uint32_t no_t[32] = {};

      // Data -----------------------------------------------

      for(; itr_body != m_data_last; ++itr_body){
	// Magic check
	uint32_t data_type = (*itr_body >> k_ADC_MAGIC_SHIFT) & k_ADC_MAGIC_MASK;

	// Waveform
	if(data_type == k_ADC_MAGIC_WORD){
	  uint32_t ch        = (*itr_body >> k_ADC_CH_SHIFT)       & k_ADC_CH_MASK;
	  uint32_t crs_cnt   = (*itr_body >> k_ADC_CRS_CNT_SHIFT)  & k_ADC_CRS_CNT_MASK;
	  uint32_t val       = (*itr_body >> k_ADC_DATA_SHIFT)     & k_ADC_DATA_MASK;
	  fill( ch, k_fadc, val);
	  fill( ch, k_crs_cnt, crs_cnt);

	}else{

	  // TDC
	  data_type = (*itr_body >> k_TDC_MAGIC_SHIFT) & k_TDC_MAGIC_MASK;
	  //	  std::cout << std::hex << "body= " << *itr_body << std::endl;
	  if(data_type == k_LEADING_MAGIC_WORD){
	    uint32_t ch        = (*itr_body >> k_TDC_CH_SHIFT)    & k_TDC_CH_MASK;
	    uint32_t val       = (*itr_body >> k_TDC_DATA_SHIFT)  & k_TDC_DATA_MASK;
	    fill( ch, k_leading, val);
	  
	  }else if(data_type == k_TRAILING_MAGIC_WORD){
	    uint32_t ch        = (*itr_body >> k_TDC_CH_SHIFT)    & k_TDC_CH_MASK;
	    uint32_t val       = (*itr_body >> k_TDC_DATA_SHIFT)  & k_TDC_DATA_MASK;
	    fill( ch, k_trailing, val);

	  }else{

	    // Unknown
	    cerr << "#W " << func_name << " Unknown data type ("
		 << std::hex << *itr_body << std::dec
		 << ")"
		 << std::endl;
	  }
	}

	//	Unpacker::dump_data(*this);
      }//for(i)
    return;


    // // Data -----------------------------------------------
    // for(; itr_body != m_data_last; ++itr_body){

    // 	// Magic check
    // 	uint32_t data_type = (*itr_body >> k_TDC_MAGIC_SHIFT) & k_TDC_MAGIC_MASK;
    // 	uint32_t ch        = (*itr_body >> k_TDC_CH_SHIFT)    & k_TDC_CH_MASK;
    // 	uint32_t val       = (*itr_body >> k_TDC_DATA_SHIFT)  & k_TDC_DATA_MASK;
    // 	if(data_type == k_LEADING_MAGIC_WORD){

    // 	  fill( ch, k_leading, val);
    // 	  std::cout << "\x1b[38;2;230;130;238m" << "decode: ch=" << ch << ", Leading=" << val << ", no[" << no_l[ch] + 1 << "]"<< "\x1b[m" << std::endl;
    // 	  no_l[ch] += 1;

    // 	}else if(data_type == k_TRAILING_MAGIC_WORD){

    // 	  fill( ch, k_trailing, val);     
    // 	  std::cout << "\x1b[38;2;50;205;50m" << "decode: ch=" << ch << ", Trailing=" << val << ", no[" << no_t[ch] + 1 << "]" << "\x1b[m" << std::endl;
    // 	  no_t[ch] += 1;

    // 	}else{        // ADC

    // 	  data_type = (*itr_body >> k_ADC_MAGIC_SHIFT) & k_ADC_MAGIC_MASK;
    // 	  if(data_type == k_ADC_MAGIC_WORD){
    // 	    ch        = (*itr_body >> k_ADC_CH_SHIFT)    & k_ADC_CH_MASK;
    // 	    val       = (*itr_body >> k_ADC_DATA_SHIFT)  & k_ADC_DATA_MASK;
    // 	    fill( ch, k_fadc, val);
    // 	      // if (ch != pre_ch)
    // 	      // 	std::cout << "ch=" << ch << ", FADC=" << val << std::endl;

    // 	      // pre_ch = ch;

    // 	  }else{
    // 	    cerr << "#W " << func_name << " Unknown data type ("
    // 		 << std::hex << *itr_body << std::dec
    // 		 << ")"
    // 		 << std::endl;
    // 	    Unpacker::dump_data(*this);
    // 	  }
    // 	}
    // }//for(i)

    //Unpacker::dump_data(*this);
  }

    //______________________________________________________________________________
    void
    Rayraw::check_data_format( void )
    {
      static const std::string func_name("["+k_type+"::"+__func__+"()]");

      // header magic word check
      if(k_HEADER_MAGIC != m_header->m_magic_word){
	m_error_state.set(defines::k_header_bit);
      }
    }

    //______________________________________________________________________________
    void
    Rayraw::resize_fe_data( void )
    {
      m_fe_data.resize(k_n_channel);
      for(uint32_t i = 0; i<k_n_channel; ++i){
	m_fe_data[i].resize(k_n_data_type);
      }
    }

    //______________________________________________________________________________
    bool
    Rayraw::unpack( void )
    {
      m_node_header = reinterpret_cast<DAQNode::Header*>(&(*m_data_first));
      m_header
	= reinterpret_cast<Header*>(&(*(m_data_first +DAQNode::k_header_size)));
      m_body_first = m_data_first
	+ DAQNode::k_header_size
	+ Rayraw::k_header_size;

      return true;
    }

    //______________________________________________________________________________
    void
    Rayraw::update_tag( void )
    {
      m_tag[k_tag_current].clear();

      // header event number check
      // Tag& max    = m_tag[k_tag_max].back();
      // max.m_event = k_MAX_EVENT_J0_TAG;
      // max.m_spill = k_MAX_SPILL_J0_TAG;

      uint32_t ev_counter
	= ((m_header->m_event_counter >> k_EVCOUNTER_SHIFT) & k_EVCOUNTER_MASK);
      // uint32_t j0_ev_tag
      //   = ((m_header->m_event_counter >> k_J0TAG_EVENT_SHIFT) & k_J0TAG_EVENT_MASK);
      // uint32_t j0_spill_tag
      //   = ((m_header->m_event_counter >> k_J0TAG_SPILL_SHIFT) & k_J0TAG_SPILL_MASK);

      Tag tag( ev_counter, 0, 0 );
      m_tag[k_tag_current].push_back( tag );
      m_has_tag.set(k_local);
      // m_has_tag.set(k_event);
      // m_has_tag.set(k_spill);

    }

  }

}
