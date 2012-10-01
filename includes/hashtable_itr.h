/* Copyright (C) 2002, 2004 Christopher Clark <firstname.lastname@cl.cam.ac.uk> */

/** \file hashtable_itr.h
  * \brief Iterator functions for hashtables
  * \author Christopher Clark \<christopher.clark@cl.cam.ac.uk\>
  */
#ifndef __HASHTABLE_ITR_CWC22__
#define __HASHTABLE_ITR_CWC22__
#include "hashtable.h"
#include "hashtable_private.h" /* needed to enable inlining */

/*****************************************************************************/
/* This struct is only concrete here to allow the inlining of two of the
 * accessor functions. */
/** \brief An iterator for a hash table. */
struct hashtable_itr
{
    struct hashtable *h;    /**< \brief The hash table to iterate over */
    struct entry *e;        /**< \brief The current hash entry. */
    struct entry *parent;   /**< \brief The parent of the current hash entry. */
    unsigned int index;     /**< \brief The numeric index of the current entry.
                              */
};


/*****************************************************************************/
/* hashtable_iterator
 */

/** \brief Generates an iterator for a given hash
  * \param h    A hash table to iterate over
  * \return A hashtable_itr corresponding to the hash passed
  */

struct hashtable_itr *
hashtable_iterator(struct hashtable *h);

/**
 * \brief return the value of the (key,value) pair at the current position 
 * \param i The iterator to use
 * \return The key/value pair at the current position
 */

extern inline void *
hashtable_iterator_key(struct hashtable_itr *i)
{
    return i->e->k;
}

/**
  * \brief return the value of the (key,value) pair at the current position 
  * \param i The iterator to use
  */

extern inline void *
hashtable_iterator_value(struct hashtable_itr *i)
{
    return i->e->v;
}

/**
  * \brief advance the iterator to the next element 
  * \param itr The iterator to use
  * \return zero if advanced to end of table 
  */

int
hashtable_iterator_advance(struct hashtable_itr *itr);

/**
  * \brief remove current element and advance the iterator to the next element
  *
  * \return zero if advanced to end of table 
  * \param itr The iterator to use
  * \bug if you need the value to free it, read it before removing. ie: beware memory leaks!
  */

int
hashtable_iterator_remove(struct hashtable_itr *itr);

/**
  * \brief overwrite the supplied iterator, to point to the entry matching the supplied key.
  * \param itr The iterator to use
  * \param h points to the hashtable to be searched.
  * \param k The key to rewrite the value to
  * \return zero if not found
  */
int
hashtable_iterator_search(struct hashtable_itr *itr,
                          struct hashtable *h, void *k);


#endif /* __HASHTABLE_ITR_CWC22__*/

/*
 * Copyright (c) 2002, 2004, Christopher Clark
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
