// -*- C++ -*-

// Author: Kenji Hosomi for E13

#include "HBJTimeStamp.hh"

#include <algorithm>

#include "std_ostream.hh"
#include "HexDump.hh"

namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type HBJTimeStamp::k_type = "HBJTimeStamp";

//______________________________________________________________________________
HBJTimeStamp::HBJTimeStamp(const unpacker_type& type)
  : VmeModule(type)
{
}

//______________________________________________________________________________
HBJTimeStamp::~HBJTimeStamp()
{
}

//______________________________________________________________________________
void
HBJTimeStamp::decode()
{
  iterator_list::const_iterator f_begin = m_first_list.begin();
  iterator_list::const_iterator f_end   = m_first_list.end();
  for (iterator_list::const_iterator f = f_begin;  f!=f_end; ++f)
    {
      //      iterator first = *f;
      //      m_vme_header = reinterpret_cast<VmeModule::Header*>(&*first);
      //      m_module_data_first = first + VmeModule::k_header_size;
      
      const uint32_t* buf = reinterpret_cast<uint32_t*>(&*m_module_data_first);
      fill(0, 0, buf[0]);

    }
  return;
}

//______________________________________________________________________________
void
HBJTimeStamp::resize_fe_data()
{
  m_fe_data.resize(k_n_ch);
  for (unsigned int i=0; i<k_n_ch; ++i) {
    m_fe_data[i].resize(k_n_data_type);
  }

  return;
}

}
}
