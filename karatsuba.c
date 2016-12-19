#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#define NANO 1000000000L

void init_zeroes(char *, int); //initializes argument with 0's
char *mul(char *, char *); //assumes a is longer than b
char *nsquare_mul(char *, char *); //classic multiplication algorithm
char *karatsuba(char *, char *); //the karatsuba multiplication algorithm
char *strip_leading_zeroes(char *); //strips leading zeroes off the passed argument
char *pad_with_leading_zeroes(char *, int); //pads the argument with leading zeroes
char *slice(char *, int, int); //returns a substring of the passed argument
char *ten_pow(char *, int); //multiplies the given string with a power of 10
char *add(char *, char *); //adds two operands
char *subtract(char *, char *); //subtracts two operands
int nonzero(char *); //checks if the numerical value of a string is 0

int main()
{
	struct timespec start, stop;
	double ktime; //time taken by karatsuba
	double ntime; //time taken by nsquare
	char *a = NULL; //multiplicand
	char *b = NULL; //multiplier
	char *p = NULL; //karatsuba product
	char *n = NULL; //classic multiplication product
	int asize;
	int bsize;
	scanf("%d", &asize);
	scanf("%d", &bsize);
	a = malloc((asize + 1) * sizeof(char)); //extra 1 byte for sentinel null character
	b = malloc((bsize + 1) * sizeof(char));
	if((a == NULL) || (b == NULL))
	{
		printf("\nNot enough memory. Aborting process.\n");
		exit(1);
	}
	scanf("%s", a);
	scanf("%s", b);
	a[asize] = '\0';
	b[bsize] = '\0';
	clock_gettime(CLOCK_REALTIME, &start);
	p = karatsuba(a, b);
	clock_gettime(CLOCK_REALTIME, &stop);
	ktime = (stop.tv_sec - start.tv_sec) + (double)((stop.tv_nsec - start.tv_nsec) / NANO);
	clock_gettime(CLOCK_REALTIME, &start);
	n = nsquare_mul(a, b);
	clock_gettime(CLOCK_REALTIME, &stop);
	ntime = (stop.tv_sec - start.tv_sec) + (double)((stop.tv_nsec - start.tv_nsec) / NANO);
	printf("%s,%s,%lf,%lf\n", p, n, ktime, ntime);
	free(a);
	free(b);
	free(p);
	free(n);
	return 0;
}

int nonzero(char *string)
{
	int sum = 0;
	int length = strlen(string);
	for(int i = 0; i < length; ++i)
	{
		sum += (string[i] - '0');
	}
	return sum;
}

void init_zeroes(char *string, int length)
{
	for(int i = 0; i < length; ++i)
	{
		string[i] = '0';
	}
	string[length] = '\0';
}

char *mul(char *a, char *b)
{
	int alen = strlen(a);
	int blen = strlen(b);
	int carry;
	int digit_prod = 0;
	int newlen = alen + blen;
	char *result = malloc((newlen + 1) * sizeof(char));
	char *intermediate = malloc((newlen + 1) * sizeof(char));
	int ictrl = newlen - 1;
	init_zeroes(result, newlen);
	for(int i = blen - 1; i >= 0; --i, --ictrl)
	{
		int bdigit = b[i] - '0';
		int ipos = ictrl;
		carry = 0;
		init_zeroes(intermediate, newlen);
		for(int j = alen - 1; j >= 0; --j)
		{
			int adigit = a[j] - '0';
			digit_prod = (adigit * bdigit) + carry;
			carry = digit_prod / 10;
			digit_prod %= 10;
			intermediate[ipos] = '0' + digit_prod;
			ipos--;
		}
		intermediate[ipos] = carry + '0';
		char *res = result;
		result = add(res, intermediate);
		free(res);
	}
	free(intermediate);
	return result;
}

char *nsquare_mul(char *a, char *b)
{
	char *result = NULL;
	int alen = strlen(a);
	int blen = strlen(b);
	if (alen >= blen)
	{
		result = mul(a, b);
	}
	else
	{
		result = mul(b, a);
	}
	return result;
}

