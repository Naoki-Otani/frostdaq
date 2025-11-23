// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__TOKENIZER_H
#define HDDAQ__TOKENIZER_H


#include <iostream>
#include <algorithm>
#include <bits/allocator.h>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <deque>
#include <iterator>

namespace hddaq
{

  template 
  <
    template <class, typename> class Sequence,
    class T = std::string,
    typename Allocator = std::allocator<std::string>
  >
  class SimpleTokenizer
  {
    
  public:
    typedef T                                    value_type;
    typedef Allocator                            allocator_type;
    typedef Sequence<value_type, allocator_type> result_type;
    typedef typename result_type::iterator       iterator;
    typedef typename result_type::const_iterator const_iterator;
    typedef typename value_type::size_type       size_type;

    static value_type get_nth(const value_type& input,
			      const value_type& delimiter,
			      size_type n);
    static void       tokenize(const value_type& input,
			       result_type& output,
			       const value_type& delimiter);

  };

//______________________________________________________________________________
template 
<
  template <class, typename> class Sequence,
  class T,
  typename Allocator
>
inline
typename SimpleTokenizer<Sequence, T, Allocator>::value_type
SimpleTokenizer<Sequence, T, Allocator>::get_nth(const value_type& input,
						 const value_type& delimiter,
						 size_type n)
{
  SimpleTokenizer<std::vector>::result_type ret;
  SimpleTokenizer<std::vector>::tokenize(input, ret, delimiter);
  if (ret.size()>n)
    return ret[n];
  else
    return "";
}

//______________________________________________________________________________
template 
<
  template <class, typename> class Sequence,
  class T,
  typename Allocator
>
inline
void
SimpleTokenizer<Sequence, T, Allocator>::tokenize(const value_type& input,
						  result_type& output,
						  const value_type& delimiter)
{
  result_type ret;
  value_type temp(input);
  for (;;)
    {
      size_type dpos = temp.find(delimiter); 
      value_type substring(temp, 0, dpos);
//       substring.erase(std::remove(substring.begin(), substring.end(), ' '),
// 		      substring.end());
      substring.erase(0, substring.find_first_not_of(" "));
      substring.erase(substring.find_last_not_of(" ")+1,
		      substring.length()-substring.find_last_not_of(" "));

      if (!substring.empty())
	ret.push_back(substring);
      if (value_type::npos==dpos)
	break;
      temp.erase(0, dpos+delimiter.length());
    }
  ret.swap(output);
  return;
}

//______________________________________________________________________________
typedef SimpleTokenizer<std::vector> Tokenizer;
typedef SimpleTokenizer<std::list>   TokenizerL;
typedef SimpleTokenizer<std::deque>  TokenizerD;



//______________________________________________________________________________
inline
void 
simple_parse(const std::string& input,
	     Tokenizer::result_type& mark,
	     Tokenizer::result_type& value)
{
  Tokenizer::result_type tmp;
  Tokenizer::result_type ret_mark;
  Tokenizer::result_type ret_value;
  Tokenizer::tokenize(input, tmp, "<");

//     std::copy(tmp.begin(), tmp.end(),
// 	      std::ostream_iterator<std::string>(std::cout, " "));
//     std::cout << std::endl;

  for (Tokenizer::result_type::const_iterator i = tmp.begin();
       i != tmp.end(); ++i)
    {
      if (std::string::npos!=i->find(">"))
	{
	  ret_mark.push_back(Tokenizer::get_nth(*i, ">", 0));
// 	    std::cout << "   -> mark : " << ret_mark.back() << std::endl;
	  ret_value.push_back(Tokenizer::get_nth(*i, ">", 1));
// 	    std::cout << "   -> val : " << ret_value.back() << std::endl;
	}
    }
  
  ret_mark.swap(mark);
  ret_value.swap(value);
  return;
}

}
#endif
