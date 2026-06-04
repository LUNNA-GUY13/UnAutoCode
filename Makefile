CC = gcc
CFLAGS = -O3 -Wall -Wextra -std=c11
TEST_CFLAGS = $(CFLAGS) -DTEST_BUILD

all: unautocode

unautocode: Unautocode.c
	$(CC) $(CFLAGS) -o unautocode Unautocode.c
clean:
	rm -f unautocode test_runner unautocode.exe test_runner.exe
