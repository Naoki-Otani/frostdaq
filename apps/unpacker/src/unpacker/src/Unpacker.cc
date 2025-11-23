// -*- C++ -*-

// Author: Tomonori Takahashi

#include "Unpacker.hh"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <stdexcept>
//#include <list>
#include <vector>

#include <cstdlib>

#include "std_ostream.hh"
#include "defines.hh"
#include "escape_sequence.hh"
#include "lexical_cast.hh"
// #include "HexDump.hh"
#include "UnpackerFactory.hh"
#include "UnpackerImpl.hh"
#include "UnpackerManager.hh"
#include "UnpackerConfig.hh"
#include "std_pair_dump.hh"
#include "Clear.hh"


namespace hddaq
{
  namespace unpacker
  {

    typedef std::map<uint64_t, Unpacker*> ChildList;

//______________________________________________________________________________
Unpacker::Unpacker(const unpacker_type& type)
  : m_impl(create_unpacker(type))
{
}

//______________________________________________________________________________
Unpacker::~Unpacker()
{
  if (m_impl)
    {
      delete m_impl;
      m_impl = 0;
    }
}

//______________________________________________________________________________
void
Unpacker::hoge(const std::string& arg) const
{
  cout << "#D Unpacker::hoge() " << m_impl << " " << arg << std::endl;
  if (m_impl)
    m_impl->hoge(arg);
  else
    cout << "#D null impl ::hoge() " << arg << std::endl;
  return;
}

//______________________________________________________________________________
void
Unpacker::add_channel_map(int fe_ch,
			  int fe_data,
			  DigitList& digit_list,
			  int device,
			  int plane,
			  int segment,
			  int ch,
			  int data_type)
{
  const int n_ch = m_impl->m_fe_data.size();
  Data& data
    = digit_list.at(device).at(plane).at(segment).at(ch).at(data_type);

  try
    {
      FECh& ch_ref          =  m_impl->m_fe_data.at(fe_ch);
      const int n_data_type = ch_ref.size();
      try
	{
	  ch_ref.at(fe_data) = &data;

	  Impl::DigitId& d
	    = m_impl->m_digit_id_list.at(fe_ch).at(fe_data);
	  d.m_device    = device;
	  d.m_plane     = plane;
	  d.m_segment   = segment;
	  d.m_ch        = ch;
	  d.m_data_type = data_type;
	  if (device==Impl::gm_null_device_id)
	    m_impl->m_null_ch.at(fe_ch).at(fe_data) = true;
	}
      catch (const std::out_of_range& e)
	{
	  cerr << "\n#E Unpacker::add_channel_map() "
	       << "\n what = " << e.what() << "\n "
	       << m_impl->m_name << "\n"
	       << " FrontEnd data type ID : " << fe_data
	       << " exceeds the size of FECh container: "
	       << n_data_type << "\n"
	       << "     check <FE> in <front_end>";
	  throw;
	}
    }
  catch (const std::out_of_range& e)
    {
      cerr << "\n#E Unpacker::add_channel_map() " << m_impl->m_name
	   << "\n what = " << e.what() << "\n "
	  << " FrontEnd channel ID : " << fe_ch
	  << " exceeds the size of FrontEndData container: "
	  << n_ch << "\n"
	  << "     check  <FE ch=..... > in <front_end>\n"
	  << "         or <sub ch_num=...> in <digit_data>";
      throw;
    }
  catch (const std::exception& e)
    {
      cerr  << "\n#E Unpacker::add_channel_map() " << m_impl->m_name
	    << "\n what = " << e.what()
	    << "\n exception occurred";
      throw;
    }
  catch (...)
    {
      cerr << "\n#E Unpacker::add_channel_map() " << m_impl->m_name
	  << "\n unknown error occurred";
      throw;
    }

  return;
}

//______________________________________________________________________________
void
Unpacker::add_channel_map(FrontEndData& fe_data,
			  DigitList& digit_list)
{
  if (fe_data.empty())
    fe_data.resize(m_impl->m_fe_data.size());
  const int n_ch  = fe_data.size();
  for (int ch=0; ch<n_ch; ++ch)
    {
      FECh& ch_ref = fe_data[ch];
      if (ch_ref.empty()){
	ch_ref.resize(m_impl->m_fe_data[ch].size());
      }

      const int n_type = ch_ref.size();
      for (int type=0; type<n_type; ++type)
	{
	  const Impl::DigitId& d
	    = m_impl->m_digit_id_list[ch][type];
	  ch_ref[type]
	    = &(digit_list
		[d.m_device]
		[d.m_plane]
		[d.m_segment]
		[d.m_ch]
		[d.m_data_type]);
	}
    }

  return;
}

//______________________________________________________________________________
void
Unpacker::check()
{
  if (!m_impl) return;

//   if (0 == m_impl->get_local_tag())
//   m_impl->print();
  if (m_impl->m_data_size==0) return;
  if (!m_impl->m_is_unpack_mode) return;

  ChildList::const_iterator cend = m_impl->m_child_list.end();
  for (ChildList::const_iterator i=m_impl->m_child_list.begin(); i!=cend; ++i)
    {
      Unpacker* u = i->second;
      if (u) u->check();
    }

  defines::check_mode_t& mode = Impl::gm_check_mode;
  m_impl->update_tag();

  if(mode[defines::k_format]){
    m_impl->check_data_format();
  }

  if(false
     || mode[defines::k_local]
     || mode[defines::k_event]
     || mode[defines::k_spill]
     ){
    m_impl->check_mismatch();
  }

  if(mode[defines::k_slip1]){
    m_impl->check_slip();
  }

  Unpacker* parent = m_impl->m_parent;
  if (parent){
    if (m_impl->m_error_state[defines::k_bad_bit]){
      Impl* parent_impl = parent->m_impl;
      parent_impl->m_error_state.set(defines::k_bad_bit);
    }
  }

  return;
}

//______________________________________________________________________________
void
Unpacker::clear()
{
  // called at the beginning of each event
//   cout << "#D Unpacker::clear() " << m_impl->m_name << std::endl;

  ChildList::iterator cend = m_impl->m_child_list.end();
  for (ChildList::iterator i=m_impl->m_child_list.begin(); i!=cend; ++i)
    {
      Unpacker* u = i->second;
      if (u)
	{
 	  u->clear();
	}
    }

  if (!m_impl) return;

  if (!is_good()) m_impl->m_error_state.reset();


//   Tag& pre = m_impl->m_tag[k_tag_prev];
  m_impl->m_tag[k_tag_prev].swap(m_impl->m_tag[k_tag_current]);

//   fill_tag(m_impl->m_tag[k_tag_current], defines::k_unassigned);
  m_impl->m_tag[k_tag_current].resize(1);

  m_impl->m_has_tag.reset();
  m_impl->m_word_count = 0;
  m_impl->m_data_size  = 0;
  m_impl->m_sec.clear();
  m_impl->m_nsec.clear();
  m_impl->clear();

  FrontEndData& fe_data = *(m_impl->m_back);
  const int n_ch = fe_data.size();
  for (int i=0; i<n_ch; ++i)
    {
      FECh& ch = fe_data[i];
      const int n_data = ch.size();
//       cout << "#D " << m_impl->m_name << "::clear() n data = " << n_data
// 	   << std::endl;
      for (int j=0; j<n_data; ++j)
	if (ch[j])
// 	  ch[j]->clear();
	  hddaq::clear(*(ch[j]));
// 	else
// 	  {
// 	    cerr << "\n#E Unpacker::clear()\n unpacker " << m_impl->m_type
// 		 << " : "
// 		 << m_impl->m_name << " has null pointer to ch ="
// 		 << i << ", data type = " << j << std::endl;
// 	  }

    }

  m_impl->m_data_last = m_impl->m_data_first;
  m_impl->m_first_list.clear();
  m_impl->m_last_list.clear();
  return;
}

//______________________________________________________________________________
void
Unpacker::decode()
{
  if (!m_impl) return;
//   cout << "#D Unpacker::decode() "
//        << m_impl->m_name << " "
//        << m_impl->m_is_unpack_mode << " "
//        << m_impl->m_is_decode_mode << std::endl;
  if (m_impl->m_data_size==0) return;

  if (!m_impl->m_is_unpack_mode) return;


  if (m_impl->m_is_decode_mode){
    bool data_format_is_bad = false;
    data_format_is_bad =
       is_bad(defines::k_data_size_bit)
      |is_bad(defines::k_header_bit)
      |is_bad(defines::k_trailer_bit);
    if(!data_format_is_bad){
      m_impl->decode();
    }else{
      cerr << "#E Unpacker::decode Skip the decode process"
	   << " because data format is probaly broken"
	   << std::endl;
      m_impl->dump_in_hexadecimal();
    }
  }

//   cout << m_impl->m_child_list.size() << std::endl;
  ChildList::iterator cend   = m_impl->m_child_list.end();
  for (ChildList::iterator i = m_impl->m_child_list.begin(); i!=cend; ++i)
    {
      Unpacker* u = i->second;
      if (u)
	{
 	  //cout << "#D " << u->get_name() << std::endl;
	  u->decode();
	}
    }

  return;
}

//______________________________________________________________________________
void
Unpacker::dump_data(const Impl& impl,
		    defines::e_dump_mode mode)
{
  if (mode == defines::k_binary)
    impl.dump_in_binary();
  else if (mode == defines::k_dec)
    impl.dump_in_decimal();
  else if (mode == defines::k_hex)
    impl.dump_in_hexadecimal();

//   cout << "#D " << impl.m_name << std::endl;
//   std::for_each(impl.m_data_first, impl.m_data_last, HexDump());
  return;
}

//______________________________________________________________________________
void
Unpacker::dump_data(defines::e_dump_mode mode) const
{
  if (!m_impl)
    return;

  if (false
      || m_impl->m_dump_mode[defines::k_binary]
      || mode == defines::k_binary
      ){
    m_impl->dump_in_binary();
  }

  if (false
      || m_impl->m_dump_mode[defines::k_dec]
      || mode == defines::k_dec
      ){
    m_impl->dump_in_decimal();
  }

  if (false
      || m_impl->m_dump_mode[defines::k_hex]
      || mode == defines::k_hex
      ){
    m_impl->dump_in_hexadecimal();
  }

  ChildList::const_iterator cend = m_impl->m_child_list.end();
  for (ChildList::const_iterator i=m_impl->m_child_list.begin(); i!=cend; ++i)
    {
      Unpacker* u = i->second;
      if (u){ u->dump_data(); }
    }
  return;
}

//______________________________________________________________________________
void
Unpacker::fill(unsigned int ch,
	       unsigned int data_type,
	       unsigned long long data)
{
  m_impl->fill(ch, data_type, data);
  return;
}

//______________________________________________________________________________
unsigned long long
Unpacker::get(unsigned int ch,
	      unsigned int data_type,
	      unsigned int hit_id) const
{
#ifndef RANGE_CHECK
  return (*(m_impl->m_fe_data[ch][data_type]))[hit_id];
#else
  try
    {
      return m_impl->m_fe_data.at(ch).at(data_type)->at(hit_id);
    }
  catch (const std::out_of_range& e)
    {
      std::stringstream err;
      try
	{
	  err << "[" << ch
	      << " / " << m_impl->m_fe_data.size()  << "]";
	  err << "[" << data_type
	      << " / " << m_impl->m_fe_data.at(ch).size() << "]";
	  err << "[" << hit_id
	      << " / " << m_impl->m_fe_data.at(ch).at(data_type)->size() << "]";
	  throw;
	}
      catch (const std::out_of_range& e1)
	{
	  cerr << "#E Unpacker::get() " << m_impl->m_type
	       << " invalid access " << err.str()  << " "
	       << e.what() << std::endl;
	}
    }
#endif
  return defines::k_unassignedULL;
}

//______________________________________________________________________________
Unpacker::Impl&
Unpacker::get() const
{
  return *m_impl;
}

//______________________________________________________________________________
void
Unpacker::get_buffer(iterator& begin,
		     iterator& end) const
{
  begin = m_impl->m_data_first;
  end   = m_impl->m_data_last;
  return;
}

//______________________________________________________________________________

Unpacker::iterator
Unpacker::get_buffer_begin() const
{
  return m_impl->m_data_first;
}

//______________________________________________________________________________

Unpacker::iterator
Unpacker::get_buffer_end() const
{
  return m_impl->m_data_last;
}

//______________________________________________________________________________
const defines::check_mode_t&
Unpacker::get_check_mode() const
{
  return Impl::gm_check_mode;
}

//______________________________________________________________________________
Unpacker*
Unpacker::get_child(uint64_t id) const
{
  return m_impl->get_child(id);
}

//______________________________________________________________________________
const std::map<uint64_t, Unpacker*>&
Unpacker::get_child_list() const
{
  return m_impl->m_child_list;
}

//______________________________________________________________________________
const hddaq::unpacker::FrontEndData&
Unpacker::get_data() const
{
  return m_impl->m_fe_data;
}

//______________________________________________________________________________
int
Unpacker::get_data_id(const std::string& name) const
{
  const std::map<std::string, int>& data_ref   = m_impl->get_data_id();
  std::map<std::string, int>::const_iterator i = data_ref.find(name);
  if (i==data_ref.end())
    {
      cout << "\n#E Unpacker::get_data_id() "
	   << m_impl->m_type << "\n"
	   << " cannot find " << name << "\n";
      std::for_each(data_ref.begin(), data_ref.end(), PairDump());
      std::exit(1);
    }
  return i->second;
}

//______________________________________________________________________________
unsigned int
Unpacker::get_data_size() const
{
  return m_impl->m_data_size;
}

//______________________________________________________________________________
unsigned int
Unpacker::get_dump_mode() const
{
  return m_impl->m_dump_mode.to_ulong();
}

//______________________________________________________________________________
unsigned int
Unpacker::get_entries(unsigned int ch,
		      unsigned int data_type) const
{
#ifndef RANGE_CHECK
  return m_impl->m_fe_data[ch][data_type]->size();
#else
  try
    {
      return m_impl->m_fe_data.at(ch).at(data_type)->size();
    }
  catch (const std::out_of_range& e)
    {
      std::stringstream err;
      try
	{
	  err << "[" << ch
	      << " / " << m_impl->m_fe_data.size()  << "]";
	  err << "[" << data_type
	      << " / " << m_impl->m_fe_data.at(ch).size() << "]";
	  throw;
	}
      catch (const std::out_of_range& e1)
	{
	  cerr << "#E Unpacker::get_entries() " << m_impl->m_type
	       << " invalid access " << err.str() << " "
	       << e.what() << std::endl;
	}
    }
#endif
  return defines::k_unassignedU;
}


//______________________________________________________________________________
const Unpacker::error_state_t&
Unpacker::get_error_state()
{
  m_impl->m_error_state.reset(defines::k_good_bit);

  if (m_impl->m_error_state.none())
    m_impl->m_error_state.set(defines::k_good_bit);
  else
    m_impl->m_error_state.set(defines::k_bad_bit);
  return m_impl->m_error_state;
}

//______________________________________________________________________________
unsigned int
Unpacker::get_fe_info(int data_type) const
{
  return m_impl->get_fe_info(data_type);
}

//______________________________________________________________________________
unsigned int
Unpacker::get_header(int data_type) const
{
  return m_impl->get_header(data_type);
}

//______________________________________________________________________________
uint64_t
Unpacker::get_id() const
{
  return m_impl->m_id;
}

//______________________________________________________________________________
const std::string&
Unpacker::get_name() const
{
  return m_impl->m_name;
}

//______________________________________________________________________________
Unpacker*
Unpacker::get_parent() const
{
  return m_impl->m_parent;
}

//______________________________________________________________________________
int
Unpacker::get_run_number() const
{
//   cout << "#D Unpacker " << m_impl->m_name << " "
//        << m_impl->m_run_number << std::endl;

  return m_impl->m_run_number;
}

//______________________________________________________________________________
const Tag&
Unpacker::get_tag(int tag_type) const
{
  return m_impl->m_tag[tag_type].back();
}

//______________________________________________________________________________
void
Unpacker::get_tag(std::vector<Tag>& tag_current) const
{
  tag_current.assign(m_impl->m_tag[k_tag_current].begin(),
		     m_impl->m_tag[k_tag_current].end());
  return;
}

//______________________________________________________________________________
void
Unpacker::get_time(std::vector<uint64_t>& sec,
		   std::vector<uint64_t>& nsec) const
{
  sec.assign(m_impl->m_sec.begin(), m_impl->m_sec.end());;
  nsec.assign(m_impl->m_nsec.begin(), m_impl->m_nsec.end());
  return;
}

//______________________________________________________________________________
void
Unpacker::get_time(uint64_t& sec,
		   uint64_t& nsec) const
{
  if (!m_impl->m_sec.empty())
    {
      sec  = m_impl->m_sec.back();
      nsec = m_impl->m_nsec.back();
    }
  return;
}

//______________________________________________________________________________
const Unpacker::unpacker_type&
Unpacker::get_type() const
{
//   hoge();
//   cout << "#D type = " << m_impl->m_type << std::endl;
  return m_impl->m_type;
}

//______________________________________________________________________________
unsigned int
Unpacker::get_word_count() const
{
  return m_impl->m_word_count;
}

//______________________________________________________________________________
bool
Unpacker::has_tag(int t) const
{
  return m_impl->m_has_tag[t];
}

//______________________________________________________________________________
bool
Unpacker::is_bad(int err_type) const
{
  return m_impl->m_error_state[err_type];
}

//______________________________________________________________________________
bool
Unpacker::is_decode_mode() const
{
  return m_impl->m_is_decode_mode;
}

//______________________________________________________________________________
bool
Unpacker::is_esc_on() const
{
  return Impl::gm_is_esc_on;
}

//______________________________________________________________________________
bool
Unpacker::is_good()
{
  get_error_state();
  return m_impl->m_error_state[defines::k_good_bit];
}

//______________________________________________________________________________
bool
Unpacker::is_ready() const
{
  return m_impl->is_ready();
}

//______________________________________________________________________________
bool
Unpacker::is_related(int device_id, int plane_id) const
{
  return m_impl->is_related(device_id, plane_id);
}

//______________________________________________________________________________
bool
Unpacker::is_node() const
{
  return m_impl->m_is_node;
}

//______________________________________________________________________________
bool
Unpacker::is_unpack_mode() const
{
  return m_impl->m_is_unpack_mode;
}

//______________________________________________________________________________
void
Unpacker::print() const
{
  cout << "#D Unpacker ";
  cout << " ID (hex) = ";
  cout << std::hex <<  m_impl->m_id << std::dec << ", ";
  cout << " Name = " << m_impl->m_name;
  cout << std::endl;
  ChildList::const_iterator cend = m_impl->m_child_list.end();
  for (ChildList::const_iterator i=m_impl->m_child_list.begin(); i!=cend; ++i)
    {
      Unpacker* u = i->second;
      if (u)
	{
	  u->print();
	}
    }
  return;
}

//______________________________________________________________________________
void
Unpacker::print_tag() const
{
  if (m_impl->m_has_tag.any())
    {
      const Tag& tag  = m_impl->m_tag[k_tag_current].back();
      const Tag& prev = m_impl->m_tag[k_tag_prev].back();
      cout.flush();
      cout << "#D " << m_impl->m_name << "\n"
	   << "    local = " << tag.m_local << ", "
	   << "event = "     << tag.m_event << ", "
	   << "spill = "     << tag.m_spill << "\n"
	   << "    prev local = " << prev.m_local << ", "
	   << "prev event = "     << prev.m_event << ", "
	   << "prev spill = "     << prev.m_spill << "\n";
      cout.flush();
    }
  ChildList::const_iterator cend = m_impl->m_child_list.end();
  for (ChildList::const_iterator i=m_impl->m_child_list.begin(); i!=cend; ++i)
    {
      Unpacker* u = i->second;
      if (u) u->print_tag();
    }
  return;
}

//______________________________________________________________________________
void
Unpacker::reset()
{
  // called at the beginning of each run
  ChildList::iterator cend = m_impl->m_child_list.end();
  for (ChildList::iterator i=m_impl->m_child_list.begin(); i!=cend; ++i)
    {
      Unpacker* u = i->second;
      if (u)
	u->reset();
    }

//   fill_tag(m_impl->m_tag[k_tag_prev], defines::k_unassigned);
//   fill_tag(m_impl->m_tag[k_tag_current], defines::k_unassigned);
  m_impl->m_tag[k_tag_prev].resize(1);
  m_impl->m_tag[k_tag_current].resize(1);

//   m_impl->m_tag[k_tag_current] = m_impl->m_tag[k_tag_origin];
  m_impl->m_run_number = defines::k_unassigned;
  m_impl->reset();
  return;
}

//______________________________________________________________________________
void
Unpacker::reset_dump_mode(unsigned int dump_mode)
{
  m_impl->m_dump_mode.reset(dump_mode);
  return;
}

//______________________________________________________________________________
void
Unpacker::resize_fe_data()
{
  m_impl->resize_fe_data();

  FrontEndData& fe = m_impl->m_fe_data;
  const int n = fe.size();

  Impl::DigitIdList& d = m_impl->m_digit_id_list;
  d.resize(n);

  Impl::NullChList& null = m_impl->m_null_ch;
  null.resize(n);

  for (int i=0; i<n; ++i)
    {
      d[i].resize(fe[i].size());
      null[i].resize(fe[i].size());
    }

  return;
}

//______________________________________________________________________________
void
Unpacker::set_check_mode(const check_mode_t& mode,
			 const std::string& char_true,
			 const std::string& char_false)
{
  cout << "#D check mode = " << mode << std::endl;
  Impl::gm_check_mode = mode;
  if (Impl::gm_true_bit.empty())
    Impl::gm_true_bit  = (char_true.empty() ? "1" : char_true);
  if (Impl::gm_false_bit.empty())
    Impl::gm_false_bit = (char_false.empty() ? "0" : char_false);
  return;
}

//______________________________________________________________________________
void
Unpacker::set_container()
{
  m_impl->m_back = &(m_impl->m_fe_data);
  return;
}

//______________________________________________________________________________
void
Unpacker::set_container(FrontEndData& fe_data)
{
  m_impl->m_back = &fe_data;
  return;
}

//______________________________________________________________________________
void
Unpacker::set_data(unsigned int data)
{
  m_impl->set_data(data);
  return;
}

//______________________________________________________________________________
void
Unpacker::set_data(const iterator& data_first,
		   const iterator& data_last)
{
//   cout << "#D set_data()"  << std::endl;
//   hoge();
  m_impl->set_data(data_first, data_last);
  return;
}

//______________________________________________________________________________
void
Unpacker::set_data_size(unsigned int data_size)
{
  m_impl->set_data_size(data_size);
  return;
}

//______________________________________________________________________________
void
Unpacker::set_decode_mode(bool decode_mode)
{
  m_impl->m_is_decode_mode = decode_mode;
  return;
}

//______________________________________________________________________________
void
Unpacker::set_dump_mode(unsigned int dump_mode)
{
  m_impl->m_dump_mode.set(dump_mode);
  return;
}

//______________________________________________________________________________

void
Unpacker::set_esc(bool flag)
{
  Impl::gm_is_esc_on = flag;
  return;
}

//______________________________________________________________________________
void
Unpacker::set_id(uint64_t id)
{
  m_impl->m_id = id;
  return;
}

//______________________________________________________________________________
void
Unpacker::set_impl(Impl* impl)
{
  if (m_impl)
    {
      delete m_impl;
      m_impl = 0;
    }
  m_impl = impl;
  return;
}

//______________________________________________________________________________
void
Unpacker::set_name(const std::string& name)
{
//   cout << " set name = " << name;
  if (!m_impl)
    return;

  if (!name.empty())
    m_impl->m_name = name;
  else
    {
      if (m_impl->m_is_node)
	{
	  cerr << "\n#E node needs hostname or nickname" << std::endl;
	  return;
	}
      std::stringstream s;
      s << std::hex
// 	<< std::showbase
	<< "0x"
	<< m_impl->m_id;
      m_impl->m_name = m_impl->m_parent->m_impl->m_name + "_" + s.str();
    }
//   cout << " : " << m_impl->m_name << "  created" << std::endl;
  return;
}

//______________________________________________________________________________
void
Unpacker::set_null_device_id(int device_id)
{
  Impl::gm_null_device_id = device_id;
  return;
}

//______________________________________________________________________________
void
Unpacker::set_parent(Unpacker* parent)
{
  if (parent)
    parent->m_impl->m_child_list[m_impl->m_id] = this;
  m_impl->m_parent = parent;
  return;
}

//______________________________________________________________________________
void
Unpacker::set_state(defines::e_error_state error_bit)
{
  m_impl->m_error_state.set(error_bit);
  return;
}

//______________________________________________________________________________
void
Unpacker::set_unpack_mode(bool flag)
{
  m_impl->m_is_unpack_mode = flag;
  return;
}

//______________________________________________________________________________
void
Unpacker::show_summary()
{
  if (!m_impl) return;

  is_good();
  if (m_impl->m_has_tag.none())  return;
  if (!m_impl->m_is_unpack_mode) return;

  bool has_local = m_impl->m_has_tag[k_local];
  bool has_event = m_impl->m_has_tag[k_event];
  bool has_spill = m_impl->m_has_tag[k_spill];

  bool inherit_event = m_impl->m_tag_status[k_event_inherit];
  bool inherit_spill = m_impl->m_tag_status[k_spill_inherit];
  if (true
      && !Impl::gm_check_mode[defines::k_show_all]
      && (!m_impl->m_is_node)
      && (!is_bad(defines::k_bad_bit)))
    {
      return;
    }


  std::string state = m_impl->m_error_state.to_string();
  std::replace(state.begin(), state.end(), '1', Impl::gm_true_bit[0]);
  std::replace(state.begin(), state.end(), '0', Impl::gm_false_bit[0]);

  const Tag& tag    = m_impl->m_tag[k_tag_current].back();
  const Tag& prev   = m_impl->m_tag[k_tag_prev].back();
  const Tag& tag_org= m_impl->m_tag[k_tag_origin].back();

  Tag masked;
  masked.m_local = tag.m_local & k_SMP_MAX_LOCAL_TAG;
  masked.m_event = tag.m_event & k_FINESSE_MAX_EVENT_TAG;
  masked.m_spill = tag.m_spill & k_FINESSE_MAX_SPILL_TAG;
  std::string local_tag  = i2a(tag.m_local - tag_org.m_local);
  std::string local_prev = i2a(prev.m_local - tag_org.m_local);

  std::string event_tag  = i2a(tag.m_event) + "(" + i2a(masked.m_event) + ")";
  std::string event_prev
    = i2a(prev.m_event) + "("
    + i2a(prev.m_event & k_FINESSE_MAX_EVENT_TAG) + ")";

  std::string spill_tag  = i2a(tag.m_spill) + "(" + i2a(masked.m_spill) + ")";
  std::string spill_prev
    = i2a(prev.m_spill) + "("
    + i2a(prev.m_spill & k_FINESSE_MAX_SPILL_TAG) + ")";

  tag_summary.setf(std::ios_base::left);
  // name
  tag_summary << std::setfill(' ')
	      << std::setw(24) << m_impl->m_name;
  tag_summary.unsetf(std::ios_base::left);
  tag_summary.setf(std::ios_base::right);
  // error state
  tag_summary << ((is_bad() && Impl::gm_is_esc_on)
		  ? esc::k_yellow : "")
	      << std::setw(defines::k_n_error_state + 3)
    // 		<< m_impl->m_error_state
	      << state
    // data size
	      << std::setw(7)  << m_impl->m_data_size << " "
    // local tag
	      << std::setw(12) << (has_local ? local_tag : "") << " "
    // event tag
	      << ((inherit_event && Impl::gm_is_esc_on) ? esc::k_light_blue : "")
	      << std::setw(12) << (has_event ? event_tag : "") << " "
	      << (Impl::gm_is_esc_on ? esc::k_default_color : "")
    // spill tag
	      << ((inherit_spill && Impl::gm_is_esc_on) ? esc::k_light_blue : "")
	      << std::setw(12) << (has_spill ? spill_tag : "") << " "
	      << (Impl::gm_is_esc_on ? esc::k_default_color : "")
#if 0
    // local tag (previous event)
	      << std::setw(12) << (has_local ? local_prev : "") << " "
    // event tag (previous event)
	      << std::setw(12) << (has_event ? event_prev : "") << " "
    // spill tag (previous event)
	      << std::setw(12) << (has_spill ? spill_prev : "")
#endif
	      << (Impl::gm_is_esc_on ? esc::k_default_color : "")
	      << "\n";
  tag_summary.unsetf(std::ios_base::right);

  ChildList::const_iterator cend = m_impl->m_child_list.end();
  for (ChildList::const_iterator i=m_impl->m_child_list.begin(); i!=cend; ++i)
    {
      Unpacker* u = i->second;
      if (u)
	u->show_summary();
    }
  return;
}

//______________________________________________________________________________
void
Unpacker::sort_child()
{
//   cout << "#D Unpacker::sort_child()   " << m_impl->m_name << std::endl;

  m_impl->sort_child();
  ChildList::iterator cend = m_impl->m_child_list.end();
  for (ChildList::iterator i=m_impl->m_child_list.begin(); i!=cend; ++i)
    {
      Unpacker* u = i->second;
      if (u)
	u->sort_child();
    }

  return;
}

//______________________________________________________________________________
bool
Unpacker::unpack()
{
  if (!m_impl) return false;

  //  cout << "#D " << m_impl->m_name << "::unpack()" << std::endl;
  if (m_impl->m_is_unpack_mode){
    return m_impl->unpack();
  }else{
    return true;
  }
}


  }
}
