/** \file utf8.h
  * \brief Implementation of UTF-8 charset encoding (RFC3629).
  * \author Alexey Vatchenko 
  * 
  * Copyright (c) 2007 Alexey Vatchenko <av\@bsdua.org>
  * Released under the BSD License.
  */
/*
 * Copyright (c) 2007 Alexey Vatchenko <av@bsdua.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * utf8: implementation of UTF-8 charset encoding (RFC3629).
 */
#ifndef _UTF8_H_
#define _UTF8_H_

#include <sys/types.h>

#include <wchar.h>

/** \defgroup utf8flags UTF-8 function flags
  */
/*@{*/
/** \brief Set flag to ignore all errors. */
#define UTF8_IGNORE_ERROR		0x01
/** \brief Set flag to skip processing the Byte Order Mark. */
#define UTF8_SKIP_BOM			0x02
/*@}*/

/** \brief Converts a bytestream of utf8 data to wchar_t
  * \param[in]  in      The utf8 bytestream
  * \param      insize  The size of the utf8 bytestream
  * \param[out] out     A wchar_t buffer to write to
  * \param      outsize The size of the wchar_t buffer
  * \param      flags   Optional flags to control (see \ref utf8flags utf8flags)
  * \return The number of characters written to out. Returns 0 on error.
  */
size_t		utf8_to_wchar(const char *in, size_t insize, wchar_t *out,
		    size_t outsize, int flags);

/** \brief Converts a wchar_t buffer to a utf8 bytestream.
  * \param[in]  in      The wchar_t buffer to convert
  * \param      insize  The size of the wchar_t buffer
  * \param[out] out     A utf-8 bytestream buffer
  * \param      outsize The size of the utf-8 bytestream buffer
  * \param      flags   Optional flags to control (see \ref utf8flags utf8flags)
  * \return The size of the result buffer in bytes. 0 on error.
  */
size_t		wchar_to_utf8(const wchar_t *in, size_t insize, char *out,
		    size_t outsize, int flags);

#endif /* !_UTF8_H_ */
