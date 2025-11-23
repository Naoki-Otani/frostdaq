// -*- C++ -*-

// Author: Tomonori Takahashi

#include "UnpackerManager.hh"

#include <algorithm>
// #include <filesystem>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <stdexcept>
#include <cstdlib>
#include <sstream>

#include "std_ostream.hh"
#include "Clear.hh"
#include "Delete.hh"
#include "HexDump.hh"
#include "Tokenizer.hh"
#include "Unpacker.hh"
#include "UnpackerRegister.hh"
#include "IStreamRegister.hh"
#include "escape_sequence.hh"
#include "lexical_cast.hh"
#include "UnpackerConfig.hh"
#include "UnpackerXMLReadDigit.hh"
#include "UnpackerXMLChannelMap.hh"
#include "EventReader.hh"
#include "filesystem_util.hh"
#include "replace_string.hh"

#include "MemFuncThread.hh"

namespace hddaq
{
  namespace unpacker
  {

//_____________________________________________________________________________
UnpackerManager::UnpackerManager()
  : m_reader(new EventReader),
    m_input_stream(),
    m_unpacker(),
    m_root(0),
    m_is_good(true),
    m_is_esc_on(false),
    m_skip(0),
    m_max_loop(-1),
    m_print_cycle(-1),
    m_digit_list(),
    m_fifo(),
    m_front(0),
    m_is_data_ready(false),
    m_counter(0),
    m_decode_mode(true),
    m_dump_mode(),
    m_run_number(-1),
    m_enable_istream_bookmark(false)
{
  UnpackerRegister unpacker_register;
  IStreamRegister  istream_register;
}

//_____________________________________________________________________________
UnpackerManager::~UnpackerManager()
{
  // cout << "#D1 GUnpacker::~UnpackerManager()" << std::endl;
  std::for_each(m_unpacker.begin(), m_unpacker.end(), Delete());

  // cout << "#D unpackers are cleared." << std::endl;
  // if (m_is_esc_on)
  //   tag_summary << esc::k_default_color << std::endl;
  // cout << "#D GUnpacker::Impl::~Impl()" << std::endl;
  // cout << "#D2 GUnpacker::~UnpackerManager()" << std::endl;

  if (m_reader)
    delete m_reader;
  m_reader = 0;
}

//_____________________________________________________________________________
void
UnpackerManager::hoge(const std::string& arg) const
{
  cout << "#D GUnpacker::hoge()  " << arg << std::endl;
  return;
}

//_____________________________________________________________________________
void
UnpackerManager::check()
{
//   cout << "#D GUnpacker::check()" << std::endl;
  if (m_root) m_root->check();
  return;
}

//_____________________________________________________________________________
void
UnpackerManager::clear()
{
//   cout << "#D1 GUnpacker clear" << std::endl;

#ifdef DEBUG_FIFO
  static int s_n_back = 0;
  cout << "#D1 UnpackerManager back()" << std::endl;
#endif

  const int n = m_unpacker.size();
  if (m_fifo.size()>1)
    {
      EventBuffer& wpos = m_fifo.back();

      for (int i=0; i<n; ++i)
	{
	  Unpacker* u = m_unpacker[i];
	  if (u) u->set_container(wpos.m_fe[i]);
	}
    }
  else
    {
      for (int i=0; i<n; ++i)
	{
	  Unpacker* u = m_unpacker[i];
	  if (u) u->set_container();
	}
      m_is_data_ready = false;
    }

#ifdef DEBUG_FIFO
  ++s_n_back;
  cout << "#D2 UnpackerManager back() " << s_n_back << std::endl;
#endif

  if (m_root) m_root->clear();
  m_is_good = true;

  return;
}

//_____________________________________________________________________________
void
UnpackerManager::close()
{
  m_reader->close();
  return;
}

//_____________________________________________________________________________
Unpacker*
UnpackerManager::create(const unpacker_type& type)
{

  Unpacker* u = new Unpacker(type);
  m_unpacker.push_back(u);
  return u;
}

//_____________________________________________________________________________
void
UnpackerManager::decode()
{
  if (m_decode_mode) m_root->decode();
  push_event();
  return;
}

//_____________________________________________________________________________
void
UnpackerManager::disable_istream_bookmark()
{
  m_enable_istream_bookmark = false;
  return;
}

//_____________________________________________________________________________
void
UnpackerManager::dump_data() const
{
  if (m_dump_mode[defines::k_binary]){
    m_reader->dump_in_binary();
  }else if (m_dump_mode[defines::k_dec]){
    m_reader->dump_in_decimal();
  }else if (m_dump_mode[defines::k_hex]){
    m_reader->dump_in_hexadecimal();
  }

  if (m_root) m_root->dump_data();
  return;
}

//_____________________________________________________________________________
void
UnpackerManager::dump_data_device(int device_id, int plane_id) const
{
  UnpackerConfig& uCon = GConfig::get_instance();
  const UnpackerConfig::DigitInfo::NameList& nlist
    = (uCon.get_digit_info()).get_name_list();
  const std::string name_device = nlist[device_id];
  const std::string name_plane  = plane_id == -1 ? "all planes" : i2a(plane_id);

  std::cout << "#D UnpackerManager::dump_data_device" << std::endl;
  std::cout << " Front-end data relating to Device["
	    << name_device << "] Plane["
	    << name_plane << "]"
	    << std::endl;

  std::vector<Unpacker*> uvect;
  search_device(get_root(), uvect, device_id, plane_id);
  for(unsigned int i = 0; i<uvect.size(); ++i){
    uvect[i]->dump_data(defines::k_hex);
  }

  return;
}

//_____________________________________________________________________________
void
UnpackerManager::dump_data_fe(int node_id, int address, int ma) const
{
  Unpacker* parent = get_root();
  if((int)parent->get_id() == node_id){
    return parent->dump_data(defines::k_hex);
  }

  Unpacker* result = NULL;
  result = search_fe(parent, node_id);
  if(address != -1 && result){
    parent = result;
    result = search_fe(parent, address);
    if(ma != -1 && result){
      parent = result;
      result = search_fe(parent, ma);
    }
  }

  std::cout << "#D UnpackerManager::dump_data_fe" << std::endl;
  if(result){
    result->dump_data(defines::k_hex);
  }else{
    std::cout << "#E UnpackerManager::dump_data_fe\n";
    std::cout << std::hex
	      << " Frond-end with node: " << node_id
	      << ", address: " << address
	      << ", ma: " << ma << "\n"
	      << " is not found"
	      << std::dec << std::endl;
  }

  return;
}

//_____________________________________________________________________________
bool
UnpackerManager::empty() const
{
  return (m_fifo.get_entries()==0);
}

//_____________________________________________________________________________
void
UnpackerManager::enable_istream_bookmark()
{
  m_enable_istream_bookmark = true;
  return;
}

//_____________________________________________________________________________
bool
UnpackerManager::eof() const
{
  bool is_end = (0==m_max_loop) || (m_max_loop==m_counter);
  if (is_end)
    {
      cout << "#D GUnpacker::eof()  reached max loop\n";
      if (m_reader->is_open())
	{
	  cout << "#D GUnpacker close stream" << std::endl;
	  const_cast<EventReader*>(m_reader)->close();
	}
    }
  if (m_reader->eof())
    cout << "#D GUnpacker::eof()  reader reached end\n";
  bool ret = is_end || m_reader->eof() ||!m_reader->is_open();
  if (ret)
    {
      cout << "    n unpacked =  " << m_counter << " ("
	   << std::hex << std::showbase
	   << m_counter << ")" << std::dec << std::noshowbase
	   << std::endl;
    }
  return ret;
}

//_____________________________________________________________________________
const Device&
UnpackerManager::find(unsigned int device_id) const
{
  const DigitList& digit_list = *m_front;
  try
    {
      return digit_list.at(device_id);
    }
  catch (...)
    {
      cerr << "\n#E UnpackerManager::find()\n"
	   << " device id = " << device_id
	   << std::endl;
      throw;
    }

}

//_____________________________________________________________________________
const Device&
UnpackerManager::find(const char* name) const
{
  const std::string name_str = name;
  return find(name_str);
}

//_____________________________________________________________________________
const Device&
UnpackerManager::find(const std::string& name) const
{
  try
    {
      UnpackerConfig& g_config = GConfig::get_instance();
      const UnpackerConfig::DigitInfo& digit_info = g_config.get_digit_info();

      return find(digit_info.get_device_id(name));
    }
  catch (...)
    {
      cerr << "\n#E UnpackerManager::find()\n"
	   << " device name = " << name
	   << std::endl;
      throw;
    }
}

//_____________________________________________________________________________
const Plane&
UnpackerManager::find(unsigned int device_id, unsigned int plane_id) const
{
  const DigitList& digit_list = *m_front;
  try
    {
      return (digit_list.at(device_id)).at(plane_id);
    }
  catch (...)
    {
      cerr << "\n#E UnpackerManager::find()\n"
	   << " device id = " << device_id
	   << std::endl;
      throw;
    }

}

//_____________________________________________________________________________
const Plane&
UnpackerManager::find(const char* device_name, const char* plane_name) const
{
  const std::string device_name_str = device_name;
  const std::string plane_name_str  = plane_name;
  return find(device_name_str, plane_name_str);
}

//_____________________________________________________________________________
const Plane&
UnpackerManager::find(const std::string& device_name,
		      const std::string& plane_name ) const
{
  try
    {
      UnpackerConfig& g_config = GConfig::get_instance();
      const UnpackerConfig::DigitInfo& digit_info = g_config.get_digit_info();

      unsigned int device_id = digit_info.get_device_id(device_name);
      unsigned int plane_id  = digit_info.get_plane_id(device_id, plane_name);

      return find(device_id, plane_id);
    }
  catch (...)
    {
      cerr << "\n#E UnpackerManager::find()\n"
	   << " device name = " << device_name
	   << std::endl;
      throw;
    }
}

//_____________________________________________________________________________
unsigned long long
UnpackerManager::get(unsigned int device_id,
		     unsigned int plane_id,
		     unsigned int segment_id,
		     unsigned int ch,
		     unsigned int data_type,
		     unsigned int hit_id) const
{
  const DigitList& digit_list = *m_front;
  return digit_list[device_id][plane_id][segment_id][ch][data_type][hit_id];
}

//_____________________________________________________________________________
unsigned long long
UnpackerManager::get(const char* name,
		     unsigned int plane_id,
		     unsigned int segment_id,
		     unsigned int ch,
		     unsigned int data_type,
		     unsigned int hit_id) const
{
  const std::string device_name_str = name;
  return get(device_name_str, plane_id, segment_id, ch, data_type, hit_id);
}

//_____________________________________________________________________________
unsigned long long
UnpackerManager::get(const std::string& name,
		     unsigned int plane_id,
		     unsigned int segment_id,
		     unsigned int ch,
		     unsigned int data_type,
		     unsigned int hit_id) const
{
  const Device& device = find(name);
  return device[plane_id][segment_id][ch][data_type][hit_id];
}

//_____________________________________________________________________________
unsigned long long
UnpackerManager::get(const char* device_name,
		     const char* plane_name,
		     unsigned int segment_id,
		     unsigned int ch,
		     unsigned int data_type,
		     unsigned int hit_id) const
{
  const std::string device_name_str = device_name;
  const std::string plane_name_str  = plane_name;
  return get(device_name_str, plane_name_str, segment_id, ch, data_type, hit_id);
}

//_____________________________________________________________________________
unsigned long long
UnpackerManager::get(const std::string& device_name,
		     const std::string& plane_name,
		     unsigned int segment_id,
		     unsigned int ch,
		     unsigned int data_type,
		     unsigned int hit_id) const
{
  const Plane& plane = find(device_name, plane_name);
  return plane[segment_id][ch][data_type][hit_id];
}

//_____________________________________________________________________________
void
UnpackerManager::get_buffer(const std::vector<uint64_t>& fe_id,
			    iterator& begin,
			    iterator& end) const
{
  Unpacker* u = m_root;
  int n = fe_id.size();
  for (int i=0; i<n; ++i)
    {
      Unpacker* child = u->get_child(fe_id[i]);
      if (child)
	u = child;
    }
  u->get_buffer(begin, end);
  return;
}

//_____________________________________________________________________________
UnpackerManager::iterator
UnpackerManager::get_buffer_begin(const std::vector<uint64_t>& fe_id) const
{
  Unpacker* u = m_root;
  int n = fe_id.size();
  for (int i=0; i<n; ++i)
    {
      Unpacker* child = u->get_child(fe_id[i]);
      if (child)
	u = child;
    }
  return u->get_buffer_begin();
}

//_____________________________________________________________________________
UnpackerManager::iterator
UnpackerManager::get_buffer_end(const std::vector<uint64_t>& fe_id) const
{
  Unpacker* u = m_root;
  int n = fe_id.size();
  for (int i=0; i<n; ++i)
    {
      Unpacker* child = u->get_child(fe_id[i]);
      if (child)
	u = child;
    }
  return u->get_buffer_end();
}

//_____________________________________________________________________________
unsigned int
UnpackerManager::get_counter() const
{
  return m_counter;
}

//_____________________________________________________________________________
unsigned int
UnpackerManager::get_daq_root_number() const
{
  return m_reader->get_daq_root_event_number();
}

//_____________________________________________________________________________
int
UnpackerManager::get_fe_id(const char* name) const
{
  std::string name_str = name;
  return get_fe_id(name_str);
}

//_____________________________________________________________________________
unsigned int
UnpackerManager::get_fe_info(int node_id, int address, int ma,
			     int data_type) const
{
  Unpacker* parent = get_root();
  if((int)parent->get_id() == node_id){
    return parent->get_fe_info(data_type);
  }

  Unpacker* result = NULL;
  result = search_fe(parent, node_id);
  if(address != -1 && result){
    parent = result;
    result = search_fe(parent, address);
    if(ma != -1 && result){
      parent = result;
      result = search_fe(parent, ma);
    }
  }

  if(result){
    return result->get_fe_info(data_type);
  }else{
    // std::cout << "#E UnpackerManager::get_fe_info\n";
    // std::cout << std::hex
    // 	      << " Frond-end with node: " << node_id
    // 	      << ", address: " << address
    // 	      << ", ma: " << ma << "\n"
    // 	      << " is not found"
    // 	      << std::dec << std::endl;
  }

  return 0;
}

//_____________________________________________________________________________
int
UnpackerManager::get_fe_id(const std::string& name) const
{
  return GConfig::get_instance().get_channel_map().get_fe_id(name);
}

//_____________________________________________________________________________
int
UnpackerManager::get_device_id(const char* name) const
{
  std::string name_str = name;
  return get_device_id(name_str);
}

//_____________________________________________________________________________
int
UnpackerManager::get_device_id(const std::string& name) const
{
  return GConfig::get_instance().get_digit_info().get_device_id(name);
}

//_____________________________________________________________________________
int
UnpackerManager::get_plane_id(const char* device_name,
			      const char* plane_name ) const
{
  const std::string device_name_str = device_name;
  const std::string plane_name_str  = plane_name;
  return get_plane_id(device_name_str, plane_name_str);
}

//_____________________________________________________________________________
int
UnpackerManager::get_plane_id(const std::string& device_name,
			      const std::string& plane_name ) const
{
  int device_id = get_device_id(device_name);
  return GConfig::get_instance().get_digit_info().get_plane_id(device_id, plane_name);
}

//_____________________________________________________________________________
int
UnpackerManager::get_data_id(const char* device_name,
			     const char* data_name ) const
{
  const std::string device_name_str = device_name;
  const std::string data_name_str   = data_name;
  return get_data_id(device_name_str, data_name_str);
}

//_____________________________________________________________________________
int
UnpackerManager::get_data_id(const std::string& device_name,
			     const std::string& data_name ) const
{
  int device_id = get_device_id(device_name);
  return GConfig::get_instance().get_digit_info().get_data_id(device_id, 0, 0, 0, data_name);
}

//_____________________________________________________________________________
unsigned int
UnpackerManager::get_entries(unsigned int device_id,
			     unsigned int plane_id,
			     unsigned int segment_id,
			     unsigned int ch,
			     unsigned int data_type) const
{
  const DigitList& digit_list = *m_front;
#ifndef RANGE_CHECK
  return digit_list[device_id][plane_id][segment_id][ch][data_type].size();
#else
  try
    {
      return digit_list.at(device_id).at(plane_id).at(segment_id).at(ch).at(data_type).size();
    }
  catch (const std::out_of_range& e)
    {
      std::ostringstream err;
      try
	{
	  err << " ["  << device_id
	      << " / " << digit_list.size() << "] ";
	  err << " ["  << plane_id
	      << " / " << digit_list.at(device_id).size() << "] ";
	  err << " ["  << segment_id
	      << " / " << digit_list.at(device_id).at(plane_id).size() << "] ";
	  err << " ["  << ch
	      << " / " << digit_list.at(device_id).at(plane_id).at(segment_id).size() << "] ";
	  err << " ["  << data_type
	      << " / " << digit_list.at(device_id).at(plane_id).at(segment_id).at(ch).size() << "] ";
	  throw;
	}
      catch (const std::out_of_range& e1)
	{
	  cerr << "\n#E \n UnpackerManager::get_entries()\n"
	       << " invalid access " << err.str()
	       << " what = " << e.what() << std::endl;
	}
    }
#endif
  return 0U;
}

//_____________________________________________________________________________
unsigned int
UnpackerManager::get_entries(const std::string& name,
			     unsigned int plane_id,
			     unsigned int segment_id,
			     unsigned int ch,
			     unsigned int data_type) const
{
  //  const Device& device = find(name);
//   return device[subdevice_id][ch][data_type].size();

  UnpackerConfig& g_config = GConfig::get_instance();
  const UnpackerConfig::DigitInfo& digit_info = g_config.get_digit_info();

  int device_id = digit_info.get_device_id(name);
  return get_entries(device_id, plane_id, segment_id, ch, data_type);

}

//_____________________________________________________________________________
unsigned int
UnpackerManager::get_n_device() const
{
  return GConfig::get_instance().get_digit_info().get_n_device();
}

//_____________________________________________________________________________
unsigned int
UnpackerManager::get_n_plane(int device_id) const
{
  return GConfig::get_instance().get_digit_info().get_n_plane(device_id);
}

//_____________________________________________________________________________
unsigned int
UnpackerManager::get_n_segment(int device_id, int plane_id) const
{
  return GConfig::get_instance().get_digit_info()
    .get_n_segment(device_id, plane_id);
}

//_____________________________________________________________________________
unsigned int
UnpackerManager::get_n_ch(int device_id, int plane_id, int segment_id) const
{
  return GConfig::get_instance().get_digit_info()
    .get_n_ch(device_id, plane_id, segment_id);
}

//_____________________________________________________________________________
unsigned int
UnpackerManager::get_n_data(int device_id, int plane_id, int segment_id,
                            int ch_id) const
{
  return GConfig::get_instance().get_digit_info()
    .get_n_data(device_id, plane_id, segment_id, ch_id);
}

//_____________________________________________________________________________
uint32_t
UnpackerManager::get_event_number() const
{
  if (m_reader)
    return m_reader->get_daq_root_event_number();
  else
    return 0;
}

//_____________________________________________________________________________
UnpackerManager::fifo_t::size_type
UnpackerManager::get_fifo_entries() const
{
  return m_fifo.get_entries();
}

//_____________________________________________________________________________
UnpackerManager::fifo_t::size_type
UnpackerManager::get_fifo_length() const
{
  return m_fifo.size();
}

//_____________________________________________________________________________
const std::string&
UnpackerManager::get_istream() const
{
  return m_input_stream;
}

//_____________________________________________________________________________
uint64_t
UnpackerManager::get_istream_bookmark() const
{
  if (m_reader && m_reader->is_open())
    return m_reader->tellg();
  else
    return 0;
}

//_____________________________________________________________________________
int
UnpackerManager::get_max_loop() const
{
  return m_max_loop;
}

//_____________________________________________________________________________
unsigned int
UnpackerManager::get_node_header(int node_id, int header_data_type) const
{
  Unpacker* parent = get_root();
  if((int)parent->get_id() == node_id){
    return parent->get_header(header_data_type);
  }

  Unpacker* result = NULL;
  result = search_fe(parent, node_id);

  if(true
     && result
     && result->is_ready()
     && result->is_node()
     ){
    return result->get_header(header_data_type);
  }else{
    // std::cerr << "#E UnpackerManager::get_node_info\n";
    // std::cerr << std::hex
    // 	      << " No such node is found : 0x" << node_id
    // 	      << std::dec << std::endl;
    return 0;
  }
}

//_____________________________________________________________________________
Unpacker*
UnpackerManager::get_root() const
{
  if (m_root)
    return m_root;
  const int n = m_unpacker.size();
  Unpacker* u = 0;
  for (int i=0; i<n; ++i)
    {
      u = m_unpacker[i];
      if (u)
	{
// 	  cout << "#D UnpackerManager::get_root()\n"
// 	       << " non-null unpacker = " << i << std::endl;
	  return u;
	}
    }
  return u;
}

//_____________________________________________________________________________
uint32_t
UnpackerManager::get_run_number() const
{
  if (m_reader)
    return m_reader->get_daq_root_run_number();
  else
    return 0;
}

//_____________________________________________________________________________
int
UnpackerManager::get_skip() const
{
  return m_skip;
}

//_____________________________________________________________________________
void
UnpackerManager::initialize()
{
  static bool s_is_initialized = false;
  cout << "\n#D GUnpacker::initialize()" << std::endl;

  if (m_input_stream.empty())
  {
    cerr << "\n#E GUnpacker::initialize()\n"
         << " input data stream is not specified\n\n exit" << std::endl;
    std::exit(1);
  }

//   if (is_online() && !m_reader->is_open())
  if (!m_reader->is_open())
    reset();

  if (s_is_initialized)
  {
    cerr << "#W UnpackerManager::initialize()\n"
         << " already initialized" << std::endl;
    return;
  }

  const int n_unpacker = m_unpacker.size();

  if (m_fifo.size()>1)
  {
    cout << "#D UnpackerManager::initialize()\n"
         << " initialize fifo " << m_fifo.size() << std::endl;
    for (fifo_t::iterator itr=m_fifo.begin(); itr!=m_fifo.end(); ++itr)
    {
      itr->m_digit = m_digit_list;
      itr->m_fe.resize(n_unpacker);
      for (int i=0; i<n_unpacker; ++i)
      {
        Unpacker* u = m_unpacker[i];
        if (u){
          u->add_channel_map(itr->m_fe[i], itr->m_digit);
        }
      }
    }
    cout << "#D initialize fifo --> done" << std::endl;
  }
  else
  {
    m_front = &m_digit_list;
  }

  if (m_enable_istream_bookmark && m_reader->get_stream_type() == ".dat") {
    std::string base = hddaq::basename(m_input_stream);
    replace_all(base, ".dat", "_bookmark.dat");
    // replace_all(base, ".gz", "");
    std::ostringstream path_oss;
    path_oss << hddaq::dirname(m_input_stream)
             << "/bookmark/" << base;
    m_reader->set_bookmark(path_oss.str());
  }

  int n_skipped = m_reader->skip(m_skip);
  cout << "#D GUnpacker skipped " << n_skipped << " events"
       << std::endl;

  m_reader->read();

  if (m_dump_mode[defines::k_binary])
  {
    m_reader->dump_in_binary();
  }else if (m_dump_mode[defines::k_dec])
  {
    m_reader->dump_in_decimal();
  }else if (m_dump_mode[defines::k_hex])
  {
    m_reader->dump_in_hexadecimal();
  }

  const unsigned int node_id = m_reader->get_root_id();
  if (node_id!=m_unpacker[0]->get_id())
//   if (node_id!=get_root()->get_id())
    {
      cerr << "#Warning UnpackerManager::initialize()\n"
	   << " wrong unpacker ID for root node\n data = "
	   << std::hex << std::showbase
	   << node_id
	   << "\n configuration file = " << m_unpacker[0]->get_id()
	   << std::dec << std::noshowbase << std::endl;
      for (int i=0; i<n_unpacker; ++i)
	{
	  Unpacker* u = m_unpacker[i];
	  if (u && (node_id == u->get_id()))
	    {
	      m_root = u;
	      break;
	    }
	}
      if (!m_root)
	{
	  cerr << "\n#E UnpackerManager::initialize()\n"
	       << " cannot find root node in the configuration file"
	       << std::endl;
	  m_reader->close();
	}
      return;
    }
  else
    {
      m_root = m_unpacker[0];
    }

  clear();
  if (!m_reader->unpack())
    {
      cerr << "\n#E UnpackerManager::initialize()\n"
	     << " failed to split data  .....   close stream"
	     << std::endl;
      m_reader->close();
      return;
    }


  cout << "#D GUnpacker::initialize() ----> finished" << std::endl;

//   std::cout << m_fifo.size()
// 	    << " " << m_fifo.get_entries()
// 	    << " " << m_fifo.free_size()
// 	    << std::endl;

//   m_root->dump_data();
  dump_data();
  check();
  show_summary();
  decode();
  //  ++m_counter;
  //  show_event_number();

  s_is_initialized = true;
  return;
}

//_____________________________________________________________________________
bool
UnpackerManager::is_data_ready()
{
#ifdef DEBUG_FIFO
  static int s_n_front = 0;
  cout << "#D1 UnpackerManager front()" << std::endl;
#endif

  if (m_fifo.size()>1){
    m_front = &(m_fifo.front().m_digit);
  }


#ifdef DEBUG_FIFO
  ++s_n_front;
  cout << "#D2 UnpackerManager front() " << s_n_front << std::endl;
#endif

  return m_is_data_ready;
}

//_____________________________________________________________________________
bool
UnpackerManager::is_decode_mode() const
{
  return m_decode_mode;
}

//_____________________________________________________________________________
bool
UnpackerManager::is_good() const
{
  return m_is_good;
}

//_____________________________________________________________________________
bool
UnpackerManager::is_online() const
{
  if (std::string::npos!=m_input_stream.find(":")){
    return true;
  }else{
    return false;
  }
}

//_____________________________________________________________________________
void
UnpackerManager::operator++()
{
  clear();
  read();
  if (eof())
    {
      cout << "#D GUnpacker::operator++()   exit loop" << std::endl;
      return;
    }
  if (!m_reader->unpack())
    {
      cerr << "\n#E UnpackerManager::operator++()\n"
	   << " failed to split data  .....   close stream"
	   << std::endl;
      m_reader->close();
      return;
    }
  dump_data();

  check();
  show_summary();
  decode();

  ++m_counter;

  if (m_print_cycle>0 && (m_counter%m_print_cycle==0)){
    show_event_number();
  }

  return;
}

//_____________________________________________________________________________
void
UnpackerManager::pop_event()
{
#ifdef DEBUG_FIFO
  static int s_n_pop   = 0;
  cout << "#D1 UnpackerManager::pop_event()" << std::endl;
#endif

  if (m_fifo.size()>1)
    m_fifo.pop();

#ifdef DEBUG_FIFO
  ++s_n_pop;
  cout << "#D2 UnpackerManager::pop_event() "  << s_n_pop << std::endl;
#endif
  return;

}

//_____________________________________________________________________________
void
UnpackerManager::print() const
{
  cout << "#D GUnpacker::print()" << std::endl;
  m_root->print();

  return;
}

//_____________________________________________________________________________
void
UnpackerManager::print_tag() const
{
  cout << "#D GUnpacker::print_tag()\n"
       << "============================================================="
       << std::endl;

  m_root->print_tag();

  cout << std::endl;
  return;
}

//_____________________________________________________________________________
void
UnpackerManager::push_event()
{
#ifdef DEBUG_FIFO
  static int s_n_push = 0;
  cout << "#D1 UnpackerManager::push_event()" << std::endl;
#endif

  if (m_fifo.size()>1)
    {
      m_fifo.push();
      m_is_data_ready = !empty();
    }
  else
    m_is_data_ready = true;


#ifdef DEBUG_FIFO
  ++s_n_push;
  cout << "#D2 UnpackerManager::push_event() " << s_n_push << std::endl;
#endif
  return;
}

//_____________________________________________________________________________
void
UnpackerManager::read()
{
  m_reader->read();
//   cout << "#D " << m_counter << std::endl;;
//    m_reader->dump_in_hexadecimal();

  return;
}

//_____________________________________________________________________________
void
UnpackerManager::reset()
{
  try
    {
      tag_summary << esc::k_default_color << std::endl;
      Unpacker* root = get_root();
      root->reset();

      if (!m_reader->is_open())
	m_reader->open(m_input_stream);
    }
  catch (const std::exception& e)
    {
      cerr << "\n#E UnpackerManager::reset()\n"
	   << " what = " << e.what() << std::endl;
    }
  m_counter = 0;
  return;
}

//_____________________________________________________________________________
void
UnpackerManager::set_config_file(const std::string& config_file,
				 const std::string& digit_file,
				 const std::string& channel_map_file)
{
//   cout << "#D UnpackerManager::set_config_file()\n" << std::endl;
  UnpackerConfig& g_config = GConfig::get_instance();
  try
    {
      std::string config_file_resolved      = realpath(config_file);
      std::string digit_file_resolved       = realpath(digit_file);
      std::string channel_map_file_resolved = realpath(channel_map_file);

      g_config.initialize(m_digit_list,
			  config_file_resolved,
			  digit_file_resolved,
			  channel_map_file_resolved);

      set_parameter("first",       g_config.get_control_param("first"));
      set_parameter("skip",        g_config.get_control_param("skip"));
      set_parameter("max_loop",    g_config.get_control_param("max_loop"));
      set_parameter("last",        g_config.get_control_param("last"));
      set_parameter("esc",         g_config.get_control_param("esc"));
      set_parameter("print_cycle", g_config.get_control_param("print_cycle"));
      set_parameter("fifo_length", g_config.get_control_param("fifo_length"));

      Unpacker::set_esc(m_is_esc_on);
      Unpacker::set_check_mode(std::bitset<defines::k_n_check_mode>
			       (g_config.get_control_param("error_check")),
			       g_config.get_control_param("char_true"),
			       g_config.get_control_param("char_false"));
    }
  catch (const std::exception& e)
    {
      cerr << "\n#E UnpackerManager::set_config_file()\n"
	   << " what = " << e.what()
	   << std::endl;
      throw e;
    }
  m_unpacker[0]->sort_child();
//   print();
  return;
}

//_____________________________________________________________________________
void
UnpackerManager::set_decode_mode(bool flag)
{
  m_decode_mode = flag;
  return;
}

//_____________________________________________________________________________
void
UnpackerManager::set_dump_mode(unsigned int dump_mode)
{
  m_dump_mode.set(dump_mode);
  return;
}

//_____________________________________________________________________________
void
UnpackerManager::set_istream(const std::string& name)
{
  m_input_stream = name;
  if (!is_online())
    m_input_stream = realpath(name);

  cout << "\n"
       << "#D ___________ GUnpacker::set_istream() __________\n"
       << "   input stream = " << m_input_stream
       << std::endl;
  return;
}

//_____________________________________________________________________________
void
UnpackerManager::set_istream_bookmark(uint64_t bookmark)
{
  return;
}

//_____________________________________________________________________________
void
UnpackerManager::set_parameter(const std::string& name,
			       const std::string& value)
{
  if (value==name || value.empty()) return;

  cout << "#D UnpackerManager::set_parameter()\n"
       << " key = " << name << ", value = " << value << std::endl;
  if (name=="run"){
    cout << "Run No. = " << value << std::endl;
  }else if (name=="skip" || name=="first"){
    m_skip = a2i(value);
    cout << " " << name << " = " << m_skip << std::endl;
  }else if (name=="max_loop" || name=="last"){
    int last = 0;
    if (name=="max_loop") {
      m_max_loop = a2i(value);
      last       += last + m_skip;
    }
    else if (name=="last") {
      last       = a2i(value);
      m_max_loop = last - m_skip;
    }

    if (0>=m_max_loop){
      cout << "#D GUnpacker::initialize()\n"
	   << "  got max loop == " << m_max_loop
	   << "  --> unpack all events"
	   << std::endl;

    }else{
      last += m_max_loop;
    }

    cout << " max_loop = " << m_max_loop
	 << std::endl;
    if (0<m_max_loop){
      cout << " (last = " << last << ")" << std::endl;
    }

  }else if (name=="esc"){
    m_is_esc_on = (value.find("on")!=std::string::npos);
  }else if (name=="print_cycle"){
    m_print_cycle = a2i(value);
  }else if (name=="fifo_length"){
    m_fifo.resize(a2i(value));
    m_fifo.initialize();
  }


  return;
}

//_____________________________________________________________________________
void
UnpackerManager::show_event_number() const
{
  uint32_t ev_data = m_reader->get_daq_root_event_number();
  std::stringstream ev_ss;
  ev_ss << ev_data << " (" << std::hex << std::showbase << ev_data << ")";
  cout << "#D UnpackerManager::show_event_number()\n"
       << "   (daq root data) "
       << ((ev_data==defines::k_unassignedU) ? "----------" : ev_ss.str())
       << "\n"
       << "   (counter)       " << m_counter
       << " (" << std::hex << std::showbase << m_counter
       << std::dec << std::noshowbase << ")"
       << std::endl;

  return;
}

//_____________________________________________________________________________
void
UnpackerManager::show_summary(bool is_forced)
{
//   Unpacker* root = m_unpacker[0];
  Unpacker* root = get_root();
  bool is_consistent=true;
  //  bool is_consistent=false;
  bool is_slipped=false;
  m_is_good = root->is_good();
  is_consistent
    = root->is_bad(defines::k_local_tag_bit)
    | root->is_bad(defines::k_event_tag_bit)
    | root->is_bad(defines::k_spill_tag_bit);
  is_slipped
    = root->is_bad(defines::k_local_slip_bit)
    | root->is_bad(defines::k_event_slip_bit)
    | root->is_bad(defines::k_spill_slip_bit);

  const bool is_slip_allowed
    = !(root->get_check_mode()[defines::k_slip2]);
  is_forced
    |= root->get_check_mode()[defines::k_show_always];
  if (is_forced
      ||
      (!(m_is_good || (is_slip_allowed & is_slipped & is_consistent))))
    {
      tag_summary << "\n";
      tag_summary << "======================================"
		  << "======================================"
		  << std::endl;
      tag_summary.setf(std::ios_base::left);
      tag_summary << std::setfill(' ')
		  << std::setw(24) << "nick-name";
      tag_summary.unsetf(std::ios_base::left);
      tag_summary.setf(std::ios_base::right);
      tag_summary << std::setw(defines::k_n_error_state + 3)
		  << "error-state"
		  << std::setw(7)  << "size"
		  << " "
		  << std::setw(12) << "local"
		  << " "
		  << std::setw(12) << "event(3bit)"
		  << " "
		  << std::setw(12) << "spill(2bit)"
#if 0
		  << " "
		  << std::setw(12) << "l-prev"
		  << std::setw(12) << "e-prev"
		  << std::setw(12) << "s-prev"
#endif
	// 	 << std::endl;
	   << "\n";
      tag_summary << std::setfill(' ')
		  << std::setw(24 + defines::k_n_error_state + 3)
		  << "bUuSELselthdg"
		  << "\n";
      tag_summary.unsetf(std::ios_base::right);

      root->show_summary();
      tag_summary << std::endl;


      if (m_is_esc_on)
	{
	  tag_summary << esc::k_beep << std::endl;
	  tag_summary << esc::k_default_color;
	}
    }
  tag_summary.flush();
  return;
}

//_____________________________________________________________________________
void
UnpackerManager::search_device(Unpacker* u,
			       std::vector<Unpacker*>& uvect,
			       int device_id, int plane_id
			       ) const
{
  const UnpackerImpl::ChildList& clist = u->get_child_list();
  if(0 == clist.size()){
    if(u->is_related(device_id, plane_id)){uvect.push_back(u);}
    return;
  }else{
    UnpackerImpl::ChildList::const_iterator itr = clist.begin();
    UnpackerImpl::ChildList::const_iterator end = clist.end();
    for(; itr != end; ++itr){
      if(itr->second){
	search_device(itr->second, uvect, device_id, plane_id);
      }
    }// for(itr)
  }// end if

  return;
}

//_____________________________________________________________________________
Unpacker*
UnpackerManager::search_fe(Unpacker* parent, int id) const
{
  Unpacker* ret = NULL;

  const UnpackerImpl::ChildList& clist = parent->get_child_list();
  if(0 == clist.size()){
    return ret;
  }else{
    UnpackerImpl::ChildList::const_iterator itr = clist.begin();
    UnpackerImpl::ChildList::const_iterator end = clist.end();
    for(; itr != end; ++itr){
      if(itr->second){
	ret = itr->second;
	if(id == (int)ret->get_id()){ return ret; }
      }
    }// for(itr)
  }// end if

  return NULL;
}
  }
}
