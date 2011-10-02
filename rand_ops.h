
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

// Use these functions. For example, they guarantee even distribution
// in a way that "ops.rand_val() % 5" never will.

// [0,n]
randval_t rand_zero_through_n(rand_ops_t ops, randval_t n);

// [0,n)
randval_t rand_zero_to_n(rand_ops_t ops, randval_t n);

// [m,n]
randval_t rand_m_through_n(rand_ops_t ops, randval_t m, randval_t n);

// [m,n)
randval_t rand_m_to_n(rand_ops_t ops, randval_t m, randval_t n);

#endif

