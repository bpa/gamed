#ifndef SERVER_TEST_H__
#define SERVER_TEST_H__

#include <cxxtest/TestSuite.h>
#include <gamed/server.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>

class ServerTest: public CxxTest::TestSuite {
public:

	void setUp()    {};
	void tearDown() {};

	void test_server_welcome() {
		int pid = fork();
		if (pid) {
			struct timespec req, rem;
			req.tv_sec = 0;
			req.tv_nsec = 1000000;
			nanosleep(&req,&rem);
			int sock = open_socket(GAMED_PORT);
			close(sock);
			sock = open_socket(GAMED_PORT);
			close(sock);
			kill(pid,SIGINT);
			wait(NULL);
		}
		else {
            Gamed::Server server;
			server.run();
			exit(0);
		}
	}

private:
	int open_socket(int port) {
		struct sockaddr_in sa;
		int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
		if (sock < 0) {
			perror("socket");
			TS_FAIL("Socket creation failed, something wrong with test");
		}
		socklen_t sa_len = sizeof(sa);
		memset(&sa,0,sa_len);
		sa.sin_family = AF_INET;
		sa.sin_port = htons(port);
		sa.sin_addr.s_addr = htonl(INADDR_ANY);

		if (connect(sock, (struct sockaddr*)&sa, sa_len) < 0) {
			perror("connect");
			TS_FAIL("Server not listening on test port");
		}
		return sock;
	}

	void run_command(char *command, char *argv[]=NULL) {
		if (fork()) {
			execve(command, argv, NULL);
		}
	}
};

#endif
