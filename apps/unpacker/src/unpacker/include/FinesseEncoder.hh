// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__FINESSE_ENCODER_UNPACKER_H
#define HDDAQ__FINESSE_ENCODER_UNPACKER_H

#include "Uncopyable.hh"
#include "Finesse.hh"

namespace hddaq
{
  namespace unpacker
  {

  class FinesseEncoder 
    : public Finesse,
      private Uncopyable<FinesseEncoder>
  {
  public:
    static const unpacker_type k_type;
    static const unsigned int  k_n_ch = 32U;
    enum e_data_type
      {
	k_leading,
	k_trailing,
	k_n_data_type
      };

  public:
    FinesseEncoder(const unpacker_type& type);
    virtual ~FinesseEncoder();

    virtual bool     test(unsigned int bits) const;

  protected:
    virtual uint32_t calc_check_sum() const;
    virtual void     decode();
    virtual const std::map<std::string, int>&
                     get_data_id() const;
    int              init_decode_param(std::vector<uint32_t>& l_ch,
				       std::vector<uint32_t>& h_ch,
				       std::vector<uint32_t>& mask,
				       unsigned int bits);
    virtual void     resize_fe_data();

  };


  }
}
#endif