char *karatsuba(char *a, char *b)
{
	char *result = NULL;
	int alen = strlen(a);
	int blen = strlen(b);
	int max = (alen > blen) ? alen : blen;
	int half = max / 2;
	if((alen == 1) || (blen == 1))
	{
		result = nsquare_mul(a, b);
	}
	else
	{
		int mid = (max % 2 == 0) ? half : (half + 1);
		char *newa = pad_with_leading_zeroes(a, max);
		char *newb = pad_with_leading_zeroes(b, max);
		char *ia1 = slice(newa, 0, mid);
		char *ia2 = slice(newa, mid, max);
		char *ib1 = slice(newb, 0, mid);
		char *ib2 = slice(newb, mid, max);
		char *a1 = strip_leading_zeroes(ia1);
		char *a2 = strip_leading_zeroes(ia2);
		char *b1 = strip_leading_zeroes(ib1);
		char *b2 = strip_leading_zeroes(ib2);
		free(newa); free(newb); free(ia1); free(ia2); free(ib1); free(ib2);
		if(strlen(a1) == 0)
		{
			a1 = malloc(2 * sizeof(char));
			a1[0] = '0'; a1[1] = '\0';
		}
		if(strlen(a2) == 0)
		{
			a2 = malloc(2 * sizeof(char));
			a2[0] = '0'; a2[1] = '\0';
		}
		if(strlen(b1) == 0)
		{
			b1 = malloc(2 * sizeof(char));
			b1[0] = '0'; b1[1] = '\0';
		}
		if(strlen(b2) == 0)
		{
			b2 = malloc(2 * sizeof(char));
			b2[0] = '0'; b2[1] = '\0';
		}
		char *p1 = karatsuba(a1, b1);
		char *p2 = karatsuba(a2, b2);
		char *sum1 = add(a1, a2);
		char *sum2 = add(b1, b2);
		char *sum3 = add(p1, p2);
		char *p3 = karatsuba(sum1, sum2);
		char *diff1 = subtract(p3, sum3);
		char *pow1 = ten_pow(p1, 2 * half);
		char *pow2 = ten_pow(diff1, half);
		char *res1 = add(pow1, pow2);
		result = add(res1, p2);
		free(a1);
		free(a2);
		free(b1);
		free(b2);
		free(p1);
		free(p2);
		free(p3);
		free(sum1);
		free(sum2);
		free(sum3);
		free(diff1);
		free(pow1);
		free(pow2);
		free(res1);
	}
	return result;
}

char *strip_leading_zeroes(char *number)
{
	char *result = NULL;
	int length = strlen(number);
	int start_pos = 0;
	for(; number[start_pos] == '0'; ++start_pos);
	result = malloc((length - start_pos + 1) * sizeof(char));
	for(int i = start_pos; i < length; ++i)
	{
		result[i - start_pos] = number[i];
	}
	result[length - start_pos] = '\0';
	return result;
}

char *pad_with_leading_zeroes(char *number, int new_length)
{
	int old_len = strlen(number);
	char *nnew = malloc((new_length + 1) * sizeof(char));
	nnew[new_length] = '\0';
	if(nnew == NULL)
	{
		printf("\npad_with_leading_zeroes: Not enough Memory. Aborting.\n");
		exit(1);
	}
	for(int i = 0; i < new_length - old_len; ++i)
	{
		nnew[i] = '0';
	}
	for(int i = 0; i < old_len; ++i)
	{
		nnew[new_length - old_len + i] = number[i];
	}
	return nnew;
}

char *slice(char *parent, int lower, int upper)
{
	char *child = malloc((upper - lower + 1) * sizeof(char));
	if(child == NULL)
	{
		printf("\nslice: Not enough memory. Aborting.\n");
		exit(0);
	}
	for(int i = lower; i < upper; ++i)
	{
		child[i - lower] = parent[i];
	}
	child[upper - lower] = '\0';
	return child;
}

char *ten_pow(char *number, int pow)
{
	int length = strlen(number);
	char *result = malloc((length + pow + 1) * sizeof(char));
	for(int i = 0; i < length; ++i)
	{
		result[i] = number[i];
	}
	for(int i = length; i < length + pow; ++i)
	{
		result[i] = '0';
	}
	result[length + pow] = '\0';
	return result;
}

char *add(char *a, char *b)
{
	char *result = NULL;
	char *sum = NULL;
	int alen = strlen(a);
	int blen = strlen(b);
	int carry = 0;
	int max = (alen > blen) ? alen : blen;
	char *anew = pad_with_leading_zeroes(a, max);
	char *bnew = pad_with_leading_zeroes(b, max);
	result = malloc((max + 2) * sizeof(char));
	result[0] = '0';
	result[max + 1] = '\0';
	for(int i = max - 1; i >= 0; --i)
	{
		int digit_sum = (anew[i] - '0') + (bnew[i] - '0') + carry;
		carry = digit_sum / 10;
		digit_sum %= 10;
		result[i + 1] = '0' + digit_sum;
	}
	result[0] = '0' + carry;
	free(anew);
	free(bnew);
	if(nonzero(result))
	{
		sum = strip_leading_zeroes(result);
		free(result);
	}
	else
	{
		free(result);
		sum = malloc(2 * sizeof(char));
		sum[0] = '0'; sum[1] = '\0';
	}
	return sum;
}

char *subtract(char *a, char *b)
{
	char *result = NULL;
	char *difference = NULL;
	int alen = strlen(a);
	int blen = strlen(b);
	int max = (alen > blen) ? alen : blen;
	char *anew = pad_with_leading_zeroes(a, max);
	char *bnew = pad_with_leading_zeroes(b, max);
	result = malloc((max + 1) * sizeof(char));
	result[max] = '\0';
	for(int i = max - 1; i >= 0; --i)
	{
		int digit_diff = anew[i] - bnew[i];
		if(digit_diff < 0)
		{
			digit_diff += 10;
			int index = i - 1;
			while(index >= 0 && anew[index] == '0')
			{
				anew[index] = '9';
				index--;
			}
			anew[index] -= '1';
			anew[index] += '0';
		}
		result[i] = '0' + digit_diff;
	}
	free(anew);
	free(bnew);
	if(nonzero(result))
	{
		difference = strip_leading_zeroes(result);
		free(result);
	}
	else
	{
		free(result);
		difference = malloc(2 * sizeof(char));
		difference[0] = '0'; difference[1] = '\0';
	}
	return difference;
}
