// -*- C++ -*-

// Author: Tomonori Takahashi

#include "VmeModule.hh"

#include <algorithm>

#include "std_ostream.hh"

namespace hddaq
{
  namespace unpacker
  {

//______________________________________________________________________________
VmeModule::VmeModule(const unpacker_type& type)
  : UnpackerImpl(type),
    m_vme_header(0),
    m_module_data_first()
{
}

//______________________________________________________________________________
VmeModule::~VmeModule()
{
}
    
//______________________________________________________________________________
void
VmeModule::check_data_format()
{
  // header
  if (!m_vme_header)
    {
      cerr << "\n#E " << m_type << " " << m_name << " does not have "
	   << " vme module header \n"  << std::endl;
      m_error_state.set(defines::k_header_bit);
    }
  else if (k_VME_MODULE_HEADER_MAGIC!=m_vme_header->m_magic)
    {
      cerr << "\n#E " << m_type << " " << m_name
	   << " has wrong vme module header \n data  = " 
	   << std::hex << m_vme_header->m_magic << "\n magic = "
	   << k_VME_MODULE_HEADER_MAGIC
	   << std::dec
	   << std::endl;
      m_error_state.set(defines::k_header_bit);
    }

  // data size
  m_word_count = std::distance(m_data_first, m_data_last);
  return;
}

//______________________________________________________________________________
void
VmeModule::print_vme_header() const
{
  if (!m_vme_header)
    {
      cout << "\n#E VmeModule::print_vme_header() : got "
	   << m_name << " null pointer to header"
	   << std::endl;
      return;
    }
  cout << "#D VmeModule::print_header() " << m_name << "\n"
       << std::showbase << std::hex
       << " magic = " << m_vme_header->m_magic << "\n"
       << " address = " << m_vme_header->m_vme_address << "\n"
       << " data size = " << m_vme_header->m_data_size << "\n"
       << " n times read device = " << m_vme_header->m_n_times_read_device << "\n"
       << " module type = \n    " 
       << m_vme_header->m_module_type[0] << "\n   "
       << m_vme_header->m_module_type[1] << "\n"
       << " tv = \n    "
       << m_vme_header->m_tv_sec    << "\n    "
       << m_vme_header->m_tv_nsec
       << std::dec << std::noshowbase << std::endl;

  return;
}

//______________________________________________________________________________
bool
VmeModule::unpack()
{
  m_vme_header        = reinterpret_cast<Header*>(&(*m_data_first));
  m_module_data_first = m_data_first + k_header_size;
  //   print_vme_header();

  for (iterator_list::iterator i = m_first_list.begin(),
	 iEnd =m_first_list.end(); i!=iEnd; ++i)
    {
      iterator first = *i;
      Header* vme_header = reinterpret_cast<Header*>(&(*first));
      m_sec.push_back(vme_header->m_tv_sec);
      m_nsec.push_back(vme_header->m_tv_nsec);
    }      

  return true;
}

  }
}
