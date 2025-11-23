// -*- C++ -*-

#include "HulMsT.hh"

#include <iterator>

#include "std_ostream.hh"
#include "defines.hh"
#include "Unpacker.hh"

namespace hddaq
{

namespace unpacker
{

const defines::unpacker_type HulMsT::k_type = "HulMsT";

//______________________________________________________________________________
HulMsT::HulMsT( const unpacker_type& uType )
  : DAQNode(uType),
    m_header(0),
    m_body_first(),
    m_has_rm(false)
{
  Tag& orig = m_tag[k_tag_origin].back();
  orig.m_local = k_LOCAL_TAG_ORIGIN;

  Tag& max    = m_tag[k_tag_max].back();
  max.m_local = k_MAX_LOCAL_TAG;
  max.m_event = k_MAX_EVENT_TAG;
  max.m_spill = 0;
}

//______________________________________________________________________________
HulMsT::~HulMsT( void )
{
}

//______________________________________________________________________________
void
HulMsT::decode( void )
{
  static const std::string func_name("["+k_type+"::"+__func__+"()]");

  const_iterator itr_body = m_body_first;

  // HRM ----------------------------------------------------
  if( m_has_rm ){
    uint32_t event = (*itr_body >> k_rm_event_shift) & k_rm_event_mask;
    uint32_t spill = (*itr_body >> k_rm_spill_shift) & k_rm_spill_mask;
    uint32_t sinc  = (*itr_body >> k_rm_sinc_shift)  & k_rm_sinc_mask;
    uint32_t lock  = (*itr_body >> k_rm_lock_shift)  & k_rm_lock_mask;
    fill( 0, k_rm_event, event );
    fill( 0, k_rm_spill, spill );
    fill( 0, k_rm_sinc,  sinc  );
    fill( 0, k_rm_lock,  lock  );
    itr_body++;
  }

  // MsT ---------------------------------------------------
  {
    uint32_t data_type          = (*itr_body >> k_mst_header_shift)             & k_mst_header_mask;
    uint32_t clear              = (*itr_body >> k_mst_clear_shift)              & k_mst_clear_mask;
    uint32_t accept             = (*itr_body >> k_mst_accept_shift)             & k_mst_accept_mask;
    uint32_t final_clear        = (*itr_body >> k_mst_final_clear_shift)        & k_mst_final_clear_mask;
    uint32_t consolation_accept = (*itr_body >> k_mst_consolation_accept_shift) & k_mst_consolation_accept_mask;
    uint32_t fast_clear         = (*itr_body >> k_mst_fast_clear_shift)         & k_mst_fast_clear_mask;
    uint32_t level2             = (*itr_body >> k_mst_level2_shift)             & k_mst_level2_mask;
    uint32_t no_decision        = (*itr_body >> k_mst_no_decision_shift)        & k_mst_no_decision_mask;

    if(data_type == k_MST_MAGIC){
      fill( 0, k_mst_clear,              clear);
      fill( 0, k_mst_accept,             accept);
      fill( 0, k_mst_final_clear,        final_clear);
      fill( 0, k_mst_consolation_accept, consolation_accept);
      fill( 0, k_mst_fast_clear,         fast_clear);
      fill( 0, k_mst_level2,             level2);
      fill( 0, k_mst_no_decision,        no_decision);
      itr_body++;
    }else{
      cerr << "#W " << func_name << " Unknown data type ("
	   << std::hex << *itr_body << std::dec
	   << ")"
	   << std::endl;
      Unpacker::dump_data(*this);
    }
  }// MsT

  // TDC data -----------------------------------------------
  enum DataMode{k_HR, k_LR};
  DataMode mode = k_HR;
  for(; itr_body != m_data_last; ++itr_body){
    // Block type check
    uint32_t block_type = (*itr_body >> k_block_header_shift) & k_block_header_mask;
    if(block_type == k_HR_HEADER_MAGIC){
      mode = k_HR;
      continue;
    }else if(block_type == k_LR1_HEADER_MAGIC || block_type == k_LR2_HEADER_MAGIC){
      mode = k_LR;
      continue;
    }

    // Data decode
    if(mode == k_HR){
      uint32_t data_type = (*itr_body >> k_hr_magic_shift) & k_hr_magic_mask;
      uint32_t ch        = (*itr_body >> k_hr_ch_shift)    & k_hr_ch_mask;
      uint32_t val       = (*itr_body >> k_hr_data_shift)  & k_hr_data_mask;

      if(data_type == k_HR_MAGIC){
	fill( ch, k_hr_leading, val);
      }else{
	cerr << "#W " << func_name << " Unknown data type ("
	     << std::hex << *itr_body << std::dec
	     << ")"
	     << std::endl;
	Unpacker::dump_data(*this);
      }
    }else{
      uint32_t data_type = (*itr_body >> k_lr_magic_shift) & k_lr_magic_mask;
      uint32_t ch        = (*itr_body >> k_lr_ch_shift)    & k_lr_ch_mask;
      uint32_t val       = (*itr_body >> k_lr_data_shift)  & k_lr_data_mask;

      if(data_type == k_LR_MAGIC){
	fill( ch, k_lr_leading, val);
      }else{
	cerr << "#W " << func_name << " Unknown data type ("
	     << std::hex << *itr_body << std::dec
	     << ")"
	     << std::endl;
	Unpacker::dump_data(*this);
      }
    }
  }//for(i)

  return;
}

//______________________________________________________________________________
void
HulMsT::check_data_format( void )
{
  static const std::string func_name("["+k_type+"::"+__func__+"()]");

  // header magic word check
  if(k_HEADER_MAGIC != m_header->m_magic_word){
    m_error_state.set(defines::k_header_bit);
  }

  if( m_has_rm ){
    const_iterator i = m_body_first;
    if( ( (*i>>k_rm_magic_shift) & k_rm_magic_mask ) != k_rm_magic ){
      cerr << "#W " << func_name << " invalid data " << std::endl;
      Unpacker::dump_data(*this);
      return;
    }
    uint32_t lock  = (*i>>k_rm_lock_shift)  & k_rm_lock_mask;
    if ( lock==0 ) {
      cerr << std::endl << "#E " << func_name
      	   << " serial link was down" << std::endl;
      m_error_state.set(defines::k_header_bit);
    }

    if ( m_error_state[defines::k_header_bit] ) {
      cout << "#D________________________________________________ "
	   << m_type << "::check_data_format" << "\n";
      Unpacker::dump_data(*this);
    }
  }
}

//______________________________________________________________________________
void
HulMsT::resize_fe_data( void )
{
  m_fe_data.resize(k_n_channel);
  for(uint32_t i = 0; i<k_n_channel; ++i){
    m_fe_data[i].resize(k_n_data_type);
  }
}

//______________________________________________________________________________
bool
HulMsT::unpack( void )
{
  m_node_header = reinterpret_cast<DAQNode::Header*>(&(*m_data_first));
  m_header
    = reinterpret_cast<Header*>(&(*(m_data_first +DAQNode::k_header_size)));

  m_body_first = m_data_first
    + DAQNode::k_header_size
    + HulMsT::k_header_size;

  return true;
}

//______________________________________________________________________________
void
HulMsT::update_tag( void )
{
  m_tag[k_tag_current].clear();

  //  const std::size_t data_size =
  //    ((m_header->m_event_size >> k_EVSIZE_SHIFT) & k_EVSIZE_MASK);

  m_has_rm = false;
  if( ((m_header->m_event_counter >> k_HRM_SHIFT) & k_HRM_MASK )) m_has_rm = true;

  // header event number check
  uint32_t ev_counter
    = ((m_header->m_event_counter >> k_EVCOUNTER_SHIFT) & k_EVCOUNTER_MASK);

  if( m_has_rm ){
    const_iterator i = m_body_first;
    uint32_t event
      = (( *i >> k_rm_event_shift ) & k_rm_event_mask );
    uint32_t spill
      = (( *i >> k_rm_spill_shift ) & k_rm_spill_mask );
    Tag tag( ev_counter, event, spill );
    m_tag[k_tag_current].push_back( tag );
    m_has_tag.set(k_local);
    m_has_tag.set(k_event);
    m_has_tag.set(k_spill);
  }
  else{
    uint32_t ev_tag
      = ((m_header->m_event_counter >> k_EVTAG_SHIFT) & k_EVTAG_MASK);
    Tag tag( ev_counter, ev_tag, defines::k_unassigned );
    m_tag[k_tag_current].push_back( tag );
    m_has_tag.set(k_local);
    // m_has_tag.set(k_event);
  }
}

}

}
