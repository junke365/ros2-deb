/*=============================================================================
    Copyright (c) 1999-2003 Jaakko J�rvi
    Copyright (c) 2001-2006 Joel de Guzman
    Copyright (c) 2006 Dan Marsden

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/fusion/container/deque/deque.hpp>
#include <boost/fusion/container/deque/convert.hpp>
#include <boost/fusion/adapted/mpl.hpp>

#define FUSION_SEQUENCE deque
#include "misc.hpp"

int
main()
{
    test();
    return boost::report_errors();
}

