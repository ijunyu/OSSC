/* base64.h -- Encode binary data using printable characters.
   Copyright (C) 2004-2006, 2009-2012 Free Software Foundation, Inc.
   Written by Simon Josefsson.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.  */

#ifndef BASE64_H
# define BASE64_H
#include "../ossc-config.h"

/* Get unsigned int. */
# include <stddef.h>

/* Get bool. */
#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#endif

/* Get stderr */
#include <stdio.h>

#if !defined(ASSERT)
#define ASSERT(expr) \
  do                                                                         \
    {                                                                        \
      if (!(expr))                                                           \
        {                                                                    \
          fprintf (ASSERT_STREAM, "%s:%d: assertion failed\n",               \
                   __FILE__, __LINE__);                                      \
          fflush (ASSERT_STREAM);                                            \
          abort ();                                                          \
        }                                                                    \
    }                                                                        \
  while (0)

#ifndef ASSERT_STREAM
# define ASSERT_STREAM stderr
#endif

#endif

# ifdef __cplusplus
extern "C" {
# endif

/* This uses that the expression (n+(k-1))/k means the smallest
   integer >= n/k, i.e., the ceiling of n/k.  */
# define BASE64_LENGTH(inlen) ((((inlen) + 2) / 3) * 4)

struct base64_decode_context
{
  unsigned int i;
  char buf[4];
};

extern bool isbase64 (char ch);

extern void base64_encode (const char *in, unsigned int inlen,
                           char *out, unsigned int outlen);

extern unsigned int base64_encode_alloc (const char *in, unsigned int inlen, char **out);

extern void base64_decode_ctx_init (struct base64_decode_context *ctx);

extern bool base64_decode_ctx (struct base64_decode_context *ctx,
                               const char *in, unsigned int inlen,
                               char *out, unsigned int *outlen);

extern bool base64_decode_alloc_ctx (struct base64_decode_context *ctx,
                                     const char *in, unsigned int inlen,
                                     char **out, unsigned int *outlen);

#define base64_decode(in, inlen, out, outlen) \
        base64_decode_ctx (NULL, in, inlen, out, outlen)

#define base64_decode_alloc(in, inlen, out, outlen) \
        base64_decode_alloc_ctx (NULL, in, inlen, out, outlen)

# ifdef __cplusplus
}
# endif

#endif /* BASE64_H */
