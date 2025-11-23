// -*- C++ -*-

// Author: Tomonori Takahashi

#include "Copper.hh"

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

//______________________________________________________________________________
  namespace
  {
    const uint32_t k_COPPER_HEADER_MAGIC  = 0xfffffafaU;
    const uint32_t k_COPPER_TRAILER_MAGIC = 0xfffff5f5U;
    const int k_n_finesse                 = 4;
  }

    const defines::unpacker_type Copper::k_type = "Copper";

//______________________________________________________________________________
Copper::Copper(const unpacker_type& type)
  : DAQNode(type),
    m_header(0),
    m_trailer(0)
{
}

//______________________________________________________________________________
Copper::~Copper()
{
}

//______________________________________________________________________________
void
Copper::do_check()
{
  // copper header
  if (k_COPPER_HEADER_MAGIC != m_header->m_magic)
    m_error_state.set(defines::k_bad_bit);

  // trailer
  if (k_COPPER_TRAILER_MAGIC != m_trailer->m_magic)
    m_error_state.set(defines::k_trailer_bit);

  return;
}

//______________________________________________________________________________
void
Copper::print_header() const
{
  if (!m_header)
    {
      cerr << "\n#E Copper::print_header() : "
	   << m_name << " got null pointer to header"
	   << std::endl;
      return;
    }
  cout << "#D Copper::print_header()\n"
       << std::hex
       << " magic       = " << m_header->m_magic << "\n"
       << " data size   = " << m_header->m_data_size << "\n"
       << " slot A size = "
       << m_header->m_finesse_data_size[0] << "\n"
       << " slot B size = "
       << m_header->m_finesse_data_size[1] << "\n"
       << " slot C size = "
       << m_header->m_finesse_data_size[2] << "\n"
       << " slot D size = "
       << m_header->m_finesse_data_size[3] << "\n"
       << std::dec
       << std::endl;
  return;
};

//______________________________________________________________________________
void
Copper::print_trailer() const
{
  if (!m_trailer)
    {
      cerr << "\n#E Copper::print_trailer() : "
	   << m_name << " got null pointer"
	   << std::endl;
      return;
    }
  cout << "#D Copper::print_trailer()\n"
       << std::hex
       << " magic = " << m_trailer->m_magic
       << std::dec
       << std::endl;
  return;
};

//______________________________________________________________________________
void
Copper::sort_child()
{
  m_child.resize(k_n_finesse);
  for (int i=0; i<k_n_finesse; ++i)
    m_child[i] = m_child_list[i];
  return;
}

//______________________________________________________________________________
bool
Copper::unpack()
{
  m_node_header = reinterpret_cast<DAQNode::Header*>(&(*m_data_first));
  m_header
    = reinterpret_cast<Header*>
    (&(*(m_data_first+sizeof(DAQNode::Header)/sizeof(uint32_t))));
  m_trailer
    = reinterpret_cast<Trailer*>
    (&(*(m_data_last-sizeof(Trailer)/sizeof(uint32_t))));

//   print_node_header(m_node_header);
//   print_header();
//   print_trailer();

  iterator copper_data_first = m_data_first + DAQNode::k_header_size;
//   const unsigned int k_copper_data_size = m_node_header->m_data_size;
//   cout << "#D size = " << std::hex
// 	  << k_copper_data_size << std::dec << std::endl;

  iterator finesse_data_first
    = copper_data_first + sizeof(Header)/sizeof(unsigned int);
  // std::deque<Unpacker*>::iterator fend = m_child.end();
  for (int slot=0; slot<4; ++slot)
    {
      if (0<m_header->m_finesse_data_size[slot])
	{
 	  Unpacker* child = m_child[slot];
	  if (!child)
	    {
	      cerr << "\n#E Copper::unpack()\n "
		   << m_name << " got data of unknown Finesse"
		   << std::endl;
	      finesse_data_first += m_header->m_finesse_data_size[slot];
	      continue;
	    }

 	  child->set_data_size(m_header->m_finesse_data_size[slot]);
	  child->set_data(finesse_data_first,
			  finesse_data_first
			  + m_header->m_finesse_data_size[slot]);
	  if (!child->unpack())
	    return false;
	  finesse_data_first += m_header->m_finesse_data_size[slot];
	}
    }
  return true;
}

  }
}
