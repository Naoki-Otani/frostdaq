// -*- C++ -*-

// Author: Shuhei Hayakawa

#include "HulMassTrigFera.hh"

#include <bitset>
#include <iterator>

#include "std_ostream.hh"
#include "defines.hh"
#include "Unpacker.hh"

namespace hddaq
{

namespace unpacker
{

const defines::unpacker_type HulMassTrigFera::k_type = "HulMassTrigFera";

//______________________________________________________________________________
HulMassTrigFera::HulMassTrigFera( const unpacker_type& uType )
  : DAQNode(uType),
    m_header(0),
    m_body_first(),
    m_has_rm(true)
{
  Tag& orig = m_tag[k_tag_origin].back();
  orig.m_local = k_LOCAL_TAG_ORIGIN;

  Tag& max    = m_tag[k_tag_max].back();
  max.m_local = k_MAX_LOCAL_TAG;
  max.m_event = k_MAX_EVENT_TAG;
  max.m_spill = 0;
}

//______________________________________________________________________________
HulMassTrigFera::~HulMassTrigFera( void )
{
}

//______________________________________________________________________________
void
HulMassTrigFera::decode( void )
{
  const_iterator i = m_body_first;

  if( m_has_rm ){
    uint32_t event = (*i>>k_rm_event_shift) & k_rm_event_mask;
    uint32_t spill = (*i>>k_rm_spill_shift) & k_rm_spill_mask;
    uint32_t sinc  = (*i>>k_rm_sinc_shift)  & k_rm_sinc_mask;
    uint32_t lock  = (*i>>k_rm_lock_shift)  & k_rm_lock_mask;
    fill( 0, k_rm_event, event );
    fill( 0, k_rm_spill, spill );
    fill( 0, k_rm_sinc,  sinc  );
    fill( 0, k_rm_lock,  lock  );
    i++;
  }

  const uint32_t data_size =
    ((m_header->m_event_size >> k_EVSIZE_SHIFT) & k_EVSIZE_MASK);

  for( uint32_t j=0; j<data_size-1; ++j, ++i ){
    uint32_t magic = (*i>>k_data_magic_shift) & k_data_magic_mask;
    switch( magic ){
    case k_fera_magic: {
      uint32_t ch  = (*i>>k_fera_channel_shift) & k_fera_channel_mask;
      uint32_t val = (*i>>k_fera_data_shift) & k_fera_data_mask;
      fill( ch, k_fera_data, val );
      break;
    }
    case k_coin_magic: {
      uint32_t block = (*i>>k_coin_block_shift) & k_coin_block_mask;
      std::bitset<k_n_one_block> hit_bit
	= (*i>>k_coin_channel_shift) & k_coin_channel_mask;
      for( uint32_t k=0; k<k_n_one_block; ++k ){
	uint32_t ch  = k+k_n_one_block*block;
	uint32_t val = hit_bit[k];
	fill( ch, k_coin_data, val );
      }
      break;
    }
    case k_tag_magic: {
      uint32_t tag = (*i>>k_tag_shift) & k_tag_mask;
      fill( 0, k_tag_data, tag );
      break;
    }
    default:
      cerr << "#E HulMassTrigFera::decode() invalid magic : "
	   << std::hex << magic << std::endl;
      Unpacker::dump_data(*this);
      break;
    }
  }

  return;
}

//______________________________________________________________________________
void
HulMassTrigFera::check_data_format( void )
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
    unsigned int lock  = (*i>>k_rm_lock_shift)  & k_rm_lock_mask;
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
HulMassTrigFera::resize_fe_data( void )
{
  m_fe_data.resize(k_n_channel);
  for( uint32_t i=0; i<k_n_channel; ++i ){
    m_fe_data[i].resize(k_n_data_type);
  }
}

//______________________________________________________________________________
bool
HulMassTrigFera::unpack( void )
{
  m_node_header = reinterpret_cast<DAQNode::Header*>(&(*m_data_first));
  m_header
    = reinterpret_cast<Header*>(&(*(m_data_first +DAQNode::k_header_size)));

  m_body_first = m_data_first
    + DAQNode::k_header_size
    + HulMassTrigFera::k_header_size;

  return true;
}

//______________________________________________________________________________
void
HulMassTrigFera::update_tag( void )
{
  m_tag[k_tag_current].clear();

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
    // m_has_tag.set(k_local);
    // m_has_tag.set(k_event);
    // m_has_tag.set(k_spill);
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
