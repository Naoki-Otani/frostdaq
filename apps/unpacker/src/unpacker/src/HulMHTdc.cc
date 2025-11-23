// -*- C++ -*-

#include "HulMHTdc.hh"

#include <iterator>

#include "std_ostream.hh"
#include "defines.hh"
#include "Unpacker.hh"

namespace hddaq
{

namespace unpacker
{

const defines::unpacker_type HulMHTdc::k_type = "HulMHTdc";

//______________________________________________________________________________
HulMHTdc::HulMHTdc( const unpacker_type& uType )
  : DAQNode(uType),
    m_header(0),
    m_body_first(),
    m_has_rm(false)
{
  Tag& orig = m_tag[k_tag_origin].back();
  orig.m_local = k_LOCAL_TAG_ORIGIN;

  Tag& max    = m_tag[k_tag_max].back();
  max.m_local = k_MAX_LOCAL_TAG;
  max.m_event = k_MAX_EVENT_HRM_TAG;
  max.m_spill = k_MAX_SPILL_HRM_TAG;
}

//______________________________________________________________________________
HulMHTdc::~HulMHTdc( void )
{
}

//______________________________________________________________________________
void
HulMHTdc::decode( void )
{
  static const std::string func_name("["+k_type+"::"+__func__+"()]");

  //  std::size_t n_word_body = ((m_header->m_event_size >> k_EVSIZE_SHIFT) & k_EVSIZE_MASK);
  int32_t over_flow = ((m_header->m_event_size >> k_OVERFLOW_SHIFT) & k_OVERFLOW_MASK);
  fill( 0, k_overflow, over_flow );

  const_iterator itr_body = m_body_first;

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
    //    n_word_body--;
  }

  for(; itr_body != m_data_last; ++itr_body){
    uint32_t data_type = (*itr_body >> k_data_header_shift) & k_data_header_mask;
    uint32_t ch        = (*itr_body >> k_tdc_ch_shift)      & k_tdc_ch_mask;
    uint32_t val       = (*itr_body >> k_tdc_data_shift)    & k_tdc_data_mask;
    if(data_type == k_LEADING_MAGIC){
      fill( ch, k_leading, val );
    }else if(data_type == k_TRAILING_MAGIC){
      fill( ch, k_trailing, val );
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
HulMHTdc::check_data_format( void )
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
HulMHTdc::resize_fe_data( void )
{
  m_fe_data.resize(k_n_channel);
  for(uint32_t i = 0; i<k_n_channel; ++i){
    m_fe_data[i].resize(k_n_data_type);
  }
}

//______________________________________________________________________________
bool
HulMHTdc::unpack( void )
{
  m_node_header = reinterpret_cast<DAQNode::Header*>(&(*m_data_first));
  m_header
    = reinterpret_cast<Header*>(&(*(m_data_first +DAQNode::k_header_size)));

  m_body_first = m_data_first
    + DAQNode::k_header_size
    + HulMHTdc::k_header_size;

  return true;
}

//______________________________________________________________________________
void
HulMHTdc::update_tag( void )
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
    Tag& max    = m_tag[k_tag_max].back();
    max.m_event = k_MAX_EVENT_J0_TAG;
    max.m_spill = k_MAX_SPILL_J0_TAG;

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

}
