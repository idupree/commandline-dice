
#ifndef RAND_OPS_H__
#define RAND_OPS_H__

typedef long long int randval_t;

typedef struct rand_ops_t {
	void (*rand_init)(void);
	void (*rand_deinit)(void);
	randval_t (*rand_val)(void);
} rand_ops_t;

extern const rand_ops_t libc_rand_ops;
extern const rand_ops_t urandom_rand_ops;

#endif

