#ifndef SERVER_TEST_H__
#define 

class ServerTest: public CxxTest::TestSuite {
public:

	void setUp()    {};
	void tearDown() {};

	void test_server_welcome() {
		run_command("../gamed");
		int sock = open_socket(GAMED_PORT);
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

	void run_command(char *command) {
	}
};

#endif
