// (C) Copyright Jonathan Turkanis 2004
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#include <fstream>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include "detail/temp_file.hpp"
#include "detail/verification.hpp"

using namespace boost;
using namespace boost::iostreams;
using namespace boost::iostreams::test;
using std::ifstream;
using boost::unit_test::test_suite;   

void file_descriptor_test()
{

    typedef stream<file_descriptor_source> fdistream;
    typedef stream<file_descriptor_sink>   fdostream;
    typedef stream<file_descriptor>        fdstream;

    test_file  test1;       
    test_file  test2;       
                    
    //--------------Test file_descriptor_source-------------------------------//

    {
        fdistream  first(file_descriptor_source(test1.name()), 0);
        ifstream   second(test2.name().c_str());
        BOOST_CHECK_MESSAGE(
            compare_streams_in_chars(first, second),
            "failed reading from file_descriptor_source in chars with no buffer"
        );
    }

    {
        fdistream  first(file_descriptor_source(test1.name()), 0);
        ifstream   second(test2.name().c_str());
        BOOST_CHECK_MESSAGE(
            compare_streams_in_chunks(first, second),
            "failed reading from file_descriptor_source in chunks with no buffer"
        );
    }

    {
        file_descriptor_source  file(test1.name());
        fdistream               first(file);
        ifstream                second(test2.name().c_str());
        BOOST_CHECK_MESSAGE(
            compare_streams_in_chars(first, second),
            "failed reading from file_descriptor_source in chars with buffer"
        );
    }

    {
        file_descriptor_source  file(test1.name());
        fdistream               first(file);
        ifstream                second(test2.name().c_str());
        BOOST_CHECK_MESSAGE(
            compare_streams_in_chunks(first, second),
            "failed reading from file_descriptor_source in chunks with buffer"
        );
    }

    //--------------Test file_descriptor_sink---------------------------------//
                                                    
    {
        temp_file             temp;
        file_descriptor_sink  file(temp.name(), BOOST_IOS::trunc);
        fdostream             out(file, 0);
        write_data_in_chars(out);
        out.close();
        BOOST_CHECK_MESSAGE(
            compare_files(test1.name(), temp.name()),
            "failed writing to file_descriptor_sink in chars with no buffer"
        );
    }

    {
        temp_file             temp;
        file_descriptor_sink  file(temp.name(), BOOST_IOS::trunc);
        fdostream             out(file, 0);
        write_data_in_chunks(out);
        out.close();
        BOOST_CHECK_MESSAGE(
            compare_files(test1.name(), temp.name()),
            "failed writing to file_descriptor_sink in chunks with no buffer"
        );
    }

    {
        temp_file             temp;
        file_descriptor_sink  file(temp.name(), BOOST_IOS::trunc);
        fdostream             out(file);
        write_data_in_chars(out);
        out.close();
        BOOST_CHECK_MESSAGE(
            compare_files(test1.name(), temp.name()),
            "failed writing to file_descriptor_sink in chars with buffer"
        );
    }

    {
        temp_file             temp;
        file_descriptor_sink  file(temp.name(), BOOST_IOS::trunc);
        fdostream             out(file);
        write_data_in_chunks(out);
        out.close();
        BOOST_CHECK_MESSAGE(
            compare_files(test1.name(), temp.name()),
            "failed writing to file_descriptor_sink in chunks with buffer"
        );
    }

    //--------------Test file_descriptor--------------------------------------//

    {
        temp_file                  temp;
        file_descriptor            file( temp.name(),
                                         BOOST_IOS::in | 
                                         BOOST_IOS::out |
                                         BOOST_IOS::trunc | 
                                         BOOST_IOS::binary );
        fdstream                   io(file, BUFSIZ);
        BOOST_CHECK_MESSAGE(
            test_seekable_in_chars(io),
            "failed seeking within a file_descriptor, in chars"
        );
    }

    {
        temp_file                  temp;
        file_descriptor            file( temp.name(),
                                         BOOST_IOS::in | 
                                         BOOST_IOS::out |
                                         BOOST_IOS::trunc | 
                                         BOOST_IOS::binary );
        fdstream                   io(file, BUFSIZ);
        BOOST_CHECK_MESSAGE(
            test_seekable_in_chunks(io),
            "failed seeking within a file_descriptor, in chunks"
        );
    }
}

test_suite* init_unit_test_suite(int, char* []) 
{
    test_suite* test = BOOST_TEST_SUITE("file_descriptor test");
    test->add(BOOST_TEST_CASE(&file_descriptor_test));
    return test;
}
