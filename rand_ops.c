#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <errno.h>
#include <assert.h>

#include "rand_ops.h"



// [0,n]
// This is the one the others are implemented in terms of
// because it is total: no error conditions, can access
// the full range of possible randomness, etc.
randval_t rand_zero_through_n(rand_ops_t ops, randval_t n) {
	assert(n >= 0);

	//Major optimization:
	randval_t wanted_bits;
	{
		randval_t unwanted_bits = ~0;
		randval_t n_tmp = n;
		while(n_tmp != 0) {
			n_tmp >>= 1;
			unwanted_bits <<= 1;
		}
		wanted_bits = ~unwanted_bits;
	}

	randval_t ret;
	do {
		ret = ops.rand_val() & wanted_bits;
	} while (ret > n);
	return ret;
}

// [0,n)
randval_t rand_zero_to_n(rand_ops_t ops, randval_t n) {
	assert(n > 0);
	return rand_zero_through_n(ops, n-1);
}

// [m,n]
randval_t rand_m_through_n(rand_ops_t ops, randval_t m, randval_t n) {
	assert(n >= m);
	return m + rand_zero_through_n(ops, n - m);
}

// [m,n)
randval_t rand_m_to_n(rand_ops_t ops, randval_t m, randval_t n) {
	assert(n > m);
	return m + rand_zero_through_n(ops, n - m - 1);
}



void libc_rand_init(void) {
	// 1-second precision is annoying
	// Even, run 'd' two times in one second and get the same results!
	// #include <time.h> ..... srand(time(NULL));
	struct timeval now;
	gettimeofday(&now, NULL);
	srand(now.tv_usec ^ now.tv_sec);
}

randval_t libc_rand_val(void) {
	return ((randval_t)rand()) + ((randval_t)rand()>>16)
		+ ((randval_t)rand()<<16) + ((randval_t)rand()<<31);
}

void libc_rand_deinit(void) {
}

const rand_ops_t libc_rand_ops = {
	.rand_init = libc_rand_init,
	.rand_deinit = libc_rand_deinit,
	.rand_val = libc_rand_val
};


// Use C std lib rather than, say, syscalls, because
// buffering /dev/urandom seems good when generating
// large numbers of random numbers, and doesn't cost
// much for small amounts.
static FILE* urandom_handle;
void urandom_rand_init(void) {
	urandom_handle = fopen("/dev/urandom", "rb");
	if(!urandom_handle) {
		fprintf(stderr, "Opening /dev/urandom failed!\n");
		exit(1);
	}
}

randval_t urandom_rand_val(void) {
	randval_t ret;
	fread(&ret, sizeof(ret), 1, urandom_handle);
	if(ret < 0) {
		//This interface demands nonnegative numbers.
		//Using ~ keeps it still evenly distributed.
		ret = ~ret;
	}
	return ret;
}

void urandom_rand_deinit(void) {
	fclose(urandom_handle);
}

const rand_ops_t urandom_rand_ops = {
	.rand_init = urandom_rand_init,
	.rand_deinit = urandom_rand_deinit,
	.rand_val = urandom_rand_val
};

