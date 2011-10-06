#define GCRYPT_NO_MPI_MACROS 1
#define GCRYPT_NO_DEPRECATED 1
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <event2/event.h>
#include <gcrypt.h>
#include <glib.h>
#include <stdio.h>
#include "client.h"

int listening_socket;
ClientList clients;
EventQueue queue;
struct event_base *event_base;

int setup() {
	if (!gcry_check_version (GCRYPT_VERSION)) {
		fputs ("libgcrypt version mismatch\n", stderr);
		exit (2);
	}

	gcry_control (GCRYCTL_DISABLE_SECMEM, 0);
	gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);
	event_base = event_base_new();
	LIST_INIT(&clients);
	TAILQ_INIT(&queue);
	return 0; //NativeCall can't handle void return
}

int listen_on_port(int port) {
    int listener;
    struct sockaddr_in sa;
    socklen_t sa_len;
    listener = socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
    if (listener < 0) {
        perror("socket");
        exit(1);
    }
    sa_len = sizeof(sa);
    bzero(&sa, sa_len);
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    fprintf(stderr, "Binding to port %i\n", port);
    if (bind(listener, (struct sockaddr*)&sa, sa_len) < 0) {
        perror("Unable to bind");
        exit(1);
    }
    if (listen(listener,5) < 0) {
        perror("Listen failed: ");
        exit(1);
    }
	listening_socket = listener;
	return 0;
}

Event *next_event() {
	Event *ev;
	if (queue.tqh_first == NULL)
		event_base_loop(event_base, EVLOOP_ONCE);
	}
	ev = queue.tqh_first;
	TAILQ_REMOVE(&queue, queue.tqh_first, events);
	return ev;
}

Client *c_event_client(Event *event) { return event->client; }
unsigned char *c_event_message(Event *event) { return event->message; }
int c_event_type(Event *event) { return event->type; }
int c_event_free(Event *event) { x }

unsigned char *sha1sum(const unsigned char *data) {
	int size = gcry_md_get_algo_dlen(GCRY_MD_SHA1);
	char *sum = malloc(size+1);
	gcry_md_hash_buffer(GCRY_MD_SHA1, sum, data, strnlen(data, 1024));
	sum[size] = '\0';
	return sum;
}

unsigned char *base64_encode(const unsigned char *data) {
	return g_base64_encode(data, strnlen(data, 1024));
}

unsigned char *sha1_base64(const unsigned char *data) {
	unsigned char *base64 = NULL;
	unsigned char *sum = sha1sum(data);
	base64 = base64_encode(sum);
	free(sum);
	return base64;
}

unsigned char *encode_v8(const unsigned char *data) {
}

void bin(unsigned char data) {
	int max = 10;
	unsigned char mask = 0x80;
	while (mask) {
		printf(mask & data ? "1" : "0");
		mask >>= 1;
	}
	printf(" ");
}

unsigned char *decode_v8(const unsigned char *data) {
	int i;
	for (i=0;i<16;i++) {
		bin(data[i]);
		if ((i+1) % 4 == 0) printf("\n");
	}
	printf("Data: %i\n", data[1] & 0x80);
	printf("len: %i\n", data[1] & 0x7F);
	return &data[6];
}


