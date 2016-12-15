#!/usr/bin/env bash

n=$1

for s in b w r d c; do
	./gen_cnf $n > board_$n.cnf
	command time -f "elapsed time %e" ./test $s board_$n.cnf board_$n.sol
	./verify board_$n.cnf board_$n.sol
done
