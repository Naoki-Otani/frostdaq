// -*- C++ -*-

// Author: Tomonori Takahashi

#include "Finesse.hh"

#include <iterator>


#include "std_ostream.hh"
#include "defines.hh"

namespace hddaq
{
  namespace unpacker
  {

    const defines::unpacker_type Finesse::k_type = "Finesse";
    
//______________________________________________________________________________
Finesse::Finesse(const unpacker_type& type)
  : UnpackerImpl(type),
    m_header(0),
    m_trailer(0),
    m_body_first(),
    m_body_last()
{
  Tag& orig = m_tag[k_tag_origin].back();
  orig.m_local = k_LOCAL_TAG_ORIGIN;

  Tag& max    = m_tag[k_tag_max].back();
  max.m_local = k_FINESSE_MAX_LOCAL_TAG;
  max.m_event = k_FINESSE_MAX_EVENT_TAG;
  max.m_spill = k_FINESSE_MAX_SPILL_TAG;
}

//______________________________________________________________________________
Finesse::~Finesse()
{
}

//______________________________________________________________________________
void
Finesse::check_data_format()
{
  // header
  if (true
      && k_HEADER_MAGIC  != m_header->m_magic
      && k_HEADER_MAGIC1 != m_header->m_magic
      && k_HEADER_MAGIC2 != m_header->m_magic
      && k_HEADER_MAGIC3 != m_header->m_magic
      ){
    m_error_state.set(defines::k_header_bit);
    cout << "#E Finesse::check_data_format\n";
    cout << " wrong header magic word : " 
	 << std::hex << m_header->m_magic 
	 << std::dec << std::endl;
  }

  // trailer 
  if (k_TRAILER_masked != (m_trailer->m_value & 0xffff0000U)){
    m_error_state.set(defines::k_trailer_bit);
  }

  // data size
  m_word_count = std::distance(m_data_first, m_data_last);
  if (m_data_size != m_word_count){
    m_error_state.set(defines::k_data_size_bit);
  }
  
  return;
}

//______________________________________________________________________________
void
Finesse::print_header()
{
  if (!m_header)
    {
      cerr << "\n#E Finesse::print_header() : "
	   << m_name << " got null pointer"
	   << std::endl;
      return;
    }

  cout << "#D Finesse::print_header()\n"
       << std::hex
       << " magic = " << m_header->m_magic << "\n"
       << " tag   = " << m_header->m_tag   
       << std::dec
       << std::endl;
  
  return;
};

//______________________________________________________________________________
void
Finesse::print_trailer()
{
  if (!m_trailer)
    {
      cerr << "\n#E Finesse::print_trailer() : got null pointer"
	   << std::endl;
      return;
    }
  
  cout << "#D Finesse::print_trailer()\n"
       << std::hex
       << " value = " << m_trailer->m_value
       << std::dec
       << std::endl;
  
  return;
};

//______________________________________________________________________________
bool
Finesse::unpack()
{
  m_header = reinterpret_cast<Header*>(&(*m_data_first));
  m_trailer = reinterpret_cast<Trailer*>(&(*(m_data_last - k_trailer_size)));

  m_body_first = m_data_first + k_header_size;
  m_body_last  = m_data_last  - k_trailer_size;

  return true;
}

//______________________________________________________________________________
void
Finesse::update_tag()
{
  uint32_t tag_data = m_header->m_tag;

  Tag& tag = m_tag[k_tag_current].back();

  tag.m_local = (tag_data & k_LOCAL_TAG_mask) >> k_LOCAL_TAG_shift;
  tag.m_event = (tag_data & k_EVENT_TAG_mask) >> k_EVENT_TAG_shift;
  tag.m_spill = (tag_data & k_SPILL_TAG_mask) >> k_SPILL_TAG_shift;

//   cout << "#D Finesse::update_tag()\n"
// 	    << " " << m_name 
// 	    << " l = " << tag.m_local
// 	    << " e = " << tag.m_event
// 	    << " s = " << tag.m_spill
// 	    << std::endl;

//  m_has_tag.set();
  m_has_tag.set(k_event);
  m_has_tag.set(k_spill);
  return;
}


  }
}
