// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__UNPACKER_XML_READ_DIGIT_H
#define HDDAQ__UNPACKER_XML_READ_DIGIT_H

#include <map>
#include <string>
#include <deque>
#include <vector>

#include "xml_helper.hh"
#include "XMLRead.hh"

#include "Uncopyable.hh"
#include "defines.hh"

namespace hddaq
{

namespace unpacker
{

class UnpackerXMLReadDigit
  : public xml::XMLRead,
    private Uncopyable<UnpackerXMLReadDigit>
{

public:
  // map : name -> index
  typedef std::map<std::string, int> Ref;
  typedef std::vector<Ref>           PlaneRef;
  typedef std::vector<PlaneRef>      SegmentRef;
  typedef std::vector<SegmentRef>    ChRef;
  typedef std::vector<ChRef>         DataRef;

  // map : index -> name
  typedef std::vector<std::string>     NameList;
  typedef std::vector<NameList>        PlaneNameList;
  typedef std::vector<PlaneNameList>   SegmentNameList;
  typedef std::vector<SegmentNameList> ChNameList;
  typedef std::vector<ChNameList>      DataNameList;

  // map : index -> type
  using TypeList = std::map<int, std::string>;

private:
  std::string m_tag;
  DigitList&  m_digit_list;
  Ref         m_device_ref;
  PlaneRef    m_plane_ref;
  SegmentRef  m_segment_ref;
  ChRef       m_ch_ref;
  DataRef     m_data_ref;

  NameList        m_device_names;
  PlaneNameList   m_plane_names;
  SegmentNameList m_segment_names;
  ChNameList      m_ch_names;
  DataNameList    m_data_names;

  TypeList m_device_types;

//       int m_device_id;
//       int m_plane_id;
//       int m_segment_id;
//       int m_ch_id;
  unsigned int m_device_id;
  unsigned int m_plane_id;
  unsigned int m_segment_id;
  unsigned int m_ch_id;

  int m_null_device_id;

public:
  UnpackerXMLReadDigit(DOMElement* e,
                       DigitList& digit_list);
  virtual ~UnpackerXMLReadDigit();

  const std::string& get_device_type(int device_id) const;
  const std::string& get_device_type(const std::string& name) const;
  int  get_device_id(const std::string& name) const;
  int  get_plane_id(int device_id,
                    const std::string& name) const;
  int  get_segment_id(int device_id,
                      int plane_id,
                      const std::string& name) const;
  int  get_ch_id(int device_id,
                 int plane_id,
                 int segment_id,
                 const std::string& name) const;
  int  get_data_id(int device_id,
                   int plane_id,
                   int segment_id,
                   int ch_id,
                   const std::string& name) const;

  const NameList& get_name_list() const;
  const NameList& get_name_list(int device_id) const;
  const NameList& get_name_list(int device_id,
                                int plane_id) const;
  const NameList& get_name_list(int device_id,
                                int plane_id,
                                int segment_id) const;
  const NameList& get_name_list(int device_id,
                                int plane_id,
                                int segment_id,
                                int ch_id) const;

  unsigned int  get_n_device() const;
  unsigned int  get_n_plane(int device_id) const;
  unsigned int  get_n_segment(int device_id,
                              int plane_id=0) const;
  unsigned int  get_n_ch(int device_id,
                         int plane_id=0,
                         int segment_id=0) const;
  unsigned int  get_n_data(int device_id,
                           int plane_id=0,
                           int segment_id=0,
                           int ch_id=0) const;

  int  get_null_device_id() const;
  void print(const std::string& arg="") const;

protected:
  virtual void hoge(const std::string& arg="") const;
//       virtual void generate_content(DOMElement* e);
  virtual void read(DOMElement* e);

private:
  void read_device_digit(DOMElement* e);
  void read_plane_digit(DOMElement* e);
  void read_segment_digit(DOMElement* e);
  void read_ch_digit(DOMElement* e);
  void read_data_digit(DOMElement* e);
};

}

}
#endif
