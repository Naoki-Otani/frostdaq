// -*- C++ -*-

// Author: Tomonori Takahashi

#include "VmeCaenV820.hh"

#include <algorithm>

#include "std_ostream.hh"
#include "HexDump.hh"

namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type VmeCaenV820::k_type = "VmeCaenV820";

//______________________________________________________________________________
VmeCaenV820::VmeCaenV820(const unpacker_type& type)
  : VmeModule(type)
{
}

//______________________________________________________________________________
VmeCaenV820::~VmeCaenV820()
{
}

//______________________________________________________________________________
void
VmeCaenV820::decode()
{
  iterator_list::const_iterator f_begin = m_first_list.begin();
  iterator_list::const_iterator f_end   = m_first_list.end();
  for (iterator_list::const_iterator f = f_begin;  f!=f_end; ++f)
    {
      iterator first = *f;
      m_vme_header = reinterpret_cast<VmeModule::Header*>(&*first);
      m_module_data_first = first + VmeModule::k_header_size;

      const uint32_t* buf = reinterpret_cast<uint32_t*>(&*m_module_data_first);

      for (unsigned int ch=0; ch<k_n_ch; ++ch)
	fill(ch, 0, buf[ch]);

      // std::for_each(buf, buf+k_n_ch, HexDump());
    }
  return;
}

//______________________________________________________________________________
void
VmeCaenV820::resize_fe_data()
{
  m_fe_data.resize(k_n_ch, FECh(k_n_data_type));
  return;
}

}
}
