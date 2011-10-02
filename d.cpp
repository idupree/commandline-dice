//bash -c 'gcc -O2 -Wall d.cpp `pkg-config --cflags --libs glib-2.0` -o ../d' 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <glib.h>
#include <errno.h>

typedef long long int randval_t;

void rand_init(void) {
	// 1-second precision is annoying
	// Even, run 'd' two times in one second and get the same results!
	// #include <time.h> ..... srand(time(NULL));
	struct timeval now;
	gettimeofday(&now, NULL);
	srand(now.tv_usec ^ now.tv_sec);
}

randval_t rand_val(void) {
	return ((randval_t)rand()) + ((randval_t)rand()>>16)
		+ ((randval_t)rand()<<16) + ((randval_t)rand()<<31);
}

void pr(void* k, void* v, void* ud) {
	printf(":%i %i\n", *(int*)k, *(int*)v);
}

const char help_string[] =
"Usage: d sides [times[!]]\n"
"\t`d` rolls imaginary dice.\n"
"\n"
"\t`d 6` prints a number from 1 to 6 inclusive.\n"
"\t`d 6 10` prints ten numbers from 1 to 6 inclusive.\n"
"\t`d 6 4!` prints four non-repeating numbers from 1\n"
"\t\t\tto 6 inclusive.  For example,\n"
"\t\t\t`d 5 5!`\n"
"\t\t\tlets you choose a random order\n"
"\t\t\tfor five items you've lined up.\n"
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

	rand_init();

	//In case of overflow, atoll returns max (long)long. huh.
	randval_t sides = atoll(argv[1]);
	if(sides <= 0) {
		fprintf(stderr, "sides = %lli is too negative.\n", sides);
		exit(1);
	}
	int times = 1;
	bool factorial = false;
	if(argc >= 3) {
		char* nextc;
		errno = 0;
		times = strtol(argv[2], &nextc, 0);
		if(*nextc == '!') {
			factorial = true;
			if(nextc == argv[2]) {
				times = sides;
			}
			if(times > sides) {
				fprintf(stderr, "can't pigeonhole %i nums in %lli possibilities\n", times, sides);
				exit(1);
			}
		}
	}

	randval_t* results = (randval_t*)malloc(times * sizeof(randval_t));
	GHashTable* already = g_hash_table_new(&g_int_hash, &g_int_equal);

	for(int i = 0; i < times; ++i) {
		do {
			results[i] = rand_val() % sides + 1;
		} while(g_hash_table_lookup(already, &results[i]));

		if(factorial) {
			g_hash_table_insert(already, &results[i], &results[i]);
		}

		printf("%lld\n", results[i]);
	}

	g_hash_table_destroy(already);
	free(results);
}
