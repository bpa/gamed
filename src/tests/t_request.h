#ifndef REQUEST_TEST_H__
#define REQUEST_TEST_H__

#include <gamed/request.h>
using namespace Gamed;

class RequestTest: public CxxTest::TestSuite {
public:

	void setUp()    {};
	void tearDown() {};

	void test_create() {
        JoinRequest r;
    }

};

#endif
