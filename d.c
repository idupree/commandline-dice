//bash -c 'gcc -O2 -Wall -std=c99 *.c `pkg-config --cflags --libs glib-2.0` -o ../d'

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <glib.h>
#include <errno.h>

#include "rand_ops.h"

void errif(bool cond, const char* fmt, ...) {
	if(cond) {
		va_list args;
		va_start(args, fmt);
		vfprintf(stderr, fmt, args);
		va_end(args);
		exit(1);
	}
}



const char help_string[] =
"Usage: d sides [[times][!]]\n"
"\t`d` rolls imaginary dice.\n"
"\n"
"\t`d 6` prints a number from 1 to 6 inclusive.\n"
"\t`d 6 10` prints ten numbers from 1 to 6 inclusive.\n"
"\t`d 6 4!` prints four non-repeating numbers from 1\n"
"\t\t\tto 6 inclusive.  For example,\n"
"\t\t\t`d 5 5!`\n"
"\t\t\tlets you choose a random order\n"
"\t\t\tfor five items you've lined up.\n"
"\t\t\tFor short:\n"
"\t`d 5 !` arranges the numbers 1 to 5 in a random\n"
"\t\t\torder, equivalent to `d 5 5!`.\n"
"\n"
"\t\"sides\" may be any number from 1 on up.\n"
"\t\"times\" can't be negative, and if you want non-repeating.\n"
"\t\t\tnumbers then must be no greater than sides.\n"
;

int main(int argc, char** argv)
{
	if(argc <= 1) {
		fprintf(stderr, help_string);
		exit(1);
	}
	if(strcmp(argv[1], "--help") == 0 ||
	   strcmp(argv[1], "--version") == 0) {
		fprintf(stdout, help_string);
		exit(0);
	}

	errif(argc >= 4, "Too many arguments! Two at most: `d sides [times]`\n");

	rand_ops_t rand_ops = urandom_rand_ops;

	rand_ops.rand_init();

	randval_t sides;
	{
		const char* sides_arg = argv[1];
		char* nextc;
		errno = 0;
		sides = strtoll(sides_arg, &nextc, 10);
		errif(nextc == sides_arg || sides <= 0, "Sides ('%s'?) must be a positive number.\n", sides_arg);
		errif(errno == ERANGE && sides == LLONG_MAX, "Implementation limit: sides ('%s') must be no greater than %lli (*pout*)\n", sides_arg, LLONG_MAX);
		errif(*nextc != '\0', "Sides ('%s') must contain only an integer number.\n", sides_arg);
	}
	randval_t times = 1;
	bool factorial = false;
	if(argc >= 3) {
		const char* times_arg = argv[2];
		char* nextc;
		errno = 0;
		times = strtoll(times_arg, &nextc, 10);
		errif(times < 0, "Can't take an action less than zero (%lli) times!\n", times);
		errif(errno == ERANGE && times == LLONG_MAX, "Implementation limit: times ('%s') must be no greater than %lli (*pout*)\n", times_arg, LLONG_MAX);
		if(*nextc == '!') {
			factorial = true;
			if(nextc == times_arg) {
				times = sides;
			}
			++nextc;
			errif(times > sides, "Can't pigeonhole %i nums in %lli possibilities.\n", times, sides);
		}
		if(times == 0) {
			// This is allowed, but helpfully point out why
			// the user is getting zero output.
			fprintf(stderr, "(You roll the die zero times.)\n");
		}
		errif(nextc == times_arg || *nextc != '\0', "Times ('%s'?) must be a non-negative integer and/or \"!\".\n", times_arg);
	}

	randval_t* results = (randval_t*)malloc(times * sizeof(randval_t));
	errif(!results, "Failed to allocate %lli results!\n", times);
	GHashTable* already = g_hash_table_new(&g_int64_hash, &g_int64_equal);

	for(randval_t i = 0; i < times; ++i) {
		do {
			results[i] = rand_ops.rand_val() % sides + 1;
		} while(g_hash_table_lookup(already, &results[i]));

		if(factorial) {
			g_hash_table_insert(already, &results[i], &results[i]);
		}

		printf("%lld\n", results[i]);
	}

	g_hash_table_destroy(already);
	free(results);

	rand_ops.rand_deinit();
}
