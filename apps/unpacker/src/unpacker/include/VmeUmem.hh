// -*- C++ -*-

// Author: Koji Miwa for E13

#ifndef HDDAQ__VME_UMEM_UNPACKER_H
#define HDDAQ__VME_UMEM_UNPACKER_H

#include <vector>
#include <map>

#include "VmeModule.hh"

namespace hddaq
{
  namespace unpacker
  {

    class Unpacker;

    class VmeUmem
      : public VmeModule,
	private Uncopyable<VmeUmem>
    {

    public:
      static const unpacker_type k_type;

      static const uint32_t k_bitshift_wordtype = 15U;
      static const uint32_t k_bitshift_subaddr  = 13U;

      static const uint32_t k_VSN_MASK          = 0xffU;
      static const uint32_t k_SUBADDR_MASK      = 0x3U;
      static const uint32_t k_DATA_MASK         = 0x1fffU;

      static const int k_n_fera = 24;

    protected:
      std::vector<Unpacker*> m_fera;
      iterator               m_fera_begin;
      iterator               m_fera_end;

    public:
      VmeUmem(const unpacker_type& type);
      virtual ~VmeUmem();
      
    protected:
      virtual void decode();
      virtual void resize_fe_data();
      
      virtual bool set_fera_data(Unpacker* u,
				 unsigned int ma,
				 const iterator& data_first,
				 const iterator& data_last);
      virtual void sort_child();
      virtual bool unpack();

    };
    
  }
}
#endif
