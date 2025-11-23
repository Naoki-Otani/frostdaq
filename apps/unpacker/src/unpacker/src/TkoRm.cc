// -*- C++ -*-

// Author: Tomonori Takahashi

#include "TkoRm.hh"

#include "std_ostream.hh"
#include "defines.hh"
#include "Unpacker.hh"

namespace hddaq
{
  namespace unpacker
  {

    const defines::unpacker_type TkoRm::k_type = "TkoRm";

//______________________________________________________________________________
TkoRm::TkoRm(const unpacker_type& type)
  : Tko(type)
{
}

//______________________________________________________________________________
TkoRm::~TkoRm()
{
}


//______________________________________________________________________________
void
TkoRm::check_data_format()
{
  m_word_count = std::distance(m_data_first, m_data_last);
  if (m_word_count!=k_n_data)
    {
      cerr << "\n#E " << m_type << " " << m_name 
	   << " wrong data size" << m_word_count << std::endl;
      m_error_state.set(defines::k_data_size_bit);
    }
  else
    {
      unsigned int lock_bit = *(m_data_first + k_spill_tag) & k_LOCK_BIT_mask;
      if (lock_bit==0)
	{
	  cerr << "\n#E " << m_type << " " << m_name 
	       << " serial link was down" << std::endl;
	  m_error_state.set(defines::k_bad_bit);
	}
    }

  if (m_error_state[defines::k_bad_bit])
    {
      cout << "#D_____________________________________________________ "
	   << m_type << "::check_data_format" << "\n";
      Unpacker::dump_data(*this);

    }

  return;
}    

//______________________________________________________________________________
void
TkoRm::resize_fe_data()
{
  m_fe_data.resize(k_n_data);
  for (unsigned int i=0; i<k_n_data; ++i)
    m_fe_data[i].resize(1);
  return;
}

//______________________________________________________________________________
void
TkoRm::update_tag()
{

  Tag& tag    = m_tag[k_tag_current].back();
  tag.m_event = (*(m_data_first + k_event_tag)) & k_MTM_MAX_EVENT_TAG;
  tag.m_spill = (*(m_data_first + k_spill_tag)) & k_MTM_MAX_SPILL_TAG;

  m_has_tag[k_event] = true;
  m_has_tag[k_spill] = true;
  return;
}
 
  }
}
