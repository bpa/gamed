#ifndef SERVER_TEST_H__
#define SERVER_TEST_H__

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <cxxtest/TestSuite.h>
#include <gamed/server.h>

#define TEST_PORT 7483

class ServerTest: public CxxTest::TestSuite {
public:

	void setUp()    {};
	void tearDown() {};

	void test_server_welcome() {
		GamedServer server;
		server.config.listen(TEST_PORT);
		server.daemonize();
		int sock = open_socket(TEST_PORT);
	}

private:
	int open_socket(int port) {
		struct sockaddr_in sa;
		int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
		if (sock < 0) {
			TS_FAIL("Socket creation failed, something wrong with test");
		}
		socklen_t sa_len = sizeof(sa);
		memset(&sa,0,sa_len);
		sa.sin_family = AF_INET;
		sa.sin_port = htons(port);
		sa.sin_addr.s_addr = htonl(INADDR_ANY);

		if (connect(sock, (struct sockaddr*)&sa, sa_len) < 0) {
			TS_FAIL("Server not listening on test port");
		}
		return sock;
	}

};

#endif
