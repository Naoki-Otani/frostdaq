// -*- C++ -*-

// Author: Tomonori Takahashi

#include "XMLRead.hh"

#include "std_ostream.hh"
#include "replace_string.hh"
#include "lexical_cast.hh"

namespace hddaq
{
  namespace xml
  {

//______________________________________________________________________________
XMLRead::XMLRead(DOMElement* e)
  : m_e(e),
    m_variables(),
    m_loop_depth()
{
}

//______________________________________________________________________________
XMLRead::~XMLRead()
{
}

//______________________________________________________________________________
void
XMLRead::hoge(const std::string& arg) const
{
  cout << "#D XMLRead::hoge(" << arg << ")" << std::endl;
  return;
}

//______________________________________________________________________________
void
XMLRead::generate_content(DOMElement* e)
{
  return;
}

//______________________________________________________________________________
void
XMLRead::read(DOMElement* e)
{
  return;
}

//______________________________________________________________________________
void
XMLRead::read_loop(DOMElement* e)
{
  std::string var;
  std::string from="0";
  std::string to  ="0";
  std::string step="1";

  std::vector<std::string> attr_name;
  std::vector<std::string> attr_value;
  get_attributes(e, attr_name, attr_value);
  const int n = attr_name.size();
  for (int i=0; i<n; ++i)
    {
      const std::string& name  = attr_name[i];
      const std::string& value = attr_value[i];
      if      (name=="var")
	var = value;
      else if (name=="from")
	from = value;
      else if (name=="to")
	to   = value;
      else if (name=="step")
	step = value;
    }

  if (var.empty())
    {
      cerr << "\n#E XMLRead::read_loop()\n"
	     << " invalid read. No variable is determined for loop"
	     << std::endl;
    }

  const int ifrom = a2i(from);
  const int ito   = a2i(to);
  const int istep = a2i(step);
  int ivar        = ifrom;

//   if (ifrom==ito)
//     cerr << "\n#E XMLRead::read_loop()"
// 	      << " empty loop" << std::endl;
  if ((ifrom<ito) && (istep<=0))
    cerr << "\n#E XMLRead::read_loop()"
	      << " infinite loop" << std::endl;
  if ((ifrom>ito) && (istep>=0))
    cerr << "\n#E XMLRead::read_loop()"
	 << " infinite loop" << std::endl;

  ++m_loop_depth;

//   cout << "#D in loop = " << m_loop_depth << std::endl;
  while (false
	 || ((istep>=0) && (ivar<=ito))
	 || ((istep<=0) && (ivar>=ito))
	 )
    {
//       cout << "#D loop " << var << " " << ivar << std::endl;
      m_variables[var] = ivar;
      read(e);
      ivar += istep;
    }
  --m_loop_depth;

//   if (m_loop_depth==0)
//     m_variables.clear();

  return;
}

//______________________________________________________________________________
void
XMLRead::replace_loop_variables(std::string& var)
{
  for (std::map<std::string, double>::iterator i = m_variables.begin();
       i!=m_variables.end(); ++i)
    {
      const std::string& loop_var(i->first);
      const std::string& number(d2a(i->second));
      if (var.find(loop_var)!=std::string::npos)
	replace_all(var, loop_var, number);
    }
  return;
}

  }
}
