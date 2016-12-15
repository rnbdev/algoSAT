Algorithms for SAT
==================

This is a small academic project for my course work.

I have implemented the following SAT solving algorithms from D. E. Knuth's book.
* Backtrack
* Backtrack using watchee
* Cyclic DPLL
* Clause learning
* Random walk

To run them, just compile,

    g++ test.cc -o test

Then execute it,

    ./test S [INPUTCNF [OUTPUTFILE]]

INPUTCNF is the CNF file and solution will be flushed to OUTPUTFILE. No value means `stdin` and `stdout` respectively.

To run them all, fire,

    ./runall.sh BOARDSIZE

This will run all the algorithms and verify if the output solution is correct.

To generate CNF of extended N-queen problem, complie and run,

    g++ gen_cnf.cc -o gen_cnf
    ./gen_cnf BOARDSIZE

`gen_cnf` will flush the output to `stdout`.

Ranadeep