/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 * Copyright (C) 2012-2019 by UCSC Computational Genomics Lab
 *
 * Released under the MIT license, see LICENSE.txt
 */
#include "yomoBottomSegment.h"
#include "halDnaIterator.h"
#include "yomoGenome.h"
#include "yomoTopSegment.h"
#include <cstdlib>
#include <string>

using namespace std;
using namespace H5;
using namespace hal;

const size_t YomoBottomSegment::genomeIndexOffset = 0;
const size_t YomoBottomSegment::lengthOffset = sizeof(hal_index_t);
const size_t YomoBottomSegment::topIndexOffset = lengthOffset + sizeof(hal_size_t);
const size_t YomoBottomSegment::firstChildOffset = topIndexOffset + sizeof(hal_index_t);
const size_t YomoBottomSegment::totalSize(hal_size_t numChildren) {
    return firstChildOffset + numChildren * (sizeof(hal_index_t) + sizeof(bool));
}

YomoBottomSegment::YomoBottomSegment(YomoGenome *genome, YomoExternalArray *array, hal_index_t index)
    : BottomSegment(genome, index), _array(array) {
}

hal_size_t YomoBottomSegment::numChildrenFromDataType(const H5::DataType &dataType) {
    return (dataType.getSize() - firstChildOffset) / (sizeof(hal_index_t) + sizeof(bool));
}

hal_offset_t YomoBottomSegment::getTopParseOffset() const {
    assert(_index >= 0);
    hal_offset_t offset = 0;
    hal_index_t topIndex = getTopParseIndex();
    if (topIndex != NULL_INDEX) {
        YomoGenome *genome = dynamic_cast<YomoGenome *>(_genome);
        YomoTopSegment ts(genome, &genome->_topArray, topIndex);
        assert(ts.getStartPosition() <= getStartPosition());
        assert((hal_index_t)(ts.getStartPosition() + ts.getLength()) >= getStartPosition());
        offset = getStartPosition() - ts.getStartPosition();
    }
    return offset;
}

void YomoBottomSegment::setCoordinates(hal_index_t startPos, hal_size_t length) {
    assert(_index >= 0);
    if (_genome &&
        (startPos >= (hal_index_t)_genome->getSequenceLength() || startPos + length > _genome->getSequenceLength())) {
        throw hal_exception("Trying to set bottom segment coordinate out of range");
    }

    _array->setValue((hsize_t)_index, genomeIndexOffset, startPos);
    _array->setValue(_index + 1, genomeIndexOffset, startPos + length);
}

void YomoBottomSegment::print(std::ostream &os) const {
    os << "YOMO Bottom Segment";
}

// YOMO SPECIFIC
H5::CompType YomoBottomSegment::dataType(hal_size_t numChildren) {
    // the in-memory representations and yomo representations
    // don't necessarily have to be the same, but it simplifies
    // testing for now.
    assert(PredType::NATIVE_INT64.getSize() == sizeof(hal_index_t));
    assert(PredType::NATIVE_UINT64.getSize() == sizeof(hal_offset_t));
    assert(PredType::NATIVE_HSIZE.getSize() == sizeof(hal_size_t));
    assert(PredType::NATIVE_CHAR.getSize() == sizeof(bool));

    H5::CompType dataType(totalSize(numChildren));
    dataType.insertMember("genomeIdx", genomeIndexOffset, PredType::NATIVE_INT64);
    dataType.insertMember("length", lengthOffset, PredType::NATIVE_HSIZE);
    dataType.insertMember("topIdx", topIndexOffset, PredType::NATIVE_INT64);
    for (hsize_t i = 0; i < numChildren; ++i) {
        dataType.insertMember("childIdx" + std::to_string(i), firstChildOffset + i * (sizeof(hal_index_t) + sizeof(bool)),
                              PredType::NATIVE_INT64);
        dataType.insertMember("reverseFlag" + std::to_string(i),
                              firstChildOffset + i * (sizeof(hal_index_t) + sizeof(bool)) + sizeof(hal_index_t),
                              PredType::NATIVE_CHAR);
    }
    return dataType;
}
