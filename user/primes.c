#include "kernel/types.h"
#include "user/user.h"

__attribute__((noreturn)) void primes(int curr) {
	int prime;
	read(curr, &prime ,sizeof(prime));
	printf("prime %d\n", prime);
	int p[2];
	int child = 0;
	int n;
	while(read(curr, &n, sizeof(n)) == sizeof(n)) {
		if (n % prime == 0) continue;
		if (child == 0) {
			pipe(p);
			if (fork() == 0) {
				close(p[1]);
				close(curr);
				primes(p[0]);
			}
			close(p[0]);
			child = 1;
		}
		write(p[1], &n, sizeof(n));
	}

	close(curr);
	if (child != 0) {
		close(p[1]);
		wait(0);
	}
	exit(0);
}

int main(int argc, char* argv[]) {
	int p[2];

	pipe(p);
	if (fork() == 0) {
		close(p[1]);
		primes(p[0]);
	}
	close(p[0]);
	for (int i = 2; i <= 280; i++) {
		write(p[1], &i, sizeof(i));
	}
	close(p[1]);
	wait(0);
	exit(0);
}
