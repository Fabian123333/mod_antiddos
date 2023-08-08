prog: mod_antiddos.so

mod_antiddos.so: mod_antiddos.c
	apxs -S CC=g++ -lm -c mod_antiddos.c -lhiredis -lstdc++
