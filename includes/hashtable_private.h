/* Copyright (C) 2002, 2004 Christopher Clark <firstname.lastname@cl.cam.ac.uk> */

/** \file hashtable_private.h
  * Private functions for a simple hash table.
  * \author Christopher Clark \<christopher.clark@cl.cam.ac.uk\>
  */

#ifndef __HASHTABLE_PRIVATE_CWC22_H__
#define __HASHTABLE_PRIVATE_CWC22_H__

#include "hashtable.h"

/*****************************************************************************/
/** \brief A hashtable bucket.  */
struct entry
{
    void *k;            /**< \brief The key of this entry */
    void *v;            /**< \brief The value of this entry */
    unsigned int h;     /**< \brief The hash value of this bucket */
    struct entry *next; /**< \brief The next value in the bucket */
};

/** \brief A hashtable. */
struct hashtable {
    unsigned int tablelength;           /**< \brief The size of the table */
    struct entry **table;               /**< \brief The buckets of the table */
    unsigned int entrycount;            /**< \brief How many entries are stored 
                                          *         in the table */
    unsigned int loadlimit;             /**< \brief The load limit of the table
                                          */
    unsigned int primeindex;            /**< \brief The prime index of the 
                                          *         table */
    unsigned int (*hashfn) (void *k);   /**< \brief Function to use to generate
                                          *         a hash for this table */
    int (*eqfn) (void *k1, void *k2);   /**< \brief Function to use to compare 
                                          *         keys */
    void (*value_free_func)(void*);     /**< \brief Function to free values */
};

/*****************************************************************************/
/** \brief Produces a hash for a given key */
unsigned int
hash(struct hashtable *h, void *k);

/*****************************************************************************/
/* indexFor */
/** \brief Returns an index for a given hash value  */
//static inline unsigned int
unsigned int indexFor(unsigned int tablelength, unsigned int hashvalue) ;

/* Only works if tablelength == 2^N */
/*static inline unsigned int
indexFor(unsigned int tablelength, unsigned int hashvalue)
{
    return (hashvalue & (tablelength - 1u));
}
*/

/*****************************************************************************/
/** \brief Macro to use for freeing keys */
#define freekey(X) free(X)
/*define freekey(X) ; */


/*****************************************************************************/

#endif /* __HASHTABLE_PRIVATE_CWC22_H__*/

/*
 * Copyright (c) 2002, Christopher Clark
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * * Neither the name of the original author; nor the names of any contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 * 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
