// -*- C++ -*-

// Author: Tomonori Takahashi

#include "DAQNode.hh"

#include <algorithm>
#include <sstream>

#include <cstdlib>

#include "UnpackerManager.hh"
#include "std_ostream.hh"
#include "Unpacker.hh"
#include "defines.hh"


namespace hddaq
{
  namespace unpacker
  {
    const defines::unpacker_type DAQNode::k_type = "DAQNode";


//______________________________________________________________________________
DAQNode::DAQNode(const unpacker_type& type)
  : UnpackerImpl(type),
    m_node_header(0),
    m_child()
{
  m_tag[k_tag_origin].back().m_local = k_NODE_EVENT_NUMBER_ORIGIN;
  m_is_node   = true;
  Tag& max    = m_tag[k_tag_max].back();
  max.m_local = 0x7fffffffU;
  max.m_event = k_MTM_MAX_EVENT_TAG;
  max.m_spill = k_MTM_MAX_SPILL_TAG;
}

//______________________________________________________________________________
DAQNode::~DAQNode()
{
}

//______________________________________________________________________________
void
DAQNode::check_data_format()
{
  // run number
  if (m_run_number != static_cast<int>(m_node_header->m_run_number)
      &&
      (m_run_number>=0))
    {
      cerr << "\n#E run number changed " << k_type << " " << m_name 
	   << " prev: " << m_run_number 
	   << ", current: " << m_node_header->m_run_number
	   << std::endl;
//       std::exit(1);
      m_error_state.set(defines::k_bad_bit);
    }
  m_run_number = m_node_header->m_run_number;
  //  cout << "#D " << m_name 
  //       << " run number = " << m_run_number << std::endl;

  // node header
  if (k_NODE_HEADER_MAGIC != m_node_header->m_magic)
    m_error_state.set(defines::k_header_bit);

  // data size
  m_word_count =  std::distance(m_data_first, m_data_last);
  if (m_word_count != m_data_size)
    m_error_state.set(defines::k_data_size_bit);

  do_check();
  return;
}

//______________________________________________________________________________
void
DAQNode::do_check()
{
  return;
}

//______________________________________________________________________________
unsigned int
DAQNode::get_header(int data_type) const
{
  if(data_type >= k_n_node_header){
    cerr << "\n#E DAQNode::get_header invalid data_type: " 
	 << data_type << std::endl;
    return 0;
  }
  unsigned int* ptr = reinterpret_cast<unsigned int*>(m_node_header);
  return *(ptr + data_type);
}

//______________________________________________________________________________
void
DAQNode::print_header(const Header* h)
{
  if (!h)
    {
      cerr << "\n#E " << k_type << "::print_header() : "
	   << " got null pointer "
	   << std::endl;
      return;
    }
  
  cout << "#D print_node_header()\n"
       << std::hex
       << " magic            = " << h->m_magic << "\n"
       << " data  size       = " << h->m_data_size << "\n"
       << " event number     = " << h->m_event_number << "\n"
       << " run number       = " << h->m_run_number << "\n"
       << " node id          = " << h->m_node_id << "\n"
       << " node type        = " << h->m_node_type << "\n"
       << " number of blocks = " << h->m_number_of_blocks << "\n"
       << " unix_time        = " << h->m_unix_time << "\n"
       << std::dec
       << std::endl;
  return;
}
    
    
//______________________________________________________________________________
void
DAQNode::print_header() const
{
  if (m_node_header)
    {
      cerr << "\n#E " << k_type << "::print_header() : "
	   << m_name 
	   << " got null pointer "
	   << std::endl;
      return;
    }

  cout << "#D print_node_header()\n"
       << std::hex
       << " magic            = " << m_node_header->m_magic << "\n"
       << " data  size       = " << m_node_header->m_data_size << "\n"
       << " event number     = " << m_node_header->m_event_number << "\n"
       << " run number       = " << m_node_header->m_run_number << "\n"
       << " node id          = " << m_node_header->m_node_id << "\n"
       << " node type        = " << m_node_header->m_node_type << "\n"
       << " number of blocks = " << m_node_header->m_number_of_blocks << "\n"
       << " unix_time        = " << m_node_header->m_unix_time << "\n"
       << std::dec
       << std::endl;
  return;
}

//______________________________________________________________________________
void
DAQNode::print_node_header() const
{
  if (!m_node_header)
    {
      cerr << "\n#E " << k_type << "::print_header() : "
	   << m_name << " got null pointer"
	   << std::endl;
      return;
    }
  const Header* const h  = m_node_header;
  cout << "#D " << k_type << "::print_header() " << m_name << "\n";
  print_header(h);
  return;
}

//______________________________________________________________________________
void
DAQNode::print_trailer() const
{
  return;
}

//______________________________________________________________________________
bool
DAQNode::unpack()
{
  m_node_header = reinterpret_cast<Header*>(&(*m_data_first));

  if (m_child_list.empty())
    return true;

  uint32_t rest       = m_data_size  - k_header_size;
  iterator node_first = m_data_first + k_header_size;

  std::deque<Unpacker*>::iterator uitr = m_child.begin();
  std::deque<Unpacker*>::iterator uend = m_child.end();
  Unpacker* u = 0;
  UnpackerManager& g_unpacker = GUnpacker::get_instance();
  while (0<rest) 
    {
      Header* child_node_header = reinterpret_cast<Header*>(&(*node_first));
      uint32_t node_size = child_node_header->m_data_size;
      uint32_t node_id   = child_node_header->m_node_id;
      uint32_t node_type = child_node_header->m_node_type; 

//       cout << "#D node_id " << std::showbase 
// 	   << std::hex << node_id << std::dec << std::noshowbase
// 	   << " node_type " << node_type << std::endl;

      if (defines::k_event_type_null==node_type)
	{
	  cerr << "\n#E " << m_name
	       << " got event type null" << std::endl;
	  return false;
	}

      if (uitr==uend)
	{
	  ChildList::iterator i = m_child_list.find(node_id);
	  if (i==m_child_list.end())
	    {
	      cerr << "\n#E " << k_type << "::unpack()\n" << m_name
		   << " got unknown node : node id = "
		   << node_id << " (hex " << std::hex << node_id << ")"
		   << " --> create new node"
		   << std::endl;
	      u = g_unpacker.create(k_type);
	      u->set_id(node_id);
	      std::stringstream ss;
	      ss << "UNKNOWN_" << std::hex << node_id;
	      u->set_name(ss.str());
	      m_child_list[node_id] = u;
	    }
	  else
	    u = i->second;
// 	  cout << "#D new unpacker " << k_type << " node_id = " << node_id
// 	       << " (" << std::hex << node_id << std::dec << ")"
// 	       << ", " << u->get_id()
// 	       << std::endl;

	  // push_back
	  uitr = m_child.insert(uend, u); // std::deque<Unpacker*>
	  uend = m_child.end();
	}
      else
	{
	  u = *uitr;
// 	  cout << "#D unpacker " << k_type << " node_id = " << node_id
// 	       << " (" << std::hex << node_id << std::dec << ")";
// 	  if (u)
// 	    cout << ", " << u->get_id();
// 	  cout << std::endl;
	}

      if (defines::k_event_type_normal==node_type)
	{
	  if (u)
	    {
	      if (u->get_id()!=node_id)
		{
		  cerr << "\n#E " << k_type << "::unpack()\n" 
		       << " node id mismatch : " 
		       << " data = " << node_id << ", list = " << u->get_id()
		       << std::endl;
		  return false;
		}
	      u->set_data_size(node_size);
	      u->set_data(node_first,
			  node_first + node_size);
	      if (!u->unpack())
		return false;
	    }
	  else
	    {
	      cerr << "\n#E " << k_type << "::unpack()\n" 
		   << " null pointer to  node id = " << node_id
		   << std::endl;
	      return false;
	    }
	}
      else if (defines::k_event_type_dummy==node_type)
	{
	  if (g_unpacker.get_counter()==0)
	    cout << "#D " << u->get_name() << " : DUMMY Mode " << std::endl;
	}
      
      node_first += node_size; 
      rest       -= node_size;
      ++uitr;
    }
  return true;
}

//______________________________________________________________________________
void
DAQNode::sort_child()
{
  return;
}

//______________________________________________________________________________
void
DAQNode::update_tag()
{
  m_tag[k_tag_current].back().m_local = m_node_header->m_event_number;


  m_has_tag[k_local] = true;
  return;
}

  }
}
