// -*- C++ -*-

// Author: Tomonori Takahashi

#include "VmeMaster.hh"

#include <iterator>
#include <algorithm>

#include "HexDump.hh"
#include "std_ostream.hh"
#include "Unpacker.hh"
#include "lexical_cast.hh"
#include "defines.hh"
#include "VmeModule.hh"

namespace hddaq
{
  namespace unpacker
  {

  namespace 
  {
    const uint32_t k_VME_HEADER_MAGIC = 0x00564d45; // 'VME'
  }
    
    const defines::unpacker_type VmeMaster::k_type = "VmeMaster";


//______________________________________________________________________________
VmeMaster::VmeMaster(const unpacker_type& type)
  : DAQNode(type),
    m_header(0)
{
}

//______________________________________________________________________________
VmeMaster::~VmeMaster()
{
}

//______________________________________________________________________________

void
VmeMaster::do_check()
{
//   cout << "#D VME Master Unpacker "
// 	  << " check_data_format() " << m_name << std::endl;

  // VME header
  if (k_VME_HEADER_MAGIC != m_header->m_magic)
    m_error_state.set(defines::k_header_bit);

  return;
}

//______________________________________________________________________________
void
VmeMaster::print_header() const
{
  if (!m_header)
    {
      cerr << "\n#E VmeMaster::print_header() : got null pointer to header"
	   << std::endl;
      return;
    }
  cout << "#D VmeMaster::print_header() " << m_name << "\n"
       << std::hex
       << " magic     = " << m_header->m_magic << "\n"
       << " data size = " << m_header->m_data_size << "\n"
       << " nblock    = " << m_header->m_nblock << "\n"
       << std::dec
       << std::endl;
  return;
}    

//______________________________________________________________________________
bool
VmeMaster::unpack()
{
//   Unpacker::dump_data(*this);
  m_node_header = reinterpret_cast<DAQNode::Header*>(&(*m_data_first));
  m_header      = reinterpret_cast<Header*>(&(*m_data_first)
					    + DAQNode::k_header_size);
//   cout << std::hex << m_id << " " << m_header->m_magic << " "
//        << m_header->m_data_size << " " 
//        << m_header->m_nblock
//        << std::dec
//        << std::endl;
  unsigned int rest = m_header->m_data_size - k_header_size;
  iterator vme_module_data_first
    = m_data_first + DAQNode::k_header_size + k_header_size;

  while (0<rest)
    {
      VmeModule::Header* module_header
	= reinterpret_cast<VmeModule::Header*>(&(*vme_module_data_first));
      if (module_header->m_magic!=VmeModule::k_VME_MODULE_HEADER_MAGIC)
	{
	  cerr << "#E " << m_type << ":" << m_name 
	       << " got a wrong magic word for VME module: "
	       << std::hex << std::showbase << module_header->m_magic
	       << " (expected = " << VmeModule::k_VME_MODULE_HEADER_MAGIC
	       << ")" << std::noshowbase << std::dec
	       << "\n --> advance iterator to next word (remain = "
	       << rest << ")" 
	       << std::endl;
	  --rest;
	  ++vme_module_data_first;
	  continue;
	}
      const uint64_t vme_address    = module_header->m_vme_address;
      const unsigned int data_size  = module_header->m_data_size;
      iterator vme_module_data_last = vme_module_data_first + data_size;

      /*
      if (vme_address == 0xD1000000 || vme_address == 0xD2000000) {
	cout << "VME address : " << std::hex << vme_address << "\n" << std::dec;
	std::for_each(vme_module_data_first, 
		      vme_module_data_first + data_size,
		      HexDump());
      }
      */
	//      std::for_each(vme_module_data_first, 
	//		    vme_module_data_first + sizeof(VmeModule::Header),
	//		    HexDump());
      Unpacker* child = m_child_list[vme_address];
      if (!child)
	{
	  cerr << "\n#E VmeMaster::unpack()\n "
	       << m_name
	       << " got unknown VME address : " 
	       << std::hex << std::showbase
	       << vme_address << std::dec << std::noshowbase
	       << " size = " << data_size
	       << std::endl;
// 	  Unpacker::dump_data(*this);
// 	  return true;
	}
      else
	{
	  //      cout << "#D VmeMaster::unpack()\n"
	  //	     << " vme address = " << std::hex << vme_address
	  //	     << " data size = " << data_size
	//	     << " name = " << name
	  //	     << " rest = " << rest
	  //	     << std::endl;
	  child->set_data_size(data_size);
	  child->set_data(vme_module_data_first, vme_module_data_last);
	}
      rest -= data_size;
      vme_module_data_first += data_size;
      //      cout << "#D master rest = " << rest << std::endl;
    }

  for (std::map<uint64_t, Unpacker*>::iterator i = m_child_list.begin(),
       iEnd=m_child_list.end(); i!=iEnd; ++i)
    {
      Unpacker* child = i->second;
      if (child && 0 != child->get_data_size()) child->unpack();
    }

  if (0U!=rest)
    {
      cerr << "\n#E VmeMaster::unpack()\n"
	   << " failed to split data : rest = " << rest << std::endl;
      m_error_state.set(defines::k_data_size_bit);
    }
  return true; 

}

}
}











