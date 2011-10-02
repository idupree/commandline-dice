d: command-line dice
===================

'd' rolls dice from the Linux command-line.

To use:
------

<pre>
> d sides [[times][!]]
	`d` rolls imaginary dice.

	`d 6` prints a number from 1 to 6 inclusive.
	`d 6 10` prints ten numbers from 1 to 6 inclusive.
	`d 6 4!` prints four non-repeating numbers from 1
			to 6 inclusive.  For example,
			`d 5 5!`
			lets you choose a random order
			for five items you've lined up.
			For short:
	`d 5 !` arranges the numbers 1 to 5 in a random
			order, equivalent to `d 5 5!`.

	"sides" may be any number from 1 on up.
	"times" can't be negative, and if you want non-repeating.
			numbers then must be no greater than sides.
</pre>

If you put it on your PATH, you can just type 'd 6' anywhere
to roll a d6, or 'd 20' or even 'd 42'.

To build:
--------

If you don't have GLib development libraries, then install them. Then

    gcc -O2 -Wall -std=c99 *.c `pkg-config --cflags --libs glib-2.0` -o d

To test:
-------

    ./test

If things are horribly wrong it will tell you.
Also, you can look at all the output and see if it looks reasonable.

To be paranoid:
--------------

This uses Linux's /dev/urandom and an algorithm that should produce
uniformly distributed numbers that are as cryptographically strong as
/dev/urandom (which is: very, unless the system is in a low entropy
situation like a router that just booted up and had no entropy-file
stored, but human Linux users are quite fine).

However I didn't triple-check, so please tell me if I'm wrong!
