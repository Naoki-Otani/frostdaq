// -*- C++ -*-

// Author: Shuhei Hayakawa

#include "GetCoBoMaster.hh"

#include <iterator>
#include <algorithm>

#include "HexDump.hh"
#include "std_ostream.hh"
#include "Unpacker.hh"
#include "lexical_cast.hh"
#include "defines.hh"
#include "GetAsAd.hh"

namespace hddaq
{
namespace unpacker
{

const defines::unpacker_type GetCoBoMaster::k_type = "GetCoBoMaster";

//_____________________________________________________________________________
GetCoBoMaster::GetCoBoMaster(const unpacker_type& type)
  : DAQNode( type ),
    m_header( nullptr )
{
}

//_____________________________________________________________________________
GetCoBoMaster::~GetCoBoMaster()
{
}

//_____________________________________________________________________________
void
GetCoBoMaster::do_check()
{
  if (k_COBO_HEADER_MAGIC != m_header->m_magic)
    m_error_state.set(defines::k_header_bit);
  return;
}

//_____________________________________________________________________________
void
GetCoBoMaster::print_header() const
{
  if( !m_header ){
    cerr << std::endl
         << "#E GetCoBoMaster::print_header() : got null pointer to header"
         << std::endl;
    return;
  }
  cout << "#D GetCoBoMaster::print_header() " << m_name << "\n"
       << std::hex
       << " magic     = " << m_header->m_magic << std::endl
       << " cobo id   = " << m_header->m_cobo_id << std::endl
       << " data size = " << m_header->m_data_size << std::endl
       << " nblock    = " << m_header->m_nblock << std::endl
       << std::dec
       << std::endl;
  return;
}

//______________________________________________________________________________
bool
GetCoBoMaster::unpack()
{
  //  Unpacker::dump_data(*this);
  m_node_header = reinterpret_cast<DAQNode::Header*>(&(*m_data_first));
  m_header      = reinterpret_cast<Header*>(&(*m_data_first)
					    + DAQNode::k_header_size);
  // print_header();
  unsigned int rest = m_header->m_data_size - k_header_size;
  iterator asad_data_first
    = m_data_first + DAQNode::k_header_size + k_header_size;

  while( 0<rest ){
    GetAsAd::Header* module_header
      = reinterpret_cast<GetAsAd::Header*>(&(*asad_data_first));
    if( module_header->m_magic != GetAsAd::k_ASAD_HEADER_MAGIC ){
      cerr << "#E " << m_type << ":" << m_name
           << " got a wrong magic word for AsAd module: "
           << std::hex << std::showbase << module_header->m_magic
           << " (expected = " << GetAsAd::k_ASAD_HEADER_MAGIC
           << ")" << std::noshowbase << std::dec
           << "\n --> advance iterator to next word (remain = "
           << rest << ")"
           << std::endl;
      --rest;
      ++asad_data_first;
      continue;
    } else {
#if 0
      cout << "#D " << m_type << ":" << m_name
           << " got a correct magic word for AsAd module: "
           << std::hex << std::showbase << module_header->m_magic
           << std::endl;
#endif
    }
    const uint32_t revision = module_header->Revision;
    if( revision != GetAsAd::kRevision ){
      cerr << "#E " << m_type << ":" << m_name
           << " " << module_header
           << " got a wrong revision for AsAd module header: "
           << revision
           << " (expected = " << GetAsAd::kRevision
           << ")"
           << "\n --> advance iterator to next word (remain = "
           << rest << ")"
           << std::endl;
      --rest;
      ++asad_data_first;
      continue;
    } else {
#if 0
      cout << "#D " << m_type << ":" << m_name
           << " got a correct revision for AsAd module header: "
           << revision << std::endl;
#endif
    }
    const uint32_t asad_id = ( module_header->AsadIdx +
                               m_header->m_cobo_id*NumOfAsAd );
    uint32_t data_size = module_header->m_data_size;
    iterator asad_data_last = asad_data_first + data_size;
    if( data_size > rest ){
      cerr << "\n#E GetCoBoMaster::unpack()\n "
           << m_name
           << " found data size mismatch AsAd Id : "
           << std::hex << std::showbase
           << asad_id << std::dec << std::noshowbase
           << " size = " << data_size << ", rest = " << rest
           << std::endl;
      break;
      // data_size = rest;
    }

    Unpacker* child = m_child_list[asad_id];
    if( !child ){
      cerr << "\n#E GetCoBoMaster::unpack()\n "
           << m_name
           << " got unknown AsAd Id : "
           << std::hex << std::showbase
           << asad_id << std::dec << std::noshowbase
           << " size = " << data_size
           << std::endl;
      // Unpacker::dump_data(*this);
    } else {
#if 0
      cout << "#D GetCoBoMaster::unpack()\n"
           << " AsAd id = " << std::hex << asad_id
           << " data size = " << data_size
           << " name = " << m_name
           << " rest = " << rest
           << std::endl;
#endif
      child->set_data_size(data_size);
      child->set_data(asad_data_first, asad_data_last);
    }
    rest -= data_size;
    asad_data_first += data_size;
    // cout << "#D master rest = " << rest << std::endl;
  }

  for( std::map<uint64_t, Unpacker*>::iterator i = m_child_list.begin(),
         iEnd=m_child_list.end(); i!=iEnd; ++i ){
    Unpacker* child = i->second;
    if( child && child->get_data_size() > 0 ) child->unpack();
  }

  if( rest != 0 ){
    cerr << "\n#E GetCoBoMaster::unpack()\n"
         << " failed to split data : rest = " << rest << std::endl;
    m_error_state.set(defines::k_data_size_bit);
  }
  return true;

}

}
}
