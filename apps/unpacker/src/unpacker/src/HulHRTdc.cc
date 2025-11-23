// -*- C++ -*-

#include "HulHRTdc.hh"

#include <iterator>

#include "std_ostream.hh"
#include "defines.hh"
#include "Unpacker.hh"

namespace hddaq
{

namespace unpacker
{

const defines::unpacker_type HulHRTdc::k_type = "HulHRTdc";

//______________________________________________________________________________
HulHRTdc::HulHRTdc( const unpacker_type& uType )
  : DAQNode(uType),
    m_header(0),
    m_body_first()
{
  Tag& orig = m_tag[k_tag_origin].back();
  orig.m_local = k_LOCAL_TAG_ORIGIN;

  Tag& max    = m_tag[k_tag_max].back();
  max.m_local = k_MAX_LOCAL_TAG;
  max.m_event = k_MAX_EVENT_HRM_TAG;
  max.m_spill = k_MAX_SPILL_HRM_TAG;
}

//______________________________________________________________________________
HulHRTdc::~HulHRTdc( void )
{
}

//______________________________________________________________________________
void
HulHRTdc::decode( void )
{
  static const std::string func_name("["+k_type+"::"+__func__+"()]");

  int32_t over_flow = ((m_header->m_event_size >> k_OVERFLOW_SHIFT) & k_OVERFLOW_MASK);
  fill( 0, k_overflow, over_flow );

  const_iterator itr_body = m_body_first;

  // TDC data -----------------------------------------------
  int index_block = 0;
  for(; itr_body != m_data_last; ++itr_body){
    // Block type check
    uint32_t block_type = (*itr_body >> k_block_header_shift) & k_block_header_mask;
    if(block_type == k_BLOCK1_HEADER_MAGIC || block_type == k_BLOCK2_HEADER_MAGIC){
      index_block = block_type == k_BLOCK1_HEADER_MAGIC ? 0 : 1;
      continue;
    }

    // Data decode
    uint32_t data_type = (*itr_body >> k_magic_shift) & k_magic_mask;
    uint32_t ch        = (*itr_body >> k_ch_shift)    & k_ch_mask;
    uint32_t val       = (*itr_body >> k_data_shift)  & k_data_mask;

    if(data_type == k_LEADING_MAGIC){
      fill( ch + index_block*k_n_one_block, k_leading, val);
    }else if(data_type == k_TRAILING_MAGIC){
      fill( ch + index_block*k_n_one_block, k_trailing, val);
    }else if(data_type == k_STOP_MAGIC){
      fill( index_block, k_stop, val);
    }else{
      cerr << "#W " << func_name << " Unknown data type ("
	   << std::hex << *itr_body << std::dec
	   << ")"
	   << std::endl;
      Unpacker::dump_data(*this);
    }
  }//for(i)

  return;
}

//______________________________________________________________________________
void
HulHRTdc::check_data_format( void )
{
  static const std::string func_name("["+k_type+"::"+__func__+"()]");

  // header magic word check
  if(k_HEADER_MAGIC_1 != m_header->m_magic_word &&
     k_HEADER_MAGIC_2 != m_header->m_magic_word){
    m_error_state.set(defines::k_header_bit);
  }
}

//______________________________________________________________________________
void
HulHRTdc::resize_fe_data( void )
{
  m_fe_data.resize(k_n_channel);
  for(uint32_t i = 0; i<k_n_channel; ++i){
    m_fe_data[i].resize(k_n_data_type);
  }
}

//______________________________________________________________________________
bool
HulHRTdc::unpack( void )
{
  m_node_header = reinterpret_cast<DAQNode::Header*>(&(*m_data_first));
  m_header
    = reinterpret_cast<Header*>(&(*(m_data_first +DAQNode::k_header_size)));

  m_body_first = m_data_first
    + DAQNode::k_header_size
    + HulHRTdc::k_header_size;

  return true;
}

//______________________________________________________________________________
void
HulHRTdc::update_tag( void )
{
  m_tag[k_tag_current].clear();

  // header event number check
  Tag& max    = m_tag[k_tag_max].back();
  max.m_event = k_MAX_EVENT_J0_TAG;
  max.m_spill = k_MAX_SPILL_J0_TAG;

  uint32_t ev_counter
    = ((m_header->m_event_counter >> k_EVCOUNTER_SHIFT) & k_EVCOUNTER_MASK);
  uint32_t j0_ev_tag
    = ((m_header->m_event_counter >> k_J0TAG_EVENT_SHIFT) & k_J0TAG_EVENT_MASK);
  uint32_t j0_spill_tag
    = ((m_header->m_event_counter >> k_J0TAG_SPILL_SHIFT) & k_J0TAG_SPILL_MASK);

  Tag tag( ev_counter, j0_ev_tag, j0_spill_tag );
  m_tag[k_tag_current].push_back( tag );
  m_has_tag.set(k_local);
  m_has_tag.set(k_event);
  m_has_tag.set(k_spill);

}

}

}
