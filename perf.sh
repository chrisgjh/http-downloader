#!/bin/bash
set -v
dd if=/dev/urandom of=test.dat  bs=10M count=1

time ./downloader test.txt 3 download > perf_test/test2.txt
time ./downloader test.txt 4 download > perf_test/test1.txt
time ./downloader test.txt 5 download > perf_test/test3.txt
time ./downloader test.txt 9 download > perf_test/test4.txt
time ./downloader test.txt 10 download > perf_test/test10.txt



time ./downloader small.txt 4 download > perf_test/small.txt
time ./downloader large.txt 4 download > perf_test/large.txt

diff -q -s bin/downloader downloader
