// -*- C++ -*-

// Author: Tomonori Takahashi

#include <iostream>
#include <fstream>
#include <set>

#include <cstdlib>

#include "OStream.hh"
#include "OStreamFactory.hh"
#include "IGZFileStream.hh"
#include "IBZFileStream.hh"
#include "ISocketStream.hh"

namespace hddaq
{
  namespace unpacker
  {

    namespace
    {
//______________________________________________________________________________
void
resolve_stream_type(const std::string& stream_name,
		    std::string& stream_type)
{
  std::set<std::string> known_type;
  known_type.insert(".dat");
  known_type.insert(".bz2");
  known_type.insert(".gz");


  const std::string::size_type delim_position
    = stream_name.find_last_of(":");
  if (std::string::npos!=delim_position) 
    {
      stream_type = "socket";
    } 
  else {
    const std::string::size_type suffix_position
      = stream_name.find_last_of(".");
    const std::string suffix
      = stream_name.substr(suffix_position, 
			   (stream_name.length() - suffix_position));
    
    std::set<std::string>::iterator i = known_type.find(suffix);
    if (i!=known_type.end())
      stream_type = *i;
    else
      {
	std::cerr << "#E unknown data surce type" 
		  << stream_name
		  << std::endl;
      }
  } 

//   std::cout << "#D stream type = " << stream_type << std::endl;
  
  return;
}
    }
  
//______________________________________________________________________________
OStream::OStream()
  : m_stream(0),
    m_stream_name(),
    m_stream_type()
{
}

//______________________________________________________________________________
OStream::OStream(const std::string& stream_name,
		 std::ios_base::openmode mode)
  : m_stream(0),
    m_stream_name(),
    m_stream_type(),
    m_is_bad(false),
    m_is_eof(false),
    m_is_fail(false),
    m_is_good(true)
{
  open(stream_name, mode);
}

//______________________________________________________________________________
OStream::~OStream()
{
  if (this->is_open())
    close();
}

//______________________________________________________________________________
bool
OStream::bad() const
{
  return m_stream->bad();
}

//______________________________________________________________________________
void
OStream::close()
{
  m_is_bad  = m_stream->bad();
  m_is_eof  = m_stream->eof();
  m_is_fail = m_stream->fail();
  m_is_good = m_stream->good();

  if (m_stream)
    {
      delete m_stream;
      m_stream = 0;
    }
  std::cout << "#D OStream " << m_stream_name << " closed" << std::endl;
  return;
}

//______________________________________________________________________________
bool
OStream::eof() const
{
  return (!m_stream) ? m_is_eof :  (m_stream->eof());
}

//______________________________________________________________________________
bool
OStream::fail() const
{
  return (!m_stream) ? m_is_fail : (m_stream->fail());
}

//______________________________________________________________________________
OStream::ostream_type&
OStream::flush()
{
  if (!m_stream)
    {
      std::cerr << "#E stream does not exit" << std::endl; 
      std::exit(0);
    }

  return m_stream->flush();
}

//______________________________________________________________________________
bool
OStream::good() const
{
  return (!m_stream) ? m_is_good : (m_stream->good());
}

//______________________________________________________________________________
bool
OStream::is_open() const
{
  return (0 != m_stream);
}

//______________________________________________________________________________
void
OStream::open(const std::string& stream_name,
	      std::ios_base::openmode mode)
{
  m_is_bad      = false;
  m_is_eof      = false;
  m_is_fail     = false;
  m_is_good     = true;
  m_stream_name = stream_name;

  resolve_stream_type(stream_name, m_stream_type);
  m_stream = GOStreamFactory::get_instance().create(m_stream_type,
						    stream_name,
						    mode);  
  return;
}

//______________________________________________________________________________
OStream::ostream_type&
OStream::write(char_type* s,
	       std::streamsize n)
{
  return m_stream->write(s, n);
}

  }
}
