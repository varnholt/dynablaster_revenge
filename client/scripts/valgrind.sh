#!/usr/bin/env bash
valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes --log-file="`date +%F`".log ./dynablaster
