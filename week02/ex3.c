#include <stdio.h>

#define bool int
#define true 1
#define false 0
#define ERROR_MSG "cannot convert!"

bool is_valid_ns(unsigned short ns) {
	return 2 <= ns && ns <= 10;
}

bool is_in_given_ns(unsigned long long x, unsigned short ns) {
	while (x > 0) {
		if (x % 10 >= ns) {
			return false;
		}
		x /= 10;
	}
	return true;
}

unsigned long long convert_to_decimal(unsigned long long x, unsigned short ns) {
	unsigned int power = 1;
	unsigned long long decimal = 0;
	while (x > 0) {
		decimal += (x % 10) * power;
		power *= ns;
		x /= 10;
	}
	return decimal;
}

unsigned long long convert_from_decimal(unsigned long long x, unsigned short ns) {
	unsigned long long target = 0;
	unsigned long long power = 1;
	while (x > 0) {
		target += power * (x % ns);
		x /= ns;
		power *= 10;
	}
	return target;
}

void convert(unsigned long long x, unsigned short from, unsigned short to) {
	if (!is_valid_ns(from) || !is_valid_ns(to)) {
		printf(ERROR_MSG);
		return;
	}
	if (!is_in_given_ns(x, from)) {
		printf(ERROR_MSG);
		return;
	}
	unsigned long long decimal = convert_to_decimal(x, from);
	unsigned long long result = convert_from_decimal(decimal, to);
	printf("Result: %llu", result);
}

int main() {
	unsigned long long num;
	unsigned short from, to;

	printf("Please, input your number:\n");
	scanf("%lld", &num);
	printf("Please, input your number's initial numeric system:\n");
	scanf("%hd", &from);
	printf("Please, your number's target numeric system:\n");
	scanf("%hd", &to);

	convert(num, from, to);
}
