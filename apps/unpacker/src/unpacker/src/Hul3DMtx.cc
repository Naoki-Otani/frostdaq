//-------------------------------------------------------------------------
// hul3dmtx decoding class for unpacker
//-------------------------------------------------------------------------

#include"Hul3DMtx.hh"

#include<iterator>

#include"std_ostream.hh"
#include"defines.hh"

namespace hddaq{
  namespace unpacker{
    const defines::unpacker_type Hul3DMtx::k_type = "Hul3DMtx";

// Constructor ------------------------------------------------------------
Hul3DMtx::Hul3DMtx(const unpacker_type& uType)
  :DAQNode(uType),
   m_header(0),
   m_body_first()
{
  Tag& orig = m_tag[k_tag_origin].back();
  orig.m_local = k_LOCAL_TAG_ORIGIN;

  Tag& max    = m_tag[k_tag_max].back();
  max.m_local = k_HUL3DMTX_MAX_LOCAL_TAG;
  max.m_event = k_HUL3DMTX_MAX_EVENT_TAG;
  max.m_spill = 0;
}
      
// Destructor -------------------------------------------------------------
Hul3DMtx::~Hul3DMtx()
{
  
}
    
// decode -----------------------------------------------------------------
void Hul3DMtx::decode(){
  const_iterator i = m_body_first;

  for(int current = 0; current<(int)k_n_data; ++current, ++i){
    unsigned int data_type = (*i >> k_data_type_shift) & k_data_type_mask;

    if(data_type == k_TOF_MAGIC){
      unsigned int ch    = (*i >> k_ch_shift) & k_ch_mask;
      unsigned int time  = *i & k_tdc_data_mask;
      if(time != 0) fill(ch, k_ttof, time);
      
    }else if(data_type == k_FBH_MAGIC){
      unsigned int ch  = (*i >> k_ch_shift) & k_ch_mask;
      unsigned int time  = *i & k_tdc_data_mask;
      if(time != 0) fill(ch, k_tfbh, time);

    }else if(data_type == k_CH_MAGIC){
      unsigned int ch  = (*i >> k_ch_shift) & k_ch_mask;
      unsigned int time  = *i & k_tdc_data_mask;
      if(time != 0) fill(ch, k_tch, time);

    }else{
      cerr << "#E Hul3DMtx::decode unknown data type : " << std::hex
	   << data_type << "(" << (*i) << ")" << "\n" << std::dec;
    }

  }

  return;
}

// check_data_format ------------------------------------------------------
void Hul3DMtx::check_data_format(){
  // header magic word check
  if(k_HEADER_MAGIC != m_header->m_hul3dmtx_magic){
    m_error_state.set(defines::k_header_bit);
  }

  // header # of word check
  m_word_count = std::distance(m_body_first, m_data_last);
  if(m_word_count != k_n_data){
    m_error_state.set(defines::k_data_size_bit);
  }

  return;
}

// resize_fe_data ---------------------------------------------------------
void Hul3DMtx::resize_fe_data(){
  m_fe_data.resize(k_n_port);
  for(unsigned int i = 0; i<k_n_port; ++i){
    m_fe_data[i].resize(k_n_data_type);
  }

  return;
}

// unpack -----------------------------------------------------------------
bool Hul3DMtx::unpack(){
  m_node_header = reinterpret_cast<DAQNode::Header*>(&(*m_data_first));
  m_header 
    = reinterpret_cast<Header*>(&(*(m_data_first +DAQNode::k_header_size)));
  
  m_body_first = m_data_first 
    + DAQNode::k_header_size
    + Hul3DMtx::k_header_size;

  return true;
}

// update tag -------------------------------------------------------------
void Hul3DMtx::update_tag()
{    
  // header event number check
  unsigned int ev_counter 
    = ((m_header->m_event_counter >> k_EVCOUNTER_SHIFT) & k_EVCOUNTER_MASK);
  unsigned int ev_tag
    = m_header->m_event_counter & k_EVTAG_MASK;
  //  if(ev_tag != ((ev_counter+1) & k_EVTAG_MASK)){
  //    m_error_state.set(defines::k_event_tag_bit);
  //  }  
  
  Tag& tag = m_tag[k_tag_current].back();
  tag.m_local = ev_counter;
  tag.m_event = ev_tag;

  m_has_tag.set(k_local);
  //  m_has_tag.set(k_event);
}

  }
}
