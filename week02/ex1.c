#include <limits.h>
#include <float.h>
#include <stdio.h>

int main() {
	int i = INT_MAX;
	unsigned short us = USHRT_MAX;
	signed long int sli = LONG_MAX;
	float f = FLT_MAX;
	double d = DBL_MAX;
	printf("Size of int is %lu, max value is %d\n", sizeof i, i);
	printf("Size of unsigned short is %lu, max value is %hu\n", sizeof us, us);
	printf("Size of signed long int is %lu, max value is %ld\n", sizeof sli, sli);
	printf("Size of float is %lu, max value is %f\n", sizeof f, f);
	printf("Size of double is %lu, max value is %lf", sizeof d, d);
}
