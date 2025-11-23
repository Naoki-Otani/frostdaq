// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__UNPAKCER_H
#define HDDAQ__UNPACKER_H

#include <stdint.h>
#include <string>
#include <vector>
#include <map>

#include "Uncopyable.hh"
#include "Tag.hh"
#include "defines.hh"

namespace hddaq 
{

  namespace unpacker
  {

    class UnpackerImpl;

  class Unpacker 
    : private Uncopyable<Unpacker>
  {
    
  public:
    typedef std::vector<uint32_t>::iterator       iterator;
    typedef std::vector<uint32_t>::const_iterator const_iterator;
    typedef defines::unpacker_type                unpacker_type;
    typedef defines::error_state_t                error_state_t;
    typedef defines::check_mode_t                 check_mode_t;
    typedef UnpackerImpl                          Impl;

  private:
    Impl* m_impl;

  public:
    explicit  Unpacker(const unpacker_type& type);
    virtual  ~Unpacker();
    
    void                 hoge(const std::string& arg="") const;
    void                 add_channel_map(int fe_ch,
					 int fe_data,
					 DigitList& digit_list,
					 int device,
					 int plane,
					 int segment,
					 int ch,
					 int data_type);
    void                 add_channel_map(FrontEndData& fe_data,
					 DigitList& digit_list);
    void                 check();
    void                 clear();
    void                 decode();
    static void          dump_data(const Impl& impl, 
				   defines::e_dump_mode mode=defines::k_hex);
    void                 dump_data(defines::e_dump_mode mode=defines::k_no_dump) const;
    void                 fill(unsigned int ch,
			      unsigned int data_type,
			      unsigned long long data);
    unsigned long long   get(unsigned int ch, 
			     unsigned int data_type, 
			     unsigned int hit_id) const;
    Impl&                get() const;
    void                 get_buffer(iterator& begin,
				    iterator& end) const;
    iterator             get_buffer_begin() const;
    iterator             get_buffer_end() const;
    const check_mode_t&  get_check_mode() const;
    Unpacker*            get_child(uint64_t id) const;
    const std::map<uint64_t, Unpacker*>& get_child_list() const;
    const FrontEndData&  get_data() const;
    int                  get_data_id(const std::string& name) const;
    unsigned int         get_data_size() const;
    unsigned int         get_dump_mode() const;
    unsigned int         get_entries(unsigned int ch, 
				     unsigned int data_type) const;
    const error_state_t& get_error_state();
    unsigned int         get_fe_info(int data_type) const;
    unsigned int         get_header(int data_type) const;
    uint64_t             get_id() const;
    const std::string&   get_name() const;
    Unpacker*            get_parent() const;
    int                  get_run_number() const;
    const Tag&           get_tag(int tag_type = k_tag_current) const;
    void                 get_tag(std::vector<Tag>& tag_current) const;
    void                 get_time(std::vector<uint64_t>& sec,
 				  std::vector<uint64_t>& nsec) const;
    void                 get_time(uint64_t& sec,
 				  uint64_t& nsec) const;
    const unpacker_type& get_type() const;
    unsigned int         get_word_count() const;
    bool                 has_tag(int t) const;
    bool                 is_bad(int err_type = defines::k_bad_bit) const;
    bool                 is_decode_mode() const;
    bool                 is_esc_on() const;
    bool                 is_good();
    bool                 is_ready() const;
    bool                 is_related(int device_id,
				    int plane_id=-1) const;
    bool                 is_node() const;
    bool                 is_unpack_mode() const;
    void                 print() const;
    void                 print_tag() const;
    void                 reset();
    void                 reset_dump_mode(unsigned int dump_mode);
    void                 resize_fe_data();
    static void          set_check_mode(const check_mode_t& mode,
					const std::string& char_true="",
					const std::string& char_false="");
    void                 set_container();
    void                 set_container(FrontEndData& fe_data);
    void                 set_data(unsigned int data);
    void                 set_data(const iterator& data_first,
				  const iterator& data_last);
    void                 set_data_size(unsigned int data_size);
    void                 set_decode_mode(bool decode_mode);
    void                 set_dump_mode(unsigned int dump_mode);
    static void          set_esc(bool flag);
    void                 set_id(uint64_t id);
    void                 set_impl(Impl* impl);
    void                 set_name(const std::string& name="");
    static void          set_null_device_id(int device_id);
    void                 set_parent(Unpacker* parent);
    void                 set_state(defines::e_error_state error_bit);
    void                 set_unpack_mode(bool flag);
    void                 show_summary();
    void                 sort_child();
    bool                 unpack();

  };

  }
}
#endif
