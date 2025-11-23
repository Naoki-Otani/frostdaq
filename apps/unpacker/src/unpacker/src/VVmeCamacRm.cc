// -*- C++ -*-

#include <cstdio>
#include <string>
#include <sstream>


#include "std_ostream.hh"
#include "defines.hh"
#include "Unpacker.hh"

#include "VVmeCamacRm.hh"

namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type VVmeCamacRm::k_type = "VVmeCamacRm";

//______________________________________________________________________________
VVmeCamacRm::VVmeCamacRm(const unpacker_type& type)
  : VmeModule(type),
    m_data(0)
{

}

//______________________________________________________________________________
VVmeCamacRm::~VVmeCamacRm()
{
}

//______________________________________________________________________________
void
VVmeCamacRm::check_data_format()
{
  VmeModule::check_data_format();

  unsigned int lock_bit_event  = (m_data->m_event) & k_LOCK_BIT_mask;
  unsigned int lock_bit_spill  = (m_data->m_spill) & k_LOCK_BIT_mask;
//   unsigned int lock_bit_serial = (m_data->m_serial) & k_LOCK_BIT_mask;
  if (lock_bit_event==0 || lock_bit_spill==0)
    {
      cerr << "\n#E " << m_type << " " << m_name
      	   << " serial link was down" << std::endl;
      m_error_state.set(defines::k_bad_bit);
    }

  if (m_error_state[defines::k_bad_bit])
    {
      cout << "#D________________________________________________ "
	   << m_type << "::check_data_format" << "\n";
      Unpacker::dump_data(*this);
    }
  return;
}

//______________________________________________________________________________
void
VVmeCamacRm::decode()
{
  iterator_list::const_iterator f_begin = m_first_list.begin();
  iterator_list::const_iterator f_end   = m_first_list.end();
  for (iterator_list::const_iterator f = f_begin; f!=f_end; ++f)
    {
      iterator first = *f + VmeModule::k_header_size;
      data_t* d = reinterpret_cast<data_t*>(&(*first));

      fill(0, k_event_tag, d->m_event & k_MTM_MAX_EVENT_TAG);
      fill(1, k_event_tag, d->m_event & k_LOCK_BIT_mask);
      fill(0, k_spill_tag, d->m_spill & k_MTM_MAX_SPILL_TAG);
      fill(1, k_spill_tag, d->m_spill & k_LOCK_BIT_mask);
      fill(0, k_2ndAcc,    d->m_input_reg & 0x1U);
      fill(0, k_FastClear, (d->m_input_reg>>1) & 0x1U);

      //printf("Event : %x, Spill : %x, Reg : %x\n",
      //d->m_event, d->m_spill, d->m_input_reg);
    }

  return;
}

//______________________________________________________________________________
void
VVmeCamacRm::resize_fe_data()
{
  m_fe_data.resize(k_n_ch, FECh(k_n_data));
  return;
}

//______________________________________________________________________________
void
VVmeCamacRm::update_tag()
{
  iterator_list::const_iterator f_begin = m_first_list.begin();
  iterator_list::const_iterator f_end   = m_first_list.end();
  data_t* d = 0;
  m_tag[k_tag_current].clear();

  for (iterator_list::const_iterator f = f_begin; f!=f_end; ++f)
    {
      iterator first = *f + VmeModule::k_header_size;
      d = reinterpret_cast<data_t*>(&(*first));
      Tag tag;
      tag.m_event = (d->m_event) & k_MTM_MAX_EVENT_TAG;
      tag.m_spill = (d->m_spill) & k_MTM_MAX_SPILL_TAG;
      tag.m_local = defines::k_unassigned;
      m_tag[k_tag_current].push_back(tag);
//       if (d->m_spill_end_flag!=0)
// 	continue;
//       else
// 	break;
    }

//   Tag& tag = m_tag[k_tag_current];

//   tag.m_event = (d->m_event) & k_MTM_MAX_EVENT_TAG;
//   tag.m_spill = (d->m_spill) & k_MTM_MAX_SPILL_TAG;

  m_has_tag[k_event] = true;
  m_has_tag[k_spill] = true;
  m_data = d;

  return;
}

}
}
