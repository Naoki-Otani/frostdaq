// -*- C++ -*-

// Author: Hitoshi Sugimura

#include "CopperLite.hh"

#include <algorithm>
#include <vector>

#include "std_ostream.hh"
#include "Unpacker.hh"
#include "lexical_cast.hh"
#include "defines.hh"

namespace hddaq
{
  namespace unpacker
  {
    
//______________________________________________________________________
const defines::unpacker_type CopperLite::k_type = "CopperLite";
  
//______________________________________________________________________
CopperLite::CopperLite(const unpacker_type& type)
  : DAQNode(type), m_header(0), m_trailer(0)
{
      
}

//_______________________________________________________________________
CopperLite::~CopperLite()
{
      
}

//_______________________________________________________________________
void
CopperLite::print_header() const
{
  if (!m_header)
    {
      cerr << "\n#E CopperLite::print_header() : "
	   << m_name << " got null pointer to header"
	   << std::endl;
      return;
    }
  cout << "#D CopperLite::print_header()\n"
       << std::hex
       << " magic    = " << m_header->m_magic << "\n"
       << " slot_id  = " << m_header->m_slot_id << "\n"
       << " length   = " << m_header->m_length << "\n"
       << " id       = " << m_header->m_id << "\n"
       << std::dec
       << std::endl;
  return;
};

//_______________________________________________________________________
void
CopperLite::print_trailer() const
{
  if (!m_trailer)
    {
      cerr << "\n#E CopperLite::print_trailer() : "
	   << m_name << " got null pointer"
	   << std::endl;
      return;
    }
  cout << "#D CopperLite::print_trailer()\n"
       << std::hex
       << " magic = " << m_trailer->m_magic
       << std::dec
       << std::endl;
  return;
};

//______________________________________________________________________________
void
CopperLite::sort_child()
{
  m_child.resize(k_n_finesse);
  for (unsigned int i=0; i<k_n_finesse; ++i)
    m_child[i] = m_child_list[i];
  return;
}

//___________________________________________________________________
bool
CopperLite::unpack()
{
  m_node_header = reinterpret_cast<DAQNode::Header*>(&(*m_data_first));
  iterator itr_copper_data = m_data_first + DAQNode::k_header_size;

  for(unsigned int slot = 0; slot<k_n_finesse; ++slot){
    // Copper lite header decode
    m_header = reinterpret_cast<Header*>(&(*itr_copper_data));
    int n_word_in_finesse = m_header->m_length/sizeof(unsigned int);

    Unpacker* child = m_child[slot];
    if(!child){
      cerr << "\n#E Copper::unpack()\n "
	   << m_name << " got data of unknown Finesse"
	   << std::endl;
      itr_copper_data += sizeof(Header)/sizeof(unsigned int);
      itr_copper_data += n_word_in_finesse;
      itr_copper_data += sizeof(Trailer)/sizeof(unsigned int);
      continue;
    }

    // unpack body
    iterator finesse_data_first 
      = itr_copper_data + sizeof(Header)/sizeof(unsigned int);

    child->set_data_size(n_word_in_finesse);
    child->set_data(finesse_data_first,
		    finesse_data_first + n_word_in_finesse
		    );
    if(!child->unpack()){ return false;}
    itr_copper_data = finesse_data_first 
      + n_word_in_finesse + sizeof(Trailer)/sizeof(unsigned int);

#if 0
      std::cout<<"#D CopperLite::unpack()"<<std::endl;
      std::cout<<"CopperLite Header : "
	       <<std::hex 
	       << m_header->m_magic << " "
	       << m_header->m_slot_id << " "
	       << m_header->m_length << " "
	       << m_header->m_id << " "
	       <<std::endl;
#endif
  }

  return true;
}

  }
}
