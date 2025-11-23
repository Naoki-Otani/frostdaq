// -*- C++ -*-

// Author: Tomonori Takahashi

#include "EventReader.hh"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <unistd.h>

#include "filesystem_util.hh"
#include "std_ostream.hh"
#include "BitDump.hh"
#include "HexDump.hh"
#include "IStream.hh"
#include "Unpacker.hh"
#include "UnpackerManager.hh"
#include "defines.hh"
#include "Clear.hh"

namespace hddaq
{
  namespace unpacker
  {

  namespace
  {
    const unsigned int k_word_size = sizeof(uint32_t);
  }

//______________________________________________________________________________
EventReader::EventReader()
  : m_stream_name(),
    m_buffer(DAQNode::k_header_size),
    m_begin(),
    m_end(),
    m_stream(0),
    m_header(0),
    m_bookmark(0)
{
  m_begin = m_buffer.end();
  m_end   = m_buffer.end();
}

//______________________________________________________________________________
EventReader::~EventReader()
{
//   cout << "#D EventReader:~EventReader()" << std::endl;
  clear();
//   cout << "#D EventReader:~EventReader()" << std::endl;
}

//______________________________________________________________________________
void
EventReader::hoge(const std::string& arg) const
{
  cout << "#D EventReader::hoge()  " << arg << std::endl;
  return;
}

//______________________________________________________________________________
void
EventReader::clear()
{
#ifndef BUFFER_REUSE
  m_buffer.clear();
#endif
  m_header = 0;
  m_begin = m_buffer.end();
  m_end   = m_buffer.end();
  return;
}

//______________________________________________________________________________
void
EventReader::close()
{
  if (m_stream)
    {
      m_stream->close();
      delete m_stream;
      m_stream = 0;
    }
  clear();
  return;
}

//______________________________________________________________________________
void
EventReader::dump_header() const
{
  if (!m_header)
    {
      cerr << "#D EventReader::dump_header() got null pointer"
	     << std::endl;
      return;
    }
  cout << "#D EventReader::dump_header()" << std::endl;
  std::for_each(reinterpret_cast<unsigned int*>(m_header),
		reinterpret_cast<unsigned int*>(m_header)
		+DAQNode::k_header_size,
		HexDump());
  return;
}

//______________________________________________________________________________
void
EventReader::dump_in_binary() const
{
  cout << "#D EventReader::dump_in_binary()" << std::endl;
  std::for_each(m_buffer.begin(), m_buffer.end(), BitDump());
  return;
}

//______________________________________________________________________________
void
EventReader::dump_in_decimal() const
{
  cout << "#D EventReader::dump_in_decimal()" << std::endl;
  cout << " ... not implemented" << std::endl;
  //  std::for_each(m_buffer.begin(), m_buffer.end(), BitDump());
  return;
}

//______________________________________________________________________________
void
EventReader::dump_in_hexadecimal() const
{
  cout << "#D EventReader::dump_in_hexadecimal()" << std::endl;
  std::for_each(m_buffer.begin(), m_buffer.end(), HexDump());
  return;
}

//______________________________________________________________________________
bool
EventReader::eof() const
{
  if (m_stream){
    return m_stream->eof();
  }else{
    return false;
  }
}

//______________________________________________________________________________
unsigned int
EventReader::get_daq_root_event_number() const
{
//   cout << "#D EventReader::get_daq_root_event_number()\n   "
// 	    << m_header->m_event_number
// 	    << std::hex << "(" << m_header->m_event_number << ")"
// 	    << std::dec << std::endl;
//   dump_header();
  if (m_buffer.empty() || m_begin == m_buffer.end()){
    return defines::k_unassignedU;
  }else if (m_header){
    return m_header->m_event_number;
  }else{
    return 0;
  }
}

//______________________________________________________________________________
unsigned int
EventReader::get_daq_root_run_number() const
{
//   cout << "#D EventReader::get_daq_root_run_number()\n   "
// 	    << m_header->m_run_number
// 	    << std::hex << "(" << m_header->m_run_number << ")"
// 	    << std::dec << std::endl;
//   dump_header();
  if (m_buffer.empty() || m_begin == m_buffer.end()){
    return defines::k_unassignedU;
  }else if (m_header){
    return m_header->m_run_number;
  }else{
    return 0;
  }
}

//______________________________________________________________________________
unsigned int
EventReader::get_root_id() const
{
  if (m_header) return m_header->m_node_id;

  cerr << "#E EventReader::get_root_id()\n "
       << " got null header " << std::endl;
  return defines::k_unassignedU;
}

//______________________________________________________________________________
const std::string&
EventReader::get_stream_type() const
{
  static std::string ret;
  if (m_stream) ret = m_stream->get_stream_type();
  return ret;
}

//______________________________________________________________________________
bool
EventReader::is_open() const
{
  if (m_stream)
//     return m_stream->is_open();
    return true;
  else
    return false;
}

//______________________________________________________________________________
void
EventReader::open(const std::string& stream_name)
{
  if (!m_stream) {
    m_stream = new IStream(stream_name);
    m_stream_name = stream_name;
  }else{
    cerr << "\n#W stream is already open : " << m_stream_name << std::endl;
  }

  if (m_stream->fail())
    {
      cerr << "\n#E ERROR : CANNOT OPEN "
	     << stream_name << std::endl;
//       if (m_steam)
// 	{
// 	  delete m_stream;
// 	  m_stream = 0;
// 	}
      const std::string& stream_type = m_stream->get_stream_type();
      close();
      if (k_stream_type_socket==stream_type)
	{
	  ::sleep(3);
	  cerr << "\n#E retry" << std::endl;
	  open(stream_name);
	}
      else
	  std::exit(1);
    }
  cout << "#D EventReader::open()  data stream " << stream_name << std::endl;
  return;
}

//______________________________________________________________________________
bool
EventReader::unpack()
{
  Unpacker& root = *(GUnpacker::get_instance().get_root());
  if (!m_header)
    {
      cerr << "#E EventReader::unpack() no header" << std::endl;
//   cout << "#D EventReader::unpack() size = " << m_header->m_data_size
//        << " ( buf = " << m_buffer.size() << ")"
//        << std::endl;
      return false;
    }
  root.set_data_size(m_header->m_data_size);
  if (m_begin==m_buffer.end()){
    cerr << "#E EventReader::unpack() no data" << std::endl;
  }

  root.set_data(m_begin, m_end);
//   hoge(root.get_type());
//   dump_header();
//   dump_in_hexadecimal();
  return root.unpack();
}

//______________________________________________________________________________
bool
EventReader::read(bool skip_flag)
{
//   cout << "#D0 ER::read() : " << DAQNode::k_header_size << std::endl;
  if (!is_open()) return true;
//   cout << "#D1 ER::read() : " << m_buffer.size() << std::endl;

  clear();

  if (DAQNode::k_header_size>m_buffer.size()){
    m_buffer.resize(DAQNode::k_header_size);
  }

//   cout << "#D2 ER::read() : " << m_header << std::endl;

  if (m_stream->read(reinterpret_cast<char*>(&(m_buffer[0])),
		     (DAQNode::k_header_size * k_word_size)))
    {
      if (is_open() && !m_stream->good())
	{
	  cerr << "\n#E1 stream is not good " << std::endl;
	  cout << "#D1 EventReader::read()\n"
		 << " close stream" << std::endl;
	  close();
	  return true;
	}
    }
  m_header = reinterpret_cast<DAQNode::Header*>(&m_buffer[0]);
//   dump_header();
  if (!m_header)
    {
      cerr << "\n#E1 null header" << std::endl;
      cout << "#D1 EventReader::read()\n"
	   << " close stream" << std::endl;
      close();
      return true;
    }

  const std::streamsize nwords = m_header->m_data_size;
  if (nwords==0)
    {
      cerr << "\n#E EventReader::read() no data" << std::endl;
      //       std::exit(1);
      //       cout << "#D show event number";
      //       show_event_number();
      //       cout << eof() << std::endl;
      cout << "#D EventReader::read()\n"
	     << " close stream" << std::endl;
      close();
    }

//   cout << "#D3 ER::read() : " << m_header->m_data_size << std::endl;

  // vector::resize() might cause m_header invalid
  if (nwords > (std::streamsize)m_buffer.size())
    m_buffer.resize(nwords);
//   cout << "#D4 ER::read() : " << m_buffer.size() << std::endl;
//   dump_header();

  const std::streamsize nbytes_to_read
    = (nwords - DAQNode::k_header_size) * k_word_size;
//   cout << "#D5 ER::read() : " << nbytes_to_read << std::endl;
  if (is_open() && !m_stream->good())
    {
      cerr << "\n#E2 stream is not good " << std::endl;
      cout << "#D2 EventReader::read()\n"
	     << " close stream" << std::endl;
      close();
      return true;
    }

  if (!m_stream)
    {
      cerr << "\n#E null stream" << std::endl;
      return true;
    }

  if (skip_flag) {
    m_stream->ignore(nbytes_to_read);
    return true;
  }

  m_stream->read(reinterpret_cast<char*>(&(m_buffer[DAQNode::k_header_size])),
		 nbytes_to_read);
  // the pointer to the first address of m_buffer must be changed
  m_header = reinterpret_cast<DAQNode::Header*>(&m_buffer[0]);
  if (!m_header)
    {
      cerr << "\n#E2 null header " << std::endl;
      cout << "#D1 EventReader::read()\n"
	   << " close stream" << std::endl;
      close();
      return true;
    }

  m_begin = m_buffer.begin();
  m_end   = m_begin + m_header->m_data_size;

//   cout << "#D6 ER::read()" << std::endl;
  if (is_open() && m_stream->eof())
    {
      cerr << "\n#E3 stream reached end " << std::endl;
      cout << "#D2 EventReader::read()\n"
	   << " close stream" << std::endl;
      close();
      //   cout << "#D is open ? = " << is_open() << " "
      // 	    << " eof = " << eof() << std::endl;
      return true;
    }

  //   cout << "#D end of read() : gcount =  " << m_stream->gcount()
  // 	       << " bytes" << std::endl;

  return true;
}

//______________________________________________________________________________
void
EventReader::set_bookmark(const std::string& bookmark_name)
{
  if(m_bookmark){
    cout << "#W bookmark is already open " << bookmark_name << std::endl;
    return;
  }
  if(!std::ifstream(bookmark_name).is_open()){
    std::ostringstream msg;
    msg << "\n#E failed to open stream bookmark file: "
        << bookmark_name << std::endl;
    throw FilesystemException(msg.str());
    // return;
  }
  m_bookmark = new IStream(bookmark_name);
  if(m_bookmark->eof() || !m_bookmark->is_open()){
    std::ostringstream msg;
    msg << "\n#E failed to open stream bookmark file: "
        << bookmark_name << std::endl;
    // cerr << msg.str() << std::endl;
    throw FilesystemException(msg.str());
  }else{
    cout << "#D EventReader::set_bookmark() bookmark "
         << bookmark_name << std::endl;
  }
  return;
}

//______________________________________________________________________________
int
EventReader::skip(int n_skip)
{
  if (n_skip <= 0)
    return 0;
  if (m_bookmark && (get_stream_type() == ".dat"
                     // || get_stream_type() == ".gz"
        )){
    if (m_bookmark->eof() || !m_bookmark->is_open()) {
      std::ostringstream msg;
      msg << "\n#E failed to open stream bookmark file" << std::endl;
      throw FilesystemException(msg.str());
    } else {
      m_bookmark->ignore(n_skip*sizeof(uint64_t));
      uint64_t bookmark;
      m_bookmark->read(reinterpret_cast<char*>(&bookmark), sizeof(uint64_t));
      if (m_bookmark->eof() || !m_bookmark->is_open()) {
        std::ostringstream msg;
        msg << "\n#E too much event skipped : " << n_skip << std::endl;
        throw FilesystemException(msg.str());
      }
      cout << "#D skipping to " << bookmark << " ..." << std::endl;
      m_stream->ignore(bookmark);
      return n_skip;
    }
  } else {
    for (int i=0; i<n_skip; ++i) {
      if (eof() || !is_open()) {
        cerr << "\n#E too much event skipped" << std::endl;
        return i-1;
      }
      read(true);
    }
    return n_skip;
  }
}

//______________________________________________________________________________
uint64_t
EventReader::tellg()
{
  if(m_stream && m_stream->is_open())
    return m_stream->tellg();
  else
    return 0;
}

  }
}
