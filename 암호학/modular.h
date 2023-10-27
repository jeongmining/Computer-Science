#pragma once
#include <stdint.h>

int gcd_recursion(int a, int b);
int gcd_loop(int a, int b);
int extended_gcd_recursion(int a, int b, int *x, int *y);
int extended_gcd_loop(int a, int b, int *x, int *y);
int mul_inv(int a, int m);
uint16_t multiply(uint16_t x);
uint16_t gf16_mul(uint16_t a, uint16_t b);
uint16_t gf16_pow(uint16_t a, uint16_t b);
uint16_t gf16_inv(uint16_t x);
