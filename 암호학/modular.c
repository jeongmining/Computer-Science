
#include "modular.h"

int gcd_recursion(int a, int b)
{
    if (b == 0)
    {
        return a;
    }
    return gcd_recursion(b, a % b);
}

int gcd_loop(int a, int b)
{
    while (b > 0)
    {
        int tmp = a;
        a = b;
        b = tmp % b;
    }
    return a;
}

int extended_gcd_recursion(int a, int b, int *x, int *y)
{
    if (b == 0)
    {
        *x = 1;
        *y = 0;
        return a;
    }

    int x1, y1;
    int gcd = extended_gcd_recursion(b, a % b, &x1, &y1);
    *x = y1;
    *y = x1 - (a / b) * y1;
    return gcd;
}

int extended_gcd_loop(int a, int b, int *x, int *y)
{
    int x0 = 1, y0 = 0, x1 = 0, y1 = 1;
    while (b > 0)
    {
        int p = a / b;
        int tmp = a % b;
        a = b;
        b = tmp;

        tmp = x0 - p * x1;
        x0 = x1;
        x1 = tmp;

        tmp = y0 - p * y1;
        y0 = y1;
        y1 = tmp;
    }
    *x = x0;
    *y = y0;

    return a;
}

int mul_inv(int a, int m)
{
    int d0 = a, d1 = m;
    int x0 = 1, x1 = 0;
    while (d1 > 1)
    {
        int p = d0 / d1;
        int tmp = d0 % d1;
        d0 = d1;
        d1 = tmp;

        tmp = x0 - p * x1;
        x0 = x1;
        x1 = tmp;
    }

    if (d1 == 1)
        return x1 > 0 ? x1 : x1 + m;
    else
        return 0;
}

uint16_t multiply(uint16_t x)
{
    return x << 1 ^ ((x >> 15) & 1 ? 0b101011 : 0);
}

uint16_t gf16_mul(uint16_t a, uint16_t b)
{
    uint16_t result = 0;
    while (b > 0)
    {
        if (b & 1)
        {
            result = result ^ a;
        }
        b >>= 1;
        a = multiply(a);
    }
    return result;
}

uint16_t gf16_pow(uint16_t a, uint16_t b)
{
    uint16_t result = 1;
    while (b > 0)
    {
        if (b & 1)
        {
            result = gf16_mul(result, a);
        }
        b >>= 1;
        a = gf16_mul(a, a);
    }
    return result;
}

uint16_t gf16_inv(uint16_t x)
{
    return gf16_pow(x, 0xfffe);
}