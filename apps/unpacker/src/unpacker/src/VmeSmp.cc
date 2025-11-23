// -*- C++ -*-

// Author: Tomonori Takahashi

#include "VmeSmp.hh"

#include <algorithm>
#include <bitset>

#include "std_ostream.hh"
#include "HexDump.hh"
#include "Unpacker.hh"
#include "lexical_cast.hh"
#include "defines.hh"

namespace hddaq
{
  namespace unpacker
  {

    const defines::unpacker_type VmeSmp::k_type = "VmeSmp";
  
//______________________________________________________________________________
VmeSmp::VmeSmp(const unpacker_type& type)
  : VmeModule(type),
    m_smp_header(0),
    m_sds_serial_number(0),
    m_sds_n_word(0),
    m_module_down(0),
    m_switch_trailer_magic(defines::k_unassignedU),
    m_register(0),
    m_tko(k_n_tko),
    m_sds_begin(),
    m_sds_end()
{
  //  cout << "#D VmeSmp::VmeSmp()" << std::endl;
  m_tag[k_tag_origin].back().m_local = k_LOCAL_TAG_ORIGIN;
  m_tag[k_tag_max].back().m_local = k_SMP_MAX_LOCAL_TAG;
  //  dump_data();
}

//______________________________________________________________________________
VmeSmp::~VmeSmp()
{
}

//______________________________________________________________________________
void
VmeSmp::check_data_format()
{
  //  cout << "#D VME SMP Unpacker::check_data_format() " 
  //       << m_name << std::endl;
  VmeModule::check_data_format();

  // trailer
  if (0!=(m_smp_header->m_event_id & k_event_last))
    if (k_SWITCH_TRAILER_MAGIC != m_switch_trailer_magic)
      {
	cerr << "\n#E SMP " << m_name
	     << " has wrong switch trailer "
	     << std::hex << m_switch_trailer_magic 
	     << std::dec
	     << std::endl;
	m_error_state.set(defines::k_trailer_bit);
      }

  if (k_MAGIC_MODULE_DOWN_BIT
      != (m_module_down >>k_MAGIC_MODULE_DOWN_BIT_shift ))
    {
      cerr << "\n#E " << k_type << " " << m_name 
	   << " has broken data in module down bit which starts : "
	   << std::hex 
	   << (m_module_down>>k_MAGIC_MODULE_DOWN_BIT_shift)
	   << std::dec
	   << std::endl;
      m_error_state.set(defines::k_trailer_bit);
    }
  std::bitset<k_n_tko> module_down_bit(m_module_down & k_MODULE_DOWN_BIT_mask);
  if (module_down_bit.any())
    {
//       cerr << "\n#E " << k_type << " " 
// 		<< m_name << " detects trailer error"
// 		<< " (module down bit) "
// 		<< module_down_bit
// 		<< " (hex:" 
// 		<< std::hex << m_module_down_bit << ")" << std::endl;
//      m_error_state.set(defines::k_trailer_bit);
    }

  
//   cout << "#D " << k_type << " data size = " 
// 	    << m_data_size 
// 	    << ", word count = " << m_word_count
// 	    << std::endl;
  if (m_data_size != m_word_count)
    m_error_state.set(defines::k_data_size_bit);

  if (m_error_state[defines::k_bad_bit])
    Unpacker::dump_data(*this);
    
  return;
}      

//______________________________________________________________________________
void
VmeSmp::decode()
{
  //  cout << "#D " << k_type << "::decode()" << std::endl;
  
//   dump_data16();
  if (!m_register)
    return;
  fill(0, k_STAR,  m_register->m_star);
  fill(0, k_SNCCR, m_register->m_snccr);
  fill(0, k_SNCR,  m_register->m_sncr);
  fill(0, k_DSR,   m_register->m_dsr);
  fill(0, k_ENR,   m_register->m_enr);
  fill(0, k_BCR,   m_register->m_bcr);
  fill(0, k_SCR,   m_register->m_scr);
  fill(0, k_ICR,   m_register->m_icr);
  fill(0, k_IVR,   m_register->m_ivr);
  fill(0, k_n_register, m_smp_header->m_event_id);
  return;
}

//______________________________________________________________________________
void
VmeSmp::dump_data16() const
{
  cout << "#D " << k_type << "::dump_data16() : name = " << m_name 
       << std::endl;
  const unsigned short* const k_first 
    = reinterpret_cast<unsigned short*>(&*m_data_first);
  const unsigned short* const k_last 
    = k_first + 2*(m_data_last - m_data_first);
  std::for_each(k_first, k_last, HexDump());
  return;
}

//______________________________________________________________________________
const std::map<std::string, int>&
VmeSmp::get_data_id() const
{
  static std::map<std::string, int> s_data_ref;
  if(s_data_ref.empty())
    {
      s_data_ref["STAR"]  = k_STAR;
      s_data_ref["SNCCR"] = k_SNCCR;
      s_data_ref["SNCR"]  = k_SNCR;
      s_data_ref["DSR"]   = k_DSR;
      s_data_ref["ENR"]   = k_ENR;
      s_data_ref["BCR"]   = k_BCR;
      s_data_ref["SCR"]   = k_SCR;
      s_data_ref["ICR"]   = k_ICR;
      s_data_ref["IVR"]   = k_IVR;
    }

  return s_data_ref;
}


//______________________________________________________________________________
void
VmeSmp::resize_fe_data()
{
  m_fe_data.resize(1);
  m_fe_data[0].resize(k_n_register + 1);
  return;
}

//______________________________________________________________________________
bool
VmeSmp::set_tko_data(Unpacker* u,
		     unsigned int ma,
		     const iterator& data_first,
		     const iterator& data_last)
{
  //  cout << "#D " << m_name << " ::set_tko_data()" << std::endl;

  //  std::for_each(data_first, data_last, HexDump());
    
  if (!u)
    {
      cerr << "\n#E " << k_type << "::unpack()\n" << m_name
	   << " got unknown data of TKO MA = " 
	   << ma << ", data : ";
      std::for_each(data_first, data_last, HexDump());
      return false;
    }

  u->set_data_size(std::distance(data_first, data_last));
  u->set_data(data_first, data_last);

  u->unpack();
  return true;
}

//______________________________________________________________________________
void
VmeSmp::sort_child()
{
  for (unsigned int i=0; i<k_n_tko; ++i){
    m_tko[i] = m_child_list[i];
  }
  
  return;
}
    
//______________________________________________________________________________
bool
VmeSmp::unpack()
{
//   hoge();

//  Unpacker::dump_data(*this);
  if(!unpack_header()) return false;
  if(!unpack_trailer()) return false;
  if(!unpack_body()) return false;

  //  Unpacker::dump_data(*this);

  return true;
}

//______________________________________________________________________________
bool
VmeSmp::unpack_body()
{
  
  iterator first = m_sds_begin + 1;
  iterator last  = m_sds_end;
  iterator itr   = first;

  const std::vector<Unpacker*>& tko = m_tko;
  unsigned int prev_ma = 0U;
  unsigned int ma      = 0U;
  bool ret             = false;

  for (; itr!=last; ++itr)
    {
      ma = (*itr & k_MA_mask) >> k_MA_shift; 
      if (ma>=k_n_tko)
	{
	  cerr << "#E " << m_type << " " << m_name
	       << " got invalid MA = " << ma << ", data = " 
	       << std::hex << std::showbase
	       << *itr << std::dec << std::noshowbase
	       << std::endl;

	  if (gm_check_mode[defines::k_format])
	    m_error_state.set(defines::k_bad_bit);
	  continue;
	}
      if (prev_ma==0U)
	prev_ma = ma;

      if (ma!=prev_ma) // module address changed
	{
	  ret = set_tko_data(tko[prev_ma], prev_ma, first, itr);
	  prev_ma = ma;
	  first   = itr;
	}
      if (itr==last-1)
	ret = set_tko_data(tko[ma], ma, first, last);
    }

  return ret;
}

//______________________________________________________________________________
bool
VmeSmp::unpack_header()
{
//   m_vme_header=reinterpret_cast<VmeModule::Header*>(&(*m_data_first));
//   m_smp_header
//     =reinterpret_cast<Header*>(&(*(m_data_first+VmeModule::k_header_size)));
  VmeModule::unpack();
  m_smp_header = reinterpret_cast<Header*>(&(*m_module_data_first));

  
  m_sds_begin = m_data_first+VmeModule::k_header_size+k_header_size;
  m_sds_serial_number = (*m_sds_begin & k_LOCAL_TAG_mask)>>k_LOCAL_TAG_shift;
  m_sds_n_word = *m_sds_begin & k_SDS_N_WORD_mask;

  m_sds_end  = m_sds_begin + m_sds_n_word;

  uint32_t rest  
    = m_vme_header->m_data_size - VmeModule::k_header_size - k_header_size;
  //  cout << "#D " << k_type << "::unpack()\n"
    //       << " header size = " << sizeof(struct VmeHeader)/sizeof(uint32_t)
  //       << " serial = " << m_sds_serial_number
  //       << " n_word = " << m_sds_n_word
  //       << " rest = " << rest
  //       << std::endl;

  if (m_sds_n_word != rest -1){ // rest include SMP header, then -1
    cerr << "#E " << k_type << " " << m_name 
	 << " N word missmatch" << std::endl;
    cerr << " SDS n word: " << m_sds_n_word << ", VME n word: " << rest-1 << std::endl;
    m_error_state.set(defines::k_data_size_bit);
    Unpacker::dump_data(*this);
    return false;
  }

  return true;
}

//______________________________________________________________________________
bool
VmeSmp::unpack_trailer()
{
  m_module_down = *m_sds_end;

  if (0U!=(m_smp_header->m_event_id & k_event_first))
    m_register = reinterpret_cast<register_type*>
      (&(*(m_data_last - k_n_register)));
  else
    m_register = 0;

  if (0!=(m_smp_header->m_event_id & k_event_last))
    m_switch_trailer_magic = *(m_sds_end+1);

  //  cout << "#D module down bit = "
  //       << std::hex << m_module_down << std::dec << std::endl;

  return true;
}

//______________________________________________________________________________
void
VmeSmp::update_tag()
{
  m_tag[k_tag_current].back().m_local = m_sds_serial_number;

  m_has_tag[k_local]           = true;
  return;
}

  }
}
