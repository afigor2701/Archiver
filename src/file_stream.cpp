#include "file_stream.h"

#include "extended_char.h"

#include <string>
#include <vector>

void FileIstream::ReadBuffer() {
    input_.read(buffer_, BUFFER_MAX_SIZE);
    buffer_size_ = input_.gcount();
    curr_byte_ = 0;
    curr_bit_ = 0;
}

bool FileIstream::GetBit() {
    bool res = (static_cast<unsigned char>(buffer_[curr_byte_]) >> (7 - curr_bit_)) & 1;
    ++curr_bit_;

    if (curr_bit_ == 8) {
        ++curr_byte_;
        curr_bit_ = 0;
        if (curr_byte_ == buffer_size_) {
            ReadBuffer();
        }
    }
    return res;
}

FileIstream::FileIstream(const std::string& file_name) : input_(file_name, std::ios_base::in | std::ios::binary) {
    for (size_t i = 0; i <= BUFFER_MAX_SIZE; ++i) {
        buffer_[i] = 0;
    }
    ReadBuffer();
}

bool FileIstream::IsOpen() const {
    return input_.is_open();
}

bool FileIstream::IsEnd() const {
    return input_.eof() && (curr_byte_ == buffer_size_);
}

char FileIstream::GetChar() {
    char symbol = 0;
    for (int64_t i = 7; i >= 0; --i) {
        symbol = static_cast<char>(symbol + (1 << i) * GetBit());
    }
    return symbol;
}

ExtendedChar FileIstream::GetExtendedChar() {
    ExtendedChar symbol = 0;
    for (int64_t i = CNT_EXTENDED_CHAR_BITS - 1; i >= 0; --i) {
        symbol += (1 << i) * GetBit();
    }
    return symbol;
}

FileIstream::~FileIstream() {
    input_.close();
}

void FileOstream::ClearBuffer() {
    for (size_t i = 0; i <= BUFFER_MAX_SIZE; ++i) {
        buffer_[i] = 0;
    }
}

void FileOstream::WriteBuffer() {
    output_.write(buffer_, curr_byte_ + (curr_bit_ > 0));
    ClearBuffer();
    curr_bit_ = 0;
    curr_byte_ = 0;
}

void FileOstream::PutBit(bool bit) {
    buffer_[curr_byte_] = static_cast<char>(buffer_[curr_byte_] + (1 << (7 - curr_bit_)) * bit);
    ++curr_bit_;
    if (curr_bit_ == 8) {
        ++curr_byte_;
        curr_bit_ = 0;
        if (curr_byte_ == BUFFER_MAX_SIZE) {
            WriteBuffer();
        }
    }
}

FileOstream::FileOstream(const std::string& file_name) : output_(file_name, std::ios_base::out | std::ios::binary) {
    ClearBuffer();
}

bool FileOstream::IsOpen() const {
    return output_.is_open();
}

void FileOstream::PutChar(char c) {
    for (int64_t i = 7; i >= 0; --i) {
        PutBit((c >> i) & 1);
    }
}

void FileOstream::PutExtendedChar(ExtendedChar c) {
    for (int64_t i = CNT_EXTENDED_CHAR_BITS - 1; i >= 0; --i) {
        PutBit((c >> i) & 1);
    }
}

void FileOstream::PutBitSeq(const std::vector<bool>& bits) {
    for (size_t i = 0; i < bits.size(); ++i) {
        PutBit(bits[i]);
    }
}

FileOstream::~FileOstream() {
    WriteBuffer();
    output_.close();
}
