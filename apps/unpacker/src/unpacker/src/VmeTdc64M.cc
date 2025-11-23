// -*- C++ -*-

// Author: Shuhei Hayakawa

#include "VmeTdc64M.hh"

#include <algorithm>

#include "std_ostream.hh"
#include "HexDump.hh"
#include "defines.hh"

namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type VmeTdc64M::k_type = "VmeTdc64M";

//______________________________________________________________________________
VmeTdc64M::VmeTdc64M(const unpacker_type& type)
  : VmeModule(type)
{
  Tag& orig = m_tag[k_tag_origin].back();
  orig.m_local = k_local_tag_origin;

  Tag& max = m_tag[k_tag_max].back();
  max.m_local = k_local_tag_max;
  max.m_event = 0;
  max.m_spill = 0;
}

//______________________________________________________________________________
VmeTdc64M::~VmeTdc64M()
{
}

//______________________________________________________________________________
void
VmeTdc64M::check_data_format()
{
  uint32_t header = *m_module_data_first;
  if (((header>>k_word_type_shift)&k_word_type_mask) != k_HEADER_MAGIC)
    m_error_state.set(defines::k_header_bit);
  return;
}

//______________________________________________________________________________
void
VmeTdc64M::decode()
{
  iterator_list::const_iterator f_begin = m_first_list.begin();
  iterator_list::const_iterator f_end   = m_first_list.end();
  for (iterator_list::const_iterator f=f_begin;  f!=f_end; ++f) {
    iterator first = *f;
    m_vme_header = reinterpret_cast<VmeModule::Header*>(&*first);
    m_module_data_first = first + VmeModule::k_header_size;

    const uint32_t* buf = reinterpret_cast<uint32_t*>(&*m_module_data_first);
    unsigned int data_size = m_vme_header->m_data_size - VmeModule::k_header_size;

    uint32_t module_id = 0xffffffff;
    for (unsigned int i=0; i<data_size; i++) {
      uint32_t word_type = ((buf[i]>>k_word_type_shift)&k_word_type_mask);
      switch(word_type){
      case k_HEADER_MAGIC:
	{
	  module_id = ((buf[i]>>k_module_id_shift) & k_module_id_mask);
	}
	break;
      case k_DATA_MAGIC:
	{
	  uint32_t ch     = ((buf[i]>>k_channel_shift)    & k_channel_mask);
	  uint32_t edge   = ((buf[i]>>k_edge_shift)       & k_edge_mask);
	  uint32_t data   = ((buf[i]>>k_data_shift)       & k_data_mask);
	  uint32_t hitnum = ((buf[i]>>k_hit_number_shift) & k_hit_number_mask);
	  switch(edge){
	  case k_leading:
	    if(hitnum>16)
	      cerr << "#E VmeTdc64M::decode() hitnum is too much: " << hitnum
		   << " ... (module_id: " << module_id << ")" << std::endl;
	    fill(ch, k_hitnum, hitnum);
	  case k_trailing:
	    fill(ch, edge, data);
	    break;
	  default:
	    cerr << "#E VmeTdc64M::decode() unknown edge: "
		 << std::hex << edge << "(" << buf[i] << ")" << std::endl
		 << std::dec;
	    break;
	  }//switch(edge)
	}//case k_DATA_MAGIC
	break;
      case k_FOOTER_MAGIC:
	{
	  //uint32_t event_number = ((buf[i]>>k_event_number_shift) & k_event_number_mask);
	  uint32_t error1 = ((buf[i]>>k_error1_shift) & k_error1_mask);
	  uint32_t error2 = ((buf[i]>>k_error2_shift) & k_error2_mask);
	  /*** error3&4 not defined yet. ***/
	  // uint32_t error3 = ((buf[i]>>k_error3_shift) & k_error3_mask);
	  // uint32_t error4 = ((buf[i]>>k_error4_shift) & k_error4_mask);
	  if( error1==1 )
	    cerr << "#W VmeTdc64M::decode() multi hit overflow"
		 << " ... (address: " << std::hex
		 << m_vme_header->m_vme_address << ")" << std::endl;
	  //<< " ... (module id: " << module_id << ")" << std::endl;
	  if( error2==1 )
	    cerr << "#E VmeTdc64M::decode() time range is not fullfilled ... (module id:"
		 << module_id << ")" << std::endl;
	}
	break;
      case k_INVALID_MAGIC:
	cerr << "#W VmeTdc64M::decode() not valid datum: "
	     << std::hex << word_type << " (" << buf[i] << ")" << std::endl
	     << std::dec;
        break;
      default:
	cerr << "#E VmeTdc64M::decode() unknown word type: "
	     << std::hex << word_type << "(" << buf[i] << ")" << std::endl
	     << std::dec;
	break;
      }//switch(word_type)
    }//for(i)
  }//for(f)
  return;
}

//______________________________________________________________________________
void
VmeTdc64M::update_tag()
{
  uint32_t buf = *(--m_data_last);
  uint32_t event_number = buf & k_event_number_mask;
  //  std::cout << std::hex << buf << std::endl;

  Tag& tag = m_tag[k_tag_current].back();
  tag.m_local = event_number;
  m_has_tag.set(k_local);
}

//______________________________________________________________________________
void
VmeTdc64M::resize_fe_data()
{
  m_fe_data.resize(k_n_ch);
  for(unsigned int i = 0; i<k_n_ch; ++i){
    m_fe_data[i].resize(k_n_data_type);
  }

  return;
}

}
}
