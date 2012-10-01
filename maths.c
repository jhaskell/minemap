#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "maths.h"

int base36_errno;

int64_t base36tobase10(char *base36, int length)
{
    int i;
    int64_t digit = 0, base10 = 0;
    char c, is_negative = 0;

    base36_errno = BASE36_OK;

    for (i = 0; i < length; i++)
    {
        base10 *= 36;
        c = *(base36 + i);

        if (c == 0)
            break;
        else if (i == 0 && c == '-')
            is_negative = 1;
        else if (c >= '0' && c <= '9')
            base10 += c - 48;
        else if (c >= 'A' && c <= 'Z')
            base10 += c - 55;
        else if (c >= 'a' && c <= 'z')
            base10 += c - 87;
        else
        {
            base36_errno = BASE36_INVALID_CHAR;
            return 0;
        }
    }

    base36_errno = BASE36_OK;
    return is_negative ? (-1) * base10 : base10;
}

int base10tobase36(int64_t base10, char *buffer, int size)
{
    int length = 0;
    int remainder, is_negative = 0;

    if (size < 1)
    {
        return (-1);
    }

    if (base10 < 0)
    {
        is_negative = 1;
        base10 = abs(base10);
        length++;
    }

    if (base10 == 0)
        length = 1;
    else
        length += floor(log(base10) / log(36)) + 1;

    if (length + 1 > size)
    {
        return (-1);
    }

    memset(buffer, 0, length + 1);
    if (is_negative)
        *buffer = '-';

    for (;;)
    {
        length--;

        remainder = base10 % 36;

        if (remainder < 10)
            buffer[length] = '0' + remainder;
        else
            buffer[length] = 'a' + remainder - 10;

        base10 = (int64_t)floor(base10 / 36);

        if (base10 < 1)
            break;

    }

    return 0;
}

uint64_t uint64_ror(uint64_t number, int count)
{
    return (number >> count) | (number << (64 - count));
}

int modulo(int number, int divisor)
{
    ldiv_t division = ldiv(number, divisor);
    return division.rem;
}
