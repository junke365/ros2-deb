/* Boost.MultiIndex test for modifier memfuns.
 *
 * Copyright 2003-2007 Joaqu�n M L�pez Mu�oz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/multi_index for library home page.
 */

#include "test_modifiers.hpp"

#include <boost/config.hpp> /* keep it first to prevent nasty warns in MSVC */
#include <boost/next_prior.hpp>
#include <iterator>
#include <vector>
#include "pre_multi_index.hpp"
#include "employee.hpp"
#include <boost/next_prior.hpp>
#include <boost/test/test_tools.hpp>

class always_one
{
public:
  always_one():n(1){}
  ~always_one(){n=0;}

  int get()const{return n;}

private:
  int n;
};


inline bool operator==(const always_one& x,const always_one& y)
{
  return x.get()==y.get();
}

#if defined(BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP)
namespace boost{
#endif

inline std::size_t hash_value(const always_one& x)
{
  return static_cast<std::size_t>(x.get());
}

#if defined(BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP)
} /* namespace boost */
#endif

using namespace boost::multi_index;

void test_modifiers()
{
  employee_set              es;
  employee_set_by_name&     i1=get<name>(es);
  employee_set_by_age&      i2=get<age>(es);
  employee_set_as_inserted& i3=get<as_inserted>(es);
  employee_set_by_ssn&      i4=get<ssn>(es);
  employee_set_randomly&    i5=get<randomly>(es);

  es.insert(employee(0,"Joe",31,1123));
  BOOST_CHECK(es.insert(employee(0,"Joe",31,1123)).second==false);
  BOOST_CHECK(i1.insert(employee(0,"Joe Jr.",5,2563)).second==false);
  BOOST_CHECK(i2.insert(employee(1,"Victor",5,1123)).second==false);
  BOOST_CHECK(i3.insert(i3.begin(),employee(1,"Victor",5,1123)).second
                ==false);
  BOOST_CHECK(i3.push_front(employee(0,"Joe Jr.",5,2563)).second==false);
  BOOST_CHECK(i3.push_back(employee(0,"Joe Jr.",5,2563)).second==false);

  employee_set_by_name::iterator it1=i1.find("Joe");
  i1.insert(it1,employee(1,"Joe Jr.",5,2563));
  BOOST_CHECK(es.size()==2);

  employee_set_by_age::iterator it2=i2.find(31);
  i2.insert(it2,employee(2,"Grandda Joe",64,7881));
  BOOST_CHECK(es.size()==3);

  employee_set_as_inserted::iterator it3=i3.begin();
  i3.insert(it3,100,employee(3,"Judy",39,6201));
  BOOST_CHECK((--it3)->ssn==6201);
  BOOST_CHECK(es.size()==4);

  employee_set_randomly::iterator it5=i5.begin();
  i5.insert(it5,100,employee(4,"Jill",52,3379));
  BOOST_CHECK(i5.begin()->age==52);
  BOOST_CHECK(es.size()==5);

  es.erase(employee(1,"Joe Jr.",5,2563));
  BOOST_CHECK(i3.size()==4&&i5.size()==4);

  BOOST_CHECK(i1.erase("Judy")==1);
  BOOST_CHECK(es.size()==3&&i2.size()==3);

  BOOST_CHECK(i2.erase(it2)->age==52);
  BOOST_CHECK(i3.size()==2&&i4.size()==2);

  i3.pop_front();
  BOOST_CHECK(i1.size()==1&&i2.size()==1);

  i5.erase(i5.begin(),i5.end());
  BOOST_CHECK(es.size()==0&&i3.size()==0);

  es.insert(employee(0,"Joe",31,1123));
  BOOST_CHECK(i1.erase(i1.begin())==i1.end());
  BOOST_CHECK(i1.size()==0);

  es.insert(employee(0,"Joe",31,1123));
  es.insert(employee(1,"Jack",31,5032));
  BOOST_CHECK(i2.erase(31)==2);
  BOOST_CHECK(i2.size()==0);

  i3.push_front(employee(1,"Jack",31,5032));
  i3.push_back(employee(0,"Joe",31,1123));
  BOOST_CHECK(i3.front()==employee(1,"Jack",31,5032));
  BOOST_CHECK(i3.back()==employee(0,"Joe",31,1123));

  i3.pop_back();
  BOOST_CHECK(i3.back()==employee(1,"Jack",31,5032));
  BOOST_CHECK(es.size()==1);

  i3.pop_front();
  BOOST_CHECK(es.size()==0);

  i5.push_back(employee(1,"Jack",31,5032));
  i5.push_front(employee(0,"Joe",31,1123));
  i5.insert(i5.end()-1,employee(2,"Grandda Joe",64,7881));
  BOOST_CHECK(i5.back()==employee(1,"Jack",31,5032));
  BOOST_CHECK(i5.front()==employee(0,"Joe",31,1123));
  BOOST_CHECK(i5[0]==i5.front()&&i5.at(0)==i5.front());
  BOOST_CHECK(i5[i5.size()-1]==i5.back()&&i5.at(i5.size()-1)==i5.back());

  i5.pop_front();
  BOOST_CHECK(i5.back()==employee(1,"Jack",31,5032));
  BOOST_CHECK(i5.front()==employee(2,"Grandda Joe",64,7881));
  BOOST_CHECK(es.size()==2);

  i5.pop_back();
  BOOST_CHECK(i5.back()==employee(2,"Grandda Joe",64,7881));
  BOOST_CHECK(i5.front()==i5.front());
  BOOST_CHECK(es.size()==1);

  i5.erase(i5.begin());
  BOOST_CHECK(es.size()==0);
  
  std::vector<employee> ve;
  ve.push_back(employee(3,"Anna",31,5388));
  ve.push_back(employee(1,"Rachel",27,9012));
  ve.push_back(employee(2,"Agatha",40,1520));

  i1.insert(ve.begin(),ve.end());
  BOOST_CHECK(i2.size()==3);

  BOOST_CHECK(i2.erase(i2.begin(),i2.end())==i2.end());
  BOOST_CHECK(es.size()==0);

  i2.insert(ve.begin(),ve.end());
  BOOST_CHECK(i3.size()==3);

  BOOST_CHECK(*(i3.erase(i3.begin()))==employee(1,"Rachel",27,9012));
  BOOST_CHECK(i3.erase(i3.begin(),i3.end())==i3.end());
  BOOST_CHECK(es.size()==0);

  i3.insert(i3.end(),ve.begin(),ve.end());
  BOOST_CHECK(es.size()==3);

  BOOST_CHECK(i4.erase(9012)==1);
  i4.erase(i4.begin());
  BOOST_CHECK(i4.erase(i4.begin(),i4.end())==i4.end());

  i4.insert(ve.begin(),ve.end());
  BOOST_CHECK(i5.size()==3);

  BOOST_CHECK(i5.erase(i5.begin(),i5.end())==i5.end());
  BOOST_CHECK(es.size()==0);

  i5.insert(i5.begin(),ve.begin(),ve.end());
  BOOST_CHECK(i1.size()==3);

  BOOST_CHECK(es.erase(es.begin(),es.end())==es.end());
  BOOST_CHECK(i2.size()==0);

  es.insert(employee(0,"Joe",31,1123));
  es.insert(employee(1,"Robert",27,5601));
  es.insert(employee(2,"John",40,7889));
  es.insert(employee(3,"Albert",20,9012));
  es.insert(employee(4,"John",57,1002));

  employee_set es_backup(es);

  employee_set es2;
  es2.insert(employee(3,"Anna",31,5388));
  es2.insert(employee(1,"Rachel",27,9012));
  es2.insert(employee(2,"Agatha",40,1520));

  employee_set es2_backup(es2);

  i1.swap(get<1>(es2));
  BOOST_CHECK(es==es2_backup&&es2==es_backup);

  i2.swap(get<2>(es2));
  BOOST_CHECK(es==es_backup&&es2==es2_backup);

  i3.swap(get<3>(es2));
  BOOST_CHECK(es==es2_backup&&es2==es_backup);

  i4.swap(get<4>(es2));
  BOOST_CHECK(es==es_backup&&es2==es2_backup);

  i5.swap(get<5>(es2));
  BOOST_CHECK(es==es2_backup&&es2==es_backup);

#if defined(BOOST_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL)
  ::boost::multi_index::detail::swap(i1,get<1>(es2));
#else
  using std::swap;
  swap(i1,get<1>(es2));
#endif

  BOOST_CHECK(es==es_backup&&es2==es2_backup);

#if defined(BOOST_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL)
  ::boost::multi_index::detail::swap(i2,get<2>(es2));
#else
  using std::swap;
  swap(i2,get<2>(es2));
#endif

  BOOST_CHECK(es==es2_backup&&es2==es_backup);

#if defined(BOOST_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL)
  ::boost::multi_index::detail::swap(i3,get<3>(es2));
#else
  using std::swap;
  swap(i3,get<3>(es2));
#endif

  BOOST_CHECK(es==es_backup&&es2==es2_backup);

#if defined(BOOST_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL)
  ::boost::multi_index::detail::swap(i4,get<4>(es2));
#else
  using std::swap;
  swap(i4,get<4>(es2));
#endif

  BOOST_CHECK(es==es2_backup&&es2==es_backup);

#if defined(BOOST_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL)
  ::boost::multi_index::detail::swap(i5,get<5>(es2));
#else
  using std::swap;
  swap(i5,get<5>(es2));
#endif

  BOOST_CHECK(es==es_backup&&es2==es2_backup);

  i3.clear();
  BOOST_CHECK(i3.size()==0);

  es=es2;
  i4.clear();
  BOOST_CHECK(i4.size()==0);

  es=es2;
  i5.clear();
  BOOST_CHECK(i5.size()==0);

  es2.clear();
  BOOST_CHECK(es2.size()==0);

  /* testcase for problem reported at
   * http://lists.boost.org/boost-users/2006/12/24215.php
   */

  multi_index_container<
    always_one,
    indexed_by<
      hashed_non_unique<identity<always_one> >
    >
  > aoc;

  aoc.insert(always_one());
  aoc.insert(always_one());
  aoc.erase(*(aoc.begin()));
  BOOST_CHECK(aoc.empty());

  /* Testcases for compliance with "as close to hint as possible"
   * proposed behavior for associative containers:
   *   http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#233
   *   http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2005/n1780.html
   */

  typedef multi_index_container<
    int,
    indexed_by<
      ordered_non_unique<identity<int> >
    >
  > int_non_unique_container;

  int_non_unique_container c;
  c.insert(0);c.insert(0);
  c.insert(1);c.insert(1);
  c.insert(2);c.insert(2);

  BOOST_CHECK(std::distance(c.begin(),c.insert(c.begin(),1))==2);
  BOOST_CHECK(std::distance(c.begin(),c.insert(boost::next(c.begin()),1))==2);
  BOOST_CHECK(std::distance(c.begin(),c.insert(c.lower_bound(1),1))==2);
  BOOST_CHECK(
    std::distance(c.begin(),c.insert(boost::next(c.lower_bound(1)),1))==3);
  BOOST_CHECK(std::distance(c.begin(),c.insert(c.upper_bound(1),1))==8);
  BOOST_CHECK(std::distance(c.begin(),c.insert(boost::prior(c.end()),1))==9);
  BOOST_CHECK(std::distance(c.begin(),c.insert(c.end(),1))==10);
}
