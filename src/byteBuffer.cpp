#include "byteBuffer.hpp"

namespace lemlib {

ByteBuffer::ByteBuffer(int mark, size_t pos, size_t lim, size_t cap, char* hb, size_t offset) {
    if (hb == nullptr) throw std::invalid_argument("Null pointer");
    this->_capacity = cap;
    limit(lim);
    position(pos);
    if (mark >= 0) {
        if (mark > (int)pos) throw std::invalid_argument("mark > position");
        this->_mark = mark;
    }

    this->hb = hb;
    this->offset = offset;
}

ByteBuffer::ByteBuffer(size_t cap, size_t lim) : ByteBuffer(-1, 0, lim, cap, new char[cap](), 0) { released = false; }

ByteBuffer::ByteBuffer(size_t bufsize, char* buf, size_t off, size_t len)
    : ByteBuffer(-1, off, off + len, bufsize, buf, 0) {}

ByteBuffer::~ByteBuffer() {
    if (!released) {
        delete[] hb;
        released = true;
    }
}

ByteBuffer::ByteBuffer(const ByteBuffer& that)
    : ByteBuffer(that._mark, that._position, that._limit, that._capacity, new char[that._capacity](), that.offset) {
    memcpy(hb, that.hb, that._capacity);
    released = false;
}

size_t ByteBuffer::nextGetIndex() {
    if (_position >= _limit) throw std::overflow_error("");
    return _position++;
}

size_t ByteBuffer::nextGetIndex(size_t nb) {
    if (_position >= _limit) throw std::overflow_error("");
    size_t p = _position;
    _position += nb;
    return p;
}

size_t ByteBuffer::nextPutIndex() {
    if (_position >= _limit) throw std::overflow_error("");
    return _position++;
}

size_t ByteBuffer::nextPutIndex(size_t nb) {
    if (_limit - _position < nb) throw std::overflow_error("");
    size_t p = _position;
    _position += nb;
    return p;
}

size_t ByteBuffer::checkIndex(size_t i) {
    if (i >= _limit) throw std::out_of_range("");
    return i;
}

size_t ByteBuffer::checkIndex(size_t i, size_t nb) {
    if (nb > _limit - i) throw std::out_of_range("");
    return i;
}

int ByteBuffer::markValue() { return _mark; }

void ByteBuffer::discardMark() { _mark = -1; }

size_t ByteBuffer::ix(size_t i) {
    return i; // no offset
}

void ByteBuffer::checkBounds(int off, int len, int size) {
    if ((off | len | (off + len) | (size - (off + len))) < 0) throw std::out_of_range("");
}

// PUBLIC METHOD

ByteBuffer ByteBuffer::allocate(size_t capacity) { return ByteBuffer(capacity, capacity); }

ByteBuffer ByteBuffer::wrap(size_t arrsize, char* array) { return wrap(arrsize, array, 0, arrsize); }

ByteBuffer ByteBuffer::wrap(size_t arrsize, char* array, size_t offset, size_t length) {
    return ByteBuffer(arrsize, array, offset, length);
}

char* ByteBuffer::array() { return hb; }

size_t ByteBuffer::arrayOffset() { return offset; }

ByteBuffer& ByteBuffer::compact() {
    memcpy(hb, &(hb[ix(position())]), remaining());
    position(remaining());
    limit(capacity());
    discardMark();
    return *this;
}

size_t ByteBuffer::capacity() { return _capacity; }

int ByteBuffer::compareTo(ByteBuffer& that) {
    size_t n = this->position() + std::min(this->remaining(), that.remaining());
    for (size_t i = this->position(), j = that.position(); i < n; i++, j++) {
        int cmp = this->get(i) - that.get(j);
        if (cmp != 0) return cmp;
    }
    return this->remaining() - that.remaining();
}

ByteBuffer& ByteBuffer::clear() {
    _position = 0;
    _limit = _capacity;
    _mark = -1;
    return *this;
}

ByteBuffer& ByteBuffer::flip() {
    _limit = _position;
    _position = 0;
    _mark = -1;
    return *this;
}

bool ByteBuffer::equals(ByteBuffer& that) {
    if (this->remaining() != that.remaining()) return false;
    int p = this->position();
    for (int i = this->limit() - 1, j = that.limit() - 1; i >= p; i--, j--)
        if (this->get(i) != that.get(j)) return false;
    return true;
}

char ByteBuffer::get() { return hb[ix(nextGetIndex())]; }

char ByteBuffer::get(size_t idx) { return hb[ix(checkIndex(idx))]; }

ByteBuffer& ByteBuffer::get(char* dst, size_t offset, size_t length) {
    checkBounds(offset, length, offset + length); // HACK dst.length

    if (length > remaining()) throw std::underflow_error("");
    // int end = offset + length;
    // for (int i = offset; i < end; i++)
    //	dst[i] = get(); // HACK
    memcpy(&(dst[offset]), &(hb[ix(position())]), length);
    nextGetIndex(length);
    return *this;
}

ByteBuffer& ByteBuffer::get(char* dst, size_t length) { return get(dst, 0, length); }

std::string ByteBuffer::getNTBS(size_t maxSize) {
    std::string rtn = "";
    for (size_t i = 0; i < maxSize; i++) {
        char now = get();
        if (now == (char)0x00) break;
        rtn += now;
    }
    return rtn;
}

bool ByteBuffer::hasRemaining() { return _position < _limit; }

ByteBuffer& ByteBuffer::put(char value) {
    hb[ix(nextPutIndex())] = value;
    return *this;
}

ByteBuffer& ByteBuffer::put(ByteBuffer& src) {
    if (&src == this) throw std::invalid_argument("");
    size_t n = src.remaining();
    if (n > remaining()) throw std::overflow_error("");
    for (size_t i = 0; i < n; i++) put(src.get());
    return *this;
}

ByteBuffer& ByteBuffer::put(size_t index, char value) {
    hb[ix(checkIndex(index))] = value;
    return *this;
}

ByteBuffer& ByteBuffer::put(char* src, size_t length) { return put(src, 0, length); }

ByteBuffer& ByteBuffer::put(char* src, size_t offset, size_t length) {
    checkBounds(offset, length, offset + length); // HACK dst.length
    if (length > remaining()) throw std::overflow_error("");
    // int end = offset + length;
    // for (int i = offset; i < end; i++)
    //	put(src[i]); // HACK
    memcpy(&(hb[ix(position())]), &(src[offset]), length);
    nextPutIndex(length);

    return *this;
}

ByteBuffer& ByteBuffer::putNTBS(const char* str) { return putNTBS(std::string(str)); }

ByteBuffer& ByteBuffer::putNTBS(std::string str) {
    return put(const_cast<char*>(str.data()), str.size()).put((char)0x00);
}

size_t ByteBuffer::limit() { return _limit; }

ByteBuffer& ByteBuffer::limit(size_t newLimit) {
    if (newLimit > _capacity) throw std::invalid_argument("");
    _limit = newLimit;
    if (_position > _limit) _position = _limit;
    if (_mark > (int)_limit) // HACK
        _mark = -1;
    return *this;
}

ByteBuffer& ByteBuffer::mark() {
    _mark = _position;
    return *this;
}

char* ByteBuffer::output() {
    size_t length = remaining();
    char* dst = new char[length]();
    memcpy(dst, &hb[_position], length);
    return dst;
}

size_t ByteBuffer::position() { return _position; }

ByteBuffer& ByteBuffer::position(size_t newPosition) {
    if (newPosition > _limit) throw std::invalid_argument("");
    _position = newPosition;
    if (_mark > (int)_position) // XXX
        _mark = -1;
    return *this;
}

size_t ByteBuffer::remaining() { return _limit - _position; }

ByteBuffer& ByteBuffer::reset() {
    int m = _mark;
    if (m < 0) throw std::invalid_argument("");
    _position = m;
    return *this;
}

ByteBuffer& ByteBuffer::rewind() {
    _position = 0;
    _mark = -1;
    return *this;
}

char& ByteBuffer::operator[](size_t idx) { return hb[idx]; }

bool ByteBuffer::operator==(const ByteBuffer& rhs) { return equals(const_cast<ByteBuffer&>(rhs)); }

} // namespace lemlib