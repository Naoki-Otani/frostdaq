// -*- C++ -*-

#include <stdio.h>
#include "VmeV1290A.hh"

namespace hddaq
{
  namespace unpacker
  {

    const defines::unpacker_type VmeV1290A::k_type = "VmeV1290A";

    //______________________________________________________________________
    VmeV1290A::VmeV1290A(const unpacker_type& type)
      : VmeModule(type),
	module_id(0)
    {

    }

    //______________________________________________________________________
    VmeV1290A::~VmeV1290A()
    {

    }

    //______________________________________________________________________
    void
    VmeV1290A::decode()
    {
      int module = 0; int ch = 0; int tdc = 0; int adc = 0;
      for(const_iterator i = m_module_data_first; i!=m_data_last; ++i)
	{
	  unsigned int data = *i;
	  if(data == k_data_module1) module = 1;
	  if(data == k_data_module2) module = 2;
	  if(module==0)
	    {
	      if(data!=0 && (data>>27)==0)
		{
		  ch  = (data>>21) & 0x1f;
		  tdc = data & 0x1fffff;
		  fill(ch, module, tdc);
		  // std::cout<<"VmeV1290A No."<<module
		  // 	   <<" : ch = "<<ch
		  // 	   <<" tdc = "<<tdc
		  // 	   <<std::endl;
		}
	    }
	  if(module==1)
	    {
	      if(data!=1 && (data>>27)==0)
		{
		  ch  = (data>>21) & 0x1f;
		  tdc = data & 0x1fffff;
		  fill(ch, module, tdc);
		  // std::cout<<"VmeV1290A No."<<module
		  // 	   <<" : ch = "<<ch
		  // 	   <<" tdc = "<<tdc
		  // 	   <<std::endl;
		}
	    }
	  if(module==2)
	    {
	      if(((data>>24) & 0xff) == 0xf8)
		{
		  ch = (data>>17) & 0xf;
		  adc = data & 0xfff;
		  fill(ch, module, adc);
		  // std::cout<<"VmeV792N"
		  // 	   <<" : ch = "<<ch
		  // 	   <<" adc = "<<adc
		  // 	   <<std::endl;
		}
	    }
	}
      return;
    }

    //______________________________________________________________________
    void
    VmeV1290A::resize_fe_data()
    {
      m_fe_data.resize(k_n_ch);
      for(unsigned int ch = 0; ch<k_n_ch; ++ch){
	m_fe_data[ch].resize(k_n_module);
      }
      return;
    }
  }
}
