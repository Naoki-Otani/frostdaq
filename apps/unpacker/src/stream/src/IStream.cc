// -*- C++ -*-

// Author: Tomonori Takahashi

#include <iostream>
#include <fstream>
#include <set>

#include "IStream.hh"
#include "IStreamFactory.hh"
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
  static std::set<std::string> known_type;
  if (known_type.empty())
    {
      known_type.insert(k_stream_type_dat_file);
      known_type.insert(k_stream_type_bzip2_file);
      known_type.insert(k_stream_type_gzip_file);
//       known_type.insert(k_stream_type_std_cin);
    }
//   std::cout << "#D resolve_stream_type() " << stream_name;
// 	    << " " << stream_type << std::endl;

  if ((stream_name==k_stream_type_std_cin) || stream_name.empty())
    stream_type = k_stream_type_std_cin;
  else if (std::string::npos!=stream_name.find_last_of(":"))
    stream_type = k_stream_type_socket;
  else
    {
      const std::string::size_type suffix_position
	= stream_name.find_last_of(".");
      try
	{
	  const std::string suffix
	    = stream_name.substr(suffix_position,
				 (stream_name.length() - suffix_position));
	  std::set<std::string>::iterator i = known_type.find(suffix);
	  if (i!=known_type.end())
	    stream_type = *i;
	  else
	    {
	      std::cerr << "#E unknown data source type"
			<< stream_name
			<< std::endl;
	    }
	}
      catch (const std::exception& e)
	{
	  std::cout << "#D resolve_stream_type()\n"
		    << " what = " << e.what()
		    << " stream = " << stream_name
		    << ", type = " << stream_type
		    << std::endl;
	}
    }
//   std::cout << " -> " << stream_type << std::endl;

  return;
}
    }


//______________________________________________________________________________
IStream::IStream()
  : m_stream(0),
    m_stream_name(),
    m_stream_type()
{
}

//______________________________________________________________________________
IStream::IStream(const std::string& stream_name,
		 std::ios_base::openmode mode)
  : m_stream(0),
    m_stream_name(),
    m_stream_type(),
    m_is_bad(false),
    m_is_eof(false),
    m_is_fail(false),
    m_is_good(true),
    m_gcount(0)
{
  open(stream_name, mode);
}

//______________________________________________________________________________
IStream::~IStream()
{
  if (this->is_open())
    close();
//   std::cout << "#D IStream::~IStream() " << m_stream_name << std::endl;
}

//______________________________________________________________________________
bool
IStream::bad() const
{
  return m_stream->bad();
}

//______________________________________________________________________________
void
IStream::close()
{
  m_gcount  = m_stream->gcount();
  m_is_bad  = m_stream->bad();
  m_is_eof  = m_stream->eof();
  m_is_fail = m_stream->fail();
  m_is_good = m_stream->good();

  if (m_stream)
    {
      delete m_stream;
      m_stream = 0;
    }
  std::cout << "#D IStream " << m_stream_name << " closed" << std::endl;
  return;
}

//______________________________________________________________________________
bool
IStream::eof() const
{
  return (!m_stream) ? m_is_eof :  (m_stream->eof());
}

//______________________________________________________________________________
bool
IStream::fail() const
{
  return (!m_stream) ? m_is_fail : (m_stream->fail());
}

//______________________________________________________________________________
std::streamsize
IStream::gcount() const
{
  return (!m_stream) ? m_gcount : (m_stream->gcount());
}

//______________________________________________________________________________
const std::string&
IStream::get_stream_type() const
{
  return m_stream_type;
}

//______________________________________________________________________________
bool
IStream::good() const
{
  return (!m_stream) ? m_is_good : (m_stream->good());
}

//______________________________________________________________________________
IStream::istream_type&
IStream::ignore()
{
  return m_stream->ignore();
}

//______________________________________________________________________________
IStream::istream_type&
IStream::ignore(std::streamsize n)
{
  if (m_stream_type == ".dat")
    return m_stream->seekg(n, std::ios::cur);
  else
    return m_stream->ignore(n);
}

//______________________________________________________________________________
IStream::istream_type&
IStream::ignore(std::streamsize n, int_type delim)
{
  return m_stream->ignore(n, delim);
}

//______________________________________________________________________________
bool
IStream::is_open() const
{
  return (0 != m_stream);
}

//______________________________________________________________________________
void
IStream::open(const std::string& stream_name,
	      std::ios_base::openmode mode)
{
  m_gcount      = 0;
  m_is_bad      = false;
  m_is_eof      = false;
  m_is_fail     = false;
  m_is_good     = true;
  m_stream_name = stream_name;

  resolve_stream_type(stream_name, m_stream_type);
  IStreamFactory& g_factory = GIStreamFactory::get_instance();
  m_stream = g_factory.create(m_stream_type, stream_name, mode);
  return;
}

//______________________________________________________________________________
IStream::istream_type&
IStream::read(char_type* s,
	      std::streamsize n)
{
//   if (m_stream_type!=k_stream_type_socket)
    return m_stream->read(s, n);
//   else
//     {
//       int nleft = n;
// //       try
// // 	{
//       while (nleft>0)
// 	{
// 	  m_stream->read(s, n);
// 	  int nread = m_stream->gcount();
// 	  std::cout << "#D IStream::read() gcount = " << nread << std::endl;
// 	  if (nread<0)
// 	    {
// 	      m_stream->setstate(std::ios::badbit);
// 	      return *m_stream;
// 	    }
// 	  else if (nread==0)
// 	    {
// 	      m_stream->setstate(std::ios::eofbit | std::ios::failbit);
// 	      return *m_stream;
// 	    }

// 	  nleft -= nread;
// 	  s     += nread;
// 	}

// // 	}
// //       catch (const std::exception& e)
// // 	{
// // 	}
//     }
//   return *m_stream;
}

//______________________________________________________________________________
int
IStream::sync()
{
  return (!m_stream) ? 0 : (m_stream->sync());
}

//______________________________________________________________________________
IStream::pos_type
IStream::tellg()
{
  if (m_stream_type == ".gz") {
    std::cerr << "#E tellg() is not supported in gz stream type" << std::endl;
    return IStream::pos_type(0);
  } else {
    return (!m_stream) ? IStream::pos_type(0) : (m_stream->tellg());
  }
}

  }
}
