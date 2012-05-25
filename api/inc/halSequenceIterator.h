/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */

#ifndef _HALSEQUENCEITERATOR_H
#define _HALSEQUENCEITERATOR_H

#include "halDefs.h"
#include "halSequence.h"

namespace hal {

/** 
 * Interface for top segment iterator exposes the top segment
 * interface and some new methods for jumping around the genome.  
 * Always hidden in smart pointers in the public interface. 
 */
class SequenceIterator
{
public:
   virtual SequenceIteratorPtr copy() = 0;
   virtual SequenceIteratorConstPtr copy() const = 0;
   virtual void toNext() const = 0;
   virtual void toPrev() const = 0;
   virtual Sequence* getSequence() = 0;
   virtual const Sequence* getSequence() const = 0;
   virtual bool equals(SequenceIteratorConstPtr p2) const = 0;

protected:
   friend class counted_ptr<SequenceIterator>;
   friend class counted_ptr<const SequenceIterator>;
   virtual ~SequenceIterator() = 0;
};

inline SequenceIterator::~SequenceIterator() {}

inline bool operator==(SequenceIteratorConstPtr p1,
                       SequenceIteratorConstPtr p2) 
{
  if (p1.get() == NULL || p2.get() == NULL)
  {
    return p1.get() == NULL && p2.get() == NULL;
  }
  return p1->equals(p2);
}

inline bool operator!=(SequenceIteratorConstPtr p1,
                       SequenceIteratorConstPtr p2)
{
  return !(p1 == p2);
}

}
#endif