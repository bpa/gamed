#define GCRYPT_NO_MPI_MACROS 1
#define GCRYPT_NO_DEPRECATED 1
#include <gcrypt.h>
#include <glib.h>
#include <stdio.h>

void setup() {
	if (!gcry_check_version (GCRYPT_VERSION)) {
		fputs ("libgcrypt version mismatch\n", stderr);
		exit (2);
	}

	gcry_control (GCRYCTL_DISABLE_SECMEM, 0);
	gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);
}

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

unsigned char *unframe_09(const unsign char *frame) {
}

unsigned char *frame_09(const unsign char *data) {
}
