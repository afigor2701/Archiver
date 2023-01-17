#pragma once

#include "extended_char.h"

#include <string>
#include <vector>
#include <fstream>

class FileIstream {
private:
    inline static const size_t BUFFER_MAX_SIZE = 9 * 8;

    char buffer_[BUFFER_MAX_SIZE + 1];
    size_t buffer_size_ = 0;
    size_t curr_byte_ = 0;
    size_t curr_bit_ = 0;
    std::fstream input_;

    void ReadBuffer();

public:
    explicit FileIstream(const std::string& file_name);

    bool IsOpen() const;
    bool IsEnd() const;

    bool GetBit();
    char GetChar();
    ExtendedChar GetExtendedChar();

    ~FileIstream();
};

class FileOstream {
private:
    inline static const size_t BUFFER_MAX_SIZE = 9 * 8;

    char buffer_[BUFFER_MAX_SIZE + 1];
    size_t curr_byte_ = 0;
    size_t curr_bit_ = 0;
    std::fstream output_;

    void ClearBuffer();
    void WriteBuffer();
    void PutBit(bool bit);

public:
    explicit FileOstream(const std::string& file_name);

    bool IsOpen() const;

    void PutChar(char c);
    void PutExtendedChar(ExtendedChar c);
    void PutBitSeq(const std::vector<bool>& bits);

    ~FileOstream();
};
