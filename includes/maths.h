
/** \file maths.h
  * \brief Generic mathing routines
  */

#ifndef MATHS_H
#define MATHS_H

#include <stdint.h>

/** \brief Describes error codes passed by base36tobase10(). */
enum base36_error_codes
{
    BASE36_OK,              /**< Everything is OK */
    BASE36_INVALID_CHAR,    /**< An invalid character was encountered */
};

/** \brief Converts a string with a base 36 number to a base 10 number.
  * \param base36   A string containing a base 36 number
  * \param length   The length of the base 36 number
  * \return The base 10 number.
  *
  * Use of the length parameter allows you to extract a base 10 number from a
  * string in place, so no string copying is required.
  */
int64_t base36tobase10(char *base36, int length);

/** \brief Converts a base 10 integer to base36.
  * \param      base10  A base 10 integer
  * \param[out] buffer  A pointer to an allocated buffer to store the base36 
  *                     string.
  * \param      size    The length of the buffer.
  * \return 0 on success, nonzero if the buffer wasn't large enough or there
  *         was another problem.
  */
int base10tobase36(int64_t base10, char *buffer, int size);

/** \brief Performs a bitwise rotate right
  * \param number   The number to manipulate
  * \param count    The number of times to rotate right
  * \return Number rotated right "count" times
  */
uint64_t uint64_ror(uint64_t number, int count);

/** \brief Performs a modulo between two numbers
  * \param number   The number whose remainder you want
  * \param divisor  The number to divide by
  * \return The remainder of the division, unoptimized by the compiler
  */
int modulo(int number, int divisor);

#endif
