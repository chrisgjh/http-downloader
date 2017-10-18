#!/bin/bash
set -v
dd if=/dev/urandom of=test.dat  bs=10M count=1

time ./downloader test.txt 4 download > perf_test/downloader1.txt
time ./bin/downloader test.txt 4 download > perf_test/downloader2.txt
time ./downloader test.txt 8 download > perf_test/downloader8.txt
time ./bin/downloader test.txt 8 download > perf_test/downloader9.txt
time ./downloader test.txt 12 download > perf_test/downloader13.txt
time ./bin/downloader test.txt 12 download > perf_test/downloader12.txt
time ./downloader test.txt 20 download > perf_test/downloader21.txt
time ./bin/downloader test.txt 20 download > perf_test/downloader22.txt
time ./downloader test.txt 30 download > perf_test/downloader31.txt
time ./bin/downloader test.txt 30 download > perf_test/downloader32.txt

time ./downloader small.txt 4 download > perf_test/small1.txt
time ./bin/downloader small.txt 4 download > perf_test/small2.txt

time ./downloader large.txt 4 download > perf_test/large2.txt
time ./bin/downloader large.txt 4 download > perf_test/large1.txt

diff -q -s bin/downloader downloader
