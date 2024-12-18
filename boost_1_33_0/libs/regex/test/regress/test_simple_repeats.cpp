/*
 *
 * Copyright (c) 2004
 * John Maddock
 *
 * Use, modification and distribution are subject to the 
 * Boost Software License, Version 1.0. (See accompanying file 
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "test.hpp"

#ifdef BOOST_MSVC
#pragma warning(disable:4127)
#endif

void test_simple_repeats()
{
   using namespace boost::regex_constants;
   // simple repeats:
   TEST_REGEX_SEARCH("a*", perl, "b", match_default, make_array(0, 0, -2, 1, 1, -2, -2));
   TEST_REGEX_SEARCH("ab*", perl, "ab", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("ab*", basic, "ab", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("ab*", perl, "sssabbbbbbsss", match_default, make_array(3, 10, -2, -2));
   TEST_REGEX_SEARCH("ab*c*", perl, "a", match_default, make_array(0, 1, -2, -2));
   TEST_REGEX_SEARCH("ab*c*", perl, "abbb", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab*c*", perl, "accc", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab*c*", perl, "abbcc", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("*a", basic, "*a", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("^*a", basic, "*a", match_default, make_array(0, 2, -2, -2));
   TEST_INVALID_REGEX("*a", perl);
   TEST_INVALID_REGEX("\\<*", perl);
   TEST_INVALID_REGEX("\\>*", perl);
   TEST_REGEX_SEARCH("\n*", perl, "\n\n", match_default, make_array(0, 2, -2, 2, 2, -2, -2));
   TEST_REGEX_SEARCH("\\**", perl, "**", match_default, make_array(0, 2, -2, 2, 2, -2, -2));
   TEST_REGEX_SEARCH("\\*", perl, "*", match_default, make_array(0, 1, -2, -2));
   TEST_REGEX_SEARCH("(ab)*", perl, "abab", match_default, make_array(0, 4, 2, 4, -2, 4, 4, -2, -2));

   // now try operator + :
   TEST_REGEX_SEARCH("ab+", perl, "a", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("ab+", perl, "ab", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("ab+", perl, "sssabbbbbbsss", match_default, make_array(3, 10, -2, -2));
   TEST_REGEX_SEARCH("ab+c+", perl, "abbb", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("ab+c+", perl, "accc", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("ab+c+", perl, "abbcc", match_default, make_array(0, 5, -2, -2));
   TEST_INVALID_REGEX("+a", perl);
   TEST_INVALID_REGEX("\\<+", perl);
   TEST_INVALID_REGEX("\\>+", perl);
   TEST_REGEX_SEARCH("\n+", perl, "\n\n", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("\\+", perl, "+", match_default, make_array(0, 1, -2, -2));
   TEST_REGEX_SEARCH("\\+", perl, "++", match_default, make_array(0, 1, -2, 1, 2, -2, -2));
   TEST_REGEX_SEARCH("\\++", perl, "++", match_default, make_array(0, 2, -2, -2));

   TEST_REGEX_SEARCH("+", basic|bk_plus_qm, "+", match_default, make_array(0, 1, -2, -2));
   TEST_INVALID_REGEX("\\+", basic|bk_plus_qm);
   TEST_REGEX_SEARCH("a\\+", basic|bk_plus_qm, "aa", match_default, make_array(0, 2, -2, -2));

   // now try operator ?
   TEST_REGEX_SEARCH("a?", perl, "b", match_default, make_array(0, 0, -2, 1, 1, -2, -2));
   TEST_REGEX_SEARCH("ab?", perl, "a", match_default, make_array(0, 1, -2, -2));
   TEST_REGEX_SEARCH("ab?", perl, "ab", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("ab?", perl, "sssabbbbbbsss", match_default, make_array(3, 5, -2, -2));
   TEST_REGEX_SEARCH("ab?c?", perl, "a", match_default, make_array(0, 1, -2, -2));
   TEST_REGEX_SEARCH("ab?c?", perl, "abbb", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("ab?c?", perl, "accc", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("ab?c?", perl, "abcc", match_default, make_array(0, 3, -2, -2));
   TEST_INVALID_REGEX("?a", perl);
   TEST_INVALID_REGEX("\\<?", perl);
   TEST_INVALID_REGEX("\\>?", perl);
   TEST_REGEX_SEARCH("\n?", perl, "\n\n", match_default, make_array(0, 1, -2, 1, 2, -2, 2, 2, -2, -2));
   TEST_REGEX_SEARCH("\\?", perl, "?", match_default, make_array(0, 1, -2, -2));
   TEST_REGEX_SEARCH("\\?", perl, "?", match_default, make_array(0, 1, -2, -2));
   TEST_REGEX_SEARCH("\\??", perl, "??", match_default, make_array(0, 1, -2, 1, 2, -2, 2, 2, -2, -2));
   TEST_REGEX_SEARCH("?", basic|bk_plus_qm, "?", match_default, make_array(0, 1, -2, -2));
   TEST_INVALID_REGEX("\\?", basic|bk_plus_qm);
   TEST_REGEX_SEARCH("a\\?", basic|bk_plus_qm, "aa", match_default, make_array(0, 1, -2, 1, 2, -2, 2, 2, -2, -2));
   TEST_REGEX_SEARCH("a\\?", basic|bk_plus_qm, "b", match_default, make_array(0, 0, -2, 1, 1, -2, -2));

   TEST_REGEX_SEARCH("a?", basic, "a?", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("a+", basic, "a+", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("a\\?", basic, "a?", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("a\\+", basic, "a+", match_default, make_array(0, 2, -2, -2));

   // now try operator {}
   TEST_REGEX_SEARCH("a{2}", perl, "a", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("a{2}", perl, "aa", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("a{2}", perl, "aaa", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("a{2,}", perl, "a", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("a{2,}", perl, "aa", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("a{2,}", perl, "aaaaa", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("a{2,4}", perl, "a", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("a{2,4}", perl, "aa", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("a{2,4}", perl, "aaa", match_default, make_array(0, 3, -2, -2));
   TEST_REGEX_SEARCH("a{2,4}", perl, "aaaa", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("a{2,4}", perl, "aaaaa", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("a{ 2 , 4 }", perl, "aaaaa", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("a{ 2 , }", perl, "aaaaa", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("a{ 2 }", perl, "aaa", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("a\\{\\}", perl, "a{}", match_default, make_array(0, 3, -2, -2));
   TEST_INVALID_REGEX("a{}", perl);
   TEST_INVALID_REGEX("a{", perl);
   TEST_INVALID_REGEX("a{1", perl);
   TEST_INVALID_REGEX("a{1,", perl);
   TEST_INVALID_REGEX("a{1,2", perl);
   TEST_INVALID_REGEX("a{ 1 , 2 ", perl);
   TEST_INVALID_REGEX("a{ }", perl);
   TEST_INVALID_REGEX("a}", perl);
   TEST_INVALID_REGEX("{1}", perl);
   TEST_INVALID_REGEX("a{b}", perl);
   TEST_INVALID_REGEX("a{1b}", perl);
   TEST_INVALID_REGEX("a{1,b}", perl);
   TEST_INVALID_REGEX("a{1,2v}", perl);
   TEST_INVALID_REGEX("a{2,1}", perl);

   // now try operator \\{\\} for POSIX basic regexes
   TEST_REGEX_SEARCH("a\\{2\\}", basic, "a", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("a\\{2\\}", basic|no_intervals, "a{2}", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("a\\{2\\}", basic, "aa", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("a\\{2\\}", basic, "aaa", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("a\\{2,\\}", basic, "a", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("a\\{2,\\}", basic, "aa", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("a\\{2,\\}", basic, "aaaaa", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("a\\{2,4\\}", basic, "a", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("a\\{2,4\\}", basic, "aa", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("a\\{2,4\\}", basic, "aaa", match_default, make_array(0, 3, -2, -2));
   TEST_REGEX_SEARCH("a\\{2,4\\}", basic, "aaaa", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("a\\{2,4\\}", basic, "aaaaa", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("a\\{ 2 , 4 \\}", basic, "aaaaa", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("a\\{ 2 , \\}", basic, "aaaaa", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("a\\{ 2 \\}", basic, "aaa", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("a{}", basic, "a{}", match_default, make_array(0, 3, -2, -2));
   TEST_INVALID_REGEX("a\\{\\}", basic);
   TEST_INVALID_REGEX("a\\{", basic);
   TEST_INVALID_REGEX("a\\{1", basic);
   TEST_INVALID_REGEX("a\\{1,", basic);
   TEST_INVALID_REGEX("a\\{1,\\", basic);
   TEST_INVALID_REGEX("a\\{ \\}", basic);
   TEST_INVALID_REGEX("a\\}", basic);
   TEST_INVALID_REGEX("\\{1\\}", basic);
   TEST_INVALID_REGEX("a\\{b\\}", basic);
   TEST_INVALID_REGEX("a\\{1b\\}", basic);
   TEST_INVALID_REGEX("a\\{1,b\\}", basic);
   TEST_INVALID_REGEX("a\\{1,2v\\}", basic);
   TEST_INVALID_REGEX("a\\{3,1\\}", basic);

}

void test_fast_repeats()
{
   using namespace boost::regex_constants;
   // extended repeat checking to exercise new algorithms:
   TEST_REGEX_SEARCH("ab.*xy", perl, "abxy_", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab.*xy", perl, "ab_xy_", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("ab.*xy", perl, "abxy", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab.*xy", perl, "ab_xy", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("ab.*", perl, "ab", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("ab.*", perl, "ab__", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH(".*xy", perl, "abc", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH(".*?xy", perl, "abc", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("a+?xy", perl, "abc", match_default, make_array(-2, -2));

   TEST_REGEX_SEARCH("ab.{2,5}xy", perl, "ab__xy_", match_default, make_array(0, 6, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}xy", perl, "ab____xy_", match_default, make_array(0, 8, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}xy", perl, "ab_____xy_", match_default, make_array(0, 9, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}xy", perl, "ab__xy", match_default, make_array(0, 6, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}xy", perl, "ab_____xy", match_default, make_array(0, 9, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}", perl, "ab__", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}", perl, "ab_______", match_default, make_array(0, 7, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}xy", perl, "ab______xy", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}xy", perl, "ab_xy", match_default, make_array(-2, -2));
   
   TEST_REGEX_SEARCH("ab.*?xy", perl, "abxy_", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab.*?xy", perl, "ab_xy_", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("ab.*?xy", perl, "abxy", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab.*?xy", perl, "ab_xy", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("ab.*?", perl, "ab", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("ab.*?", perl, "ab__", match_default, make_array(0, 2, -2, -2));
   
   TEST_REGEX_SEARCH("ab.{2,5}?xy", perl, "ab__xy_", match_default, make_array(0, 6, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}?xy", perl, "ab____xy_", match_default, make_array(0, 8, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}?xy", perl, "ab_____xy_", match_default, make_array(0, 9, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}?xy", perl, "ab__xy", match_default, make_array(0, 6, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}?xy", perl, "ab_____xy", match_default, make_array(0, 9, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}?", perl, "ab__", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}?", perl, "ab_______", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}?xy", perl, "ab______xy", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}xy", perl, "ab_xy", match_default, make_array(-2, -2));

   // again but with slower algorithm variant:
   TEST_REGEX_SEARCH("ab.*xy", perl, "abxy_", match_not_dot_newline|match_not_dot_null, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab.*xy", perl, "ab_xy_", match_not_dot_newline|match_not_dot_null, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("ab.*xy", perl, "abxy", match_not_dot_newline|match_not_dot_null, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab.*xy", perl, "ab_xy", match_not_dot_newline|match_not_dot_null, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("ab.*", perl, "ab", match_not_dot_newline|match_not_dot_null, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("ab.*", perl, "ab__", match_not_dot_newline|match_not_dot_null, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH(".*xy", perl, "abc", match_not_dot_newline|match_not_dot_null, make_array(-2, -2));
   TEST_REGEX_SEARCH(".*?xy", perl, "abc", match_not_dot_newline|match_not_dot_null, make_array(-2, -2));
   TEST_REGEX_SEARCH(".*xy", perl, "ab\nbc", match_not_dot_newline|match_not_dot_null, make_array(-2, -2));
   TEST_REGEX_SEARCH(".*?xy", perl, "ax\nbc", match_not_dot_newline|match_not_dot_null, make_array(-2, -2));

   TEST_REGEX_SEARCH("ab.{2,5}xy", perl, "ab__xy_", match_not_dot_newline|match_not_dot_null, make_array(0, 6, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}xy", perl, "ab____xy_", match_not_dot_newline|match_not_dot_null, make_array(0, 8, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}xy", perl, "ab_____xy_", match_not_dot_newline|match_not_dot_null, make_array(0, 9, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}xy", perl, "ab__xy", match_not_dot_newline|match_not_dot_null, make_array(0, 6, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}xy", perl, "ab_____xy", match_not_dot_newline|match_not_dot_null, make_array(0, 9, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}", perl, "ab__", match_not_dot_newline|match_not_dot_null, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}", perl, "ab_______", match_not_dot_newline|match_not_dot_null, make_array(0, 7, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}xy", perl, "ab______xy", match_not_dot_newline|match_not_dot_null, make_array(-2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}xy", perl, "ab_xy", match_not_dot_newline|match_not_dot_null, make_array(-2, -2));
   
   TEST_REGEX_SEARCH("ab.*?xy", perl, "abxy_", match_not_dot_newline|match_not_dot_null, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab.*?xy", perl, "ab_xy_", match_not_dot_newline|match_not_dot_null, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("ab.*?xy", perl, "abxy", match_not_dot_newline|match_not_dot_null, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab.*?xy", perl, "ab_xy", match_not_dot_newline|match_not_dot_null, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("ab.*?", perl, "ab", match_not_dot_newline|match_not_dot_null, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("ab.*?", perl, "ab__", match_not_dot_newline|match_not_dot_null, make_array(0, 2, -2, -2));
   
   TEST_REGEX_SEARCH("ab.{2,5}?xy", perl, "ab__xy_", match_not_dot_newline|match_not_dot_null, make_array(0, 6, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}?xy", perl, "ab____xy_", match_not_dot_newline|match_not_dot_null, make_array(0, 8, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}?xy", perl, "ab_____xy_", match_not_dot_newline|match_not_dot_null, make_array(0, 9, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}?xy", perl, "ab__xy", match_not_dot_newline|match_not_dot_null, make_array(0, 6, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}?xy", perl, "ab_____xy", match_not_dot_newline|match_not_dot_null, make_array(0, 9, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}?", perl, "ab__", match_not_dot_newline|match_not_dot_null, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}?", perl, "ab_______", match_not_dot_newline|match_not_dot_null, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}?xy", perl, "ab______xy", match_not_dot_newline|match_not_dot_null, make_array(-2, -2));
   TEST_REGEX_SEARCH("ab.{2,5}xy", perl, "ab_xy", match_not_dot_newline|match_not_dot_null, make_array(-2, -2));

   // now again for single character repeats:
   TEST_REGEX_SEARCH("ab_*xy", perl, "abxy_", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab_*xy", perl, "ab_xy_", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("ab_*xy", perl, "abxy", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab_*xy", perl, "ab_xy", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("ab_*", perl, "ab", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("ab_*", perl, "ab__", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab_*?z", perl, "ab__", match_default, make_array(-2, -2));

   TEST_REGEX_SEARCH("ab_{2,5}xy", perl, "ab__xy_", match_default, make_array(0, 6, -2, -2));
   TEST_REGEX_SEARCH("ab_{2,5}xy", perl, "ab____xy_", match_default, make_array(0, 8, -2, -2));
   TEST_REGEX_SEARCH("ab_{2,5}xy", perl, "ab_____xy_", match_default, make_array(0, 9, -2, -2));
   TEST_REGEX_SEARCH("ab_{2,5}xy", perl, "ab__xy", match_default, make_array(0, 6, -2, -2));
   TEST_REGEX_SEARCH("ab_{2,5}xy", perl, "ab_____xy", match_default, make_array(0, 9, -2, -2));
   TEST_REGEX_SEARCH("ab_{2,5}", perl, "ab__", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab_{2,5}", perl, "ab_______", match_default, make_array(0, 7, -2, -2));
   TEST_REGEX_SEARCH("ab_{2,5}xy", perl, "ab______xy", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("ab_{2,5}xy", perl, "ab_xy", match_default, make_array(-2, -2));
   
   TEST_REGEX_SEARCH("ab_*?xy", perl, "abxy_", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab_*?xy", perl, "ab_xy_", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("ab_*?xy", perl, "abxy", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab_*?xy", perl, "ab_xy", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("ab_*?", perl, "ab", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("ab_*?", perl, "ab__", match_default, make_array(0, 2, -2, -2));
   
   TEST_REGEX_SEARCH("ab_{2,5}?xy", perl, "ab__xy_", match_default, make_array(0, 6, -2, -2));
   TEST_REGEX_SEARCH("ab_{2,5}?xy", perl, "ab____xy_", match_default, make_array(0, 8, -2, -2));
   TEST_REGEX_SEARCH("ab_{2,5}?xy", perl, "ab_____xy_", match_default, make_array(0, 9, -2, -2));
   TEST_REGEX_SEARCH("ab_{2,5}?xy", perl, "ab__xy", match_default, make_array(0, 6, -2, -2));
   TEST_REGEX_SEARCH("ab_{2,5}?xy", perl, "ab_____xy", match_default, make_array(0, 9, -2, -2));
   TEST_REGEX_SEARCH("ab_{2,5}?", perl, "ab__", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab_{2,5}?", perl, "ab_______", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab_{2,5}?xy", perl, "ab______xy", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("ab_{2,5}xy", perl, "ab_xy", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("(5*?).somesite", perl, "//555.somesite", match_default, make_array(2, 14, 2, 5, -2, -2));
}

void test_fast_repeats2()
{
   using namespace boost::regex_constants;
   // and again for sets:
   TEST_REGEX_SEARCH("ab[_,;]*xy", perl, "abxy_", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]*xy", perl, "ab_xy_", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]*xy", perl, "abxy", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]*xy", perl, "ab_xy", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]*", perl, "ab", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]*", perl, "ab__", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]*?xy", perl, "ab__z", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("ab[_,;]*?z", perl, "ab__", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("ab[_,;]*?.z", perl, "ab__,;,__z", match_default, make_array(0, 10, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]*?.z", perl, "ab__,;,__y", match_default, make_array(-2, -2));

   TEST_REGEX_SEARCH("ab[_,;]{2,5}xy", perl, "ab__xy_", match_default, make_array(0, 6, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]{2,5}xy", perl, "ab____xy_", match_default, make_array(0, 8, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]{2,5}xy", perl, "ab_____xy_", match_default, make_array(0, 9, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]{2,5}xy", perl, "ab__xy", match_default, make_array(0, 6, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]{2,5}xy", perl, "ab_____xy", match_default, make_array(0, 9, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]{2,5}", perl, "ab__", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]{2,5}", perl, "ab_______", match_default, make_array(0, 7, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]{2,5}xy", perl, "ab______xy", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("ab[_,;]{2,5}xy", perl, "ab_xy", match_default, make_array(-2, -2));
   
   TEST_REGEX_SEARCH("ab[_,;]*?xy", perl, "abxy_", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]*?xy", perl, "ab_xy_", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]*?xy", perl, "abxy", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]*?xy", perl, "ab_xy", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]*?", perl, "ab", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]*?", perl, "ab__", match_default, make_array(0, 2, -2, -2));
   
   TEST_REGEX_SEARCH("ab[_,;]{2,5}?xy", perl, "ab__xy_", match_default, make_array(0, 6, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]{2,5}?xy", perl, "ab____xy_", match_default, make_array(0, 8, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]{2,5}?xy", perl, "ab_____xy_", match_default, make_array(0, 9, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]{2,5}?xy", perl, "ab__xy", match_default, make_array(0, 6, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]{2,5}?xy", perl, "ab_____xy", match_default, make_array(0, 9, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]{2,5}?", perl, "ab__", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]{2,5}?", perl, "ab_______", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab[_,;]{2,5}?xy", perl, "ab______xy", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("ab[_,;]{2,5}xy", perl, "ab_xy", match_default, make_array(-2, -2));

   // and again for tricky sets with digraphs:
   TEST_REGEX_SEARCH("ab[_[.ae.]]*xy", perl, "abxy_", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]*xy", perl, "ab_xy_", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]*xy", perl, "abxy", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]*xy", perl, "ab_xy", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]*", perl, "ab", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]*", perl, "ab__", match_default, make_array(0, 4, -2, -2));

   TEST_REGEX_SEARCH("ab[_[.ae.]]{2,5}xy", perl, "ab__xy_", match_default, make_array(0, 6, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]{2,5}xy", perl, "ab____xy_", match_default, make_array(0, 8, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]{2,5}xy", perl, "ab_____xy_", match_default, make_array(0, 9, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]{2,5}xy", perl, "ab__xy", match_default, make_array(0, 6, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]{2,5}xy", perl, "ab_____xy", match_default, make_array(0, 9, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]{2,5}", perl, "ab__", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]{2,5}", perl, "ab_______", match_default, make_array(0, 7, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]{2,5}xy", perl, "ab______xy", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]{2,5}xy", perl, "ab_xy", match_default, make_array(-2, -2));
   
   TEST_REGEX_SEARCH("ab[_[.ae.]]*?xy", perl, "abxy_", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]*?xy", perl, "ab_xy_", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]*?xy", perl, "abxy", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]*?xy", perl, "ab_xy", match_default, make_array(0, 5, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]*?", perl, "ab", match_default, make_array(0, 2, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]*?", perl, "ab__", match_default, make_array(0, 2, -2, -2));
   
   TEST_REGEX_SEARCH("ab[_[.ae.]]{2,5}?xy", perl, "ab__xy_", match_default, make_array(0, 6, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]{2,5}?xy", perl, "ab____xy_", match_default, make_array(0, 8, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]{2,5}?xy", perl, "ab_____xy_", match_default, make_array(0, 9, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]{2,5}?xy", perl, "ab__xy", match_default, make_array(0, 6, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]{2,5}?xy", perl, "ab_____xy", match_default, make_array(0, 9, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]{2,5}?", perl, "ab__", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]{2,5}?", perl, "ab_______", match_default, make_array(0, 4, -2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]{2,5}?xy", perl, "ab______xy", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("ab[_[.ae.]]{2,5}xy", perl, "ab_xy", match_default, make_array(-2, -2));
   TEST_REGEX_SEARCH("([5[.ae.]]*?).somesite", perl, "//555.somesite", match_default, make_array(2, 14, 2, 5, -2, -2));
}

