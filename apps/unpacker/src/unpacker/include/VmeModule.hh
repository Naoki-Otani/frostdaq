// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__VME_MODULE_UNPACKER_H
#define HDDAQ__VME_MODULE_UNPACKER_H

#include "UnpackerImpl.hh"

namespace hddaq
{
  namespace unpacker
  {

    class VmeModule
      : public UnpackerImpl,
	private Uncopyable<VmeModule>
    {
    public:
      static const unsigned long long k_VME_MODULE_HEADER_MAGIC
      = 0x766d65626f617264ULL; // 'vmeboard'
      struct Header
      {
	unsigned long long m_magic;
	unsigned long long m_vme_address;
	unsigned long long m_data_size;
	unsigned long long m_n_times_read_device;
	unsigned long long m_module_type[2];
	unsigned long long m_tv_sec;
	unsigned long long m_tv_nsec;
      };

      static const unsigned int k_header_size = sizeof(Header)/sizeof(uint32_t);
    

    protected:
      Header*  m_vme_header;
      iterator m_module_data_first;

    public:
      VmeModule(const unpacker_type& type);
      virtual ~VmeModule();

    protected:
      virtual void check_data_format();
      virtual void print_vme_header() const;
      virtual bool unpack();

    };

  }
}
#endif
