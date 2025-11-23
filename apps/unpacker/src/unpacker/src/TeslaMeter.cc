// -*- C++ -*-

#include <iostream>

#include "std_ostream.hh"
#include "Unpacker.hh"
#include "defines.hh"
#include "lexical_cast.hh"
#include "TeslaMeter.hh"


namespace hddaq
{
namespace unpacker
{

  namespace 
  {
    const uint32_t k_TESLA_METER_MAGIC = 0x00464c44; // 'FLD'
  }
  
  const defines::unpacker_type TeslaMeter::k_type = "TeslaMeter";

//______________________________________________________________________________
TeslaMeter::TeslaMeter(const unpacker_type& type)
  : DAQNode(type),
    m_data(0)
{
}

//______________________________________________________________________________
TeslaMeter::~TeslaMeter()
{
}

//______________________________________________________________________________
const TeslaMeter::Format*
TeslaMeter::get_data(const Unpacker* u)
{
//   if (k_type!=u->get_type)
//     {
//       cerr << k_type << "::get_data() : got invalid unpacker "
// 	   << u->get_type() << std::endl;
//       return 0;
//     }
  return dynamic_cast<TeslaMeter&>(u->get()).m_data;
}  

//______________________________________________________________________________
void
TeslaMeter::print(const Unpacker* u)
{
//   if (u->get_type()!=k_type)
//     {
//       cerr << k_type << "::get_data() : got invalid unpacker "
// 	   << u->get_type() << std::endl;
//     }
  dynamic_cast<TeslaMeter&>(u->get()).print();
  return;
}

//______________________________________________________________________________
void
TeslaMeter::print() const
{
  const std::string id = reinterpret_cast<char*>(&m_data->id);
  const std::string field = m_data->data;
  cout << m_name << "::print() \n"
       << " id            = " << m_data->id << " (" << id << ")" 
       << "\n data length = " << m_data->length 
       << "\n data        = " << field
       << "\n sec         = " << m_data->tv_sec
       << "\n nsec        = " << m_data->tv_nsec
       << "\n n read      = " << m_data->nread
       << "\n n send      = " << m_data->nsend
       << std::endl;
  return;
}

//______________________________________________________________________________
bool
TeslaMeter::unpack()
{
  uint32_t* const ptr = &(*m_data_first);
  m_node_header = reinterpret_cast<DAQNode::Header*>(ptr);
  m_data        = reinterpret_cast<Format*>(ptr + DAQNode::k_header_size);
  
  m_sec.push_back(m_data->tv_sec);
  m_nsec.push_back(m_data->tv_nsec);

  return true;
}

}
}
