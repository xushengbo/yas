
// Copyright (c) 2010-2023 niXman (github dot nixman at pm dot me). All
// rights reserved.
//
// This file is part of YAS(https://github.com/niXman/yas) project.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#ifndef __yas__tests__base__include__boost_cont_deque_hpp
#define __yas__tests__base__include__boost_cont_deque_hpp

/***************************************************************************/

template<typename archive_traits>
bool boost_cont_deque_test(std::ostream &log, const char* archive_type, const char *test_name) {
#if defined(YAS_SERIALIZE_BOOST_TYPES)
	boost::container::deque<int> deq1, deq2;
	deq1.push_back(0);
	deq1.push_back(1);
	deq1.push_back(2);
	deq1.push_back(3);
	deq1.push_back(4);
	deq1.push_back(5);
	deq1.push_back(6);
	deq1.push_back(7);
	deq1.push_back(8);
	deq1.push_back(9);

	typename archive_traits::oarchive oa;
	archive_traits::ocreate(oa, archive_type);
	oa & YAS_OBJECT_NVP("obj", ("deq", deq1));

	typename archive_traits::iarchive ia;
	archive_traits::icreate(ia, oa, archive_type);
	ia & YAS_OBJECT_NVP("obj", ("deq", deq2));

	if ( deq1 != deq2 ) {
		YAS_TEST_REPORT(log, archive_type, test_name);
		return false;
	}

	boost::container::deque<std::string> deq3, deq4;
	deq3.push_back("1");
	deq3.push_back("2");
	deq3.push_back("3");

	typename archive_traits::oarchive oa2;
	archive_traits::ocreate(oa2, archive_type);
	oa2 & YAS_OBJECT_NVP("obj", ("deq", deq3));

	typename archive_traits::iarchive ia2;
	archive_traits::icreate(ia2, oa2, archive_type);
	ia2 & YAS_OBJECT_NVP("obj", ("deq", deq4));

	if ( deq3 != deq4 ) {
		YAS_TEST_REPORT(log, archive_type, test_name);
		return false;
	}

#endif // defined(YAS_SERIALIZE_BOOST_TYPES)
	return true;
}

/***************************************************************************/

#endif // __yas__tests__base__include__boost_cont_deque_hpp
