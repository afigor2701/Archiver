#include "archiver_class.h"

#include "extended_char.h"
#include "trie.h"
#include "file_stream.h"
#include "program_exception.h"

#include <iostream>

#include <algorithm>
#include <unordered_map>
#include <vector>
#include <string>

void Archiver::Increment(std::vector<bool>::iterator begin, std::vector<bool>::iterator end) const {
    for (auto it = end - 1; it >= begin; --it) {
        if (*it == false) {
            *it = true;
            break;
        } else {
            *it = false;
        }
    }
}

void Archiver::MakeCanonical(std::vector<std::pair<ExtendedChar, std::vector<bool>>>& all_codes) const {
    std::sort(all_codes.begin(), all_codes.end(),
              [](const std::pair<ExtendedChar, std::vector<bool>>& left,
                 const std::pair<ExtendedChar, std::vector<bool>>& right) {
                  if (left.second.size() == right.second.size()) {
                      return left.first < right.first;
                  }
                  return left.second.size() < right.second.size();
              });

    size_t prev_size = 0;
    size_t curr_ind = 0;
    for (auto& [exchar, code] : all_codes) {
        for (size_t j = 0; j < prev_size; ++j) {
            code[j] = all_codes[curr_ind - 1].second[j];
        }

        Increment(code.begin(), code.begin() + prev_size);

        for (size_t j = prev_size; j < code.size(); ++j) {
            code[j] = false;
        }

        ++curr_ind;
        prev_size = code.size();
    }
}

void Archiver::WriteHaffmanInformation(FileOstream& output,
                                       const std::vector<std::pair<ExtendedChar, std::vector<bool>>>& all_codes) const {
    output.PutExtendedChar(all_codes.size());
    for (size_t i = 0; i < all_codes.size(); ++i) {
        output.PutExtendedChar(all_codes[i].first);
    }

    std::vector<size_t> cnt_size(all_codes.back().second.size(), 0);
    for (size_t i = 0; i < all_codes.size(); ++i) {
        ++cnt_size[all_codes[i].second.size() - 1];
    }
    for (size_t i = 0; i < cnt_size.size(); ++i) {
        output.PutExtendedChar(cnt_size[i]);
    }
}

std::vector<std::pair<ExtendedChar, std::vector<bool>>> Archiver::RestoreCodes(FileIstream& input) const {
    size_t size_alphabet = input.GetExtendedChar();

    std::vector<std::pair<ExtendedChar, std::vector<bool>>> res(size_alphabet);
    for (size_t i = 0; i < size_alphabet; ++i) {
        res[i].first = input.GetExtendedChar();
    }

    for (size_t len = 1, cnt = 0; cnt < size_alphabet; ++len) {
        int64_t curr_cnt = input.GetExtendedChar();
        while (curr_cnt > 0) {
            if (cnt == 0) {
                // first code
                res[cnt].second = std::vector<bool>(len, false);
            } else {
                res[cnt].second = res[cnt - 1].second;
                Increment(res[cnt].second.begin(), res[cnt].second.end());
                while (res[cnt].second.size() < len) {
                    res[cnt].second.push_back(false);
                }
            }
            ++cnt;
            --curr_cnt;
        }
    }

    return res;
}

ExtendedChar Archiver::NextExtendedChar(FileIstream& input,
                                        const std::unordered_map<std::vector<bool>, ExtendedChar>& symbols) const {
    std::vector<bool> curr_code;
    auto it = symbols.begin();
    while ((it = symbols.find(curr_code)) == symbols.end()) {
        curr_code.push_back(input.GetBit());
    }
    return it->second;
}

std::vector<std::pair<ExtendedChar, std::vector<bool>>> Archiver::BuildHaffman(const File& file) const {
    FileIstream input(file.path);
    if (!input.IsOpen()) {
        throw ProgramException(std::string("Cannot open file: ") + file.path);
        // throw std::invalid_argument(std::string("Cannot open file: ") + file.path);
    }

    std::unordered_map<ExtendedChar, uint64_t> cnt_symbols;
    ++cnt_symbols[FILENAME_END];
    ++cnt_symbols[ONE_MORE_FILE];
    ++cnt_symbols[ARCHIVE_END];

    for (char c : file.name) {
        ++cnt_symbols[c];
    }

    unsigned char curr = 0;
    while (!input.IsEnd()) {
        curr = input.GetChar();
        ++cnt_symbols[curr];
    }

    Trie trie(cnt_symbols);
    auto all_codes = trie.AllCodes();
    MakeCanonical(all_codes);

    return all_codes;
}

Archiver::Archiver(Parser parser) : parser_(parser) {
}

void Archiver::Archive() const {
    FileOstream output(parser_.ArchivePath());
    if (!output.IsOpen()) {
        throw ProgramException(std::string("Cannot open file: ") + parser_.ArchivePath());
        // throw std::invalid_argument(std::string("Cannot open file: ") + parser_.ArchivePath());
    }

    size_t curr_file = 0;
    for (const auto& file : parser_.Files()) {
        auto all_codes = BuildHaffman(file);
        std::unordered_map<ExtendedChar, std::vector<bool>> codes;
        for (const auto& [ex, code] : all_codes) {
            codes[ex] = code;
        }

        WriteHaffmanInformation(output, all_codes);
        FileIstream input(file.path);
        if (!input.IsOpen()) {
            throw ProgramException(std::string("Cannot open file: ") + file.path);
            // throw std::invalid_argument(std::string("Cannot open file: ") + file.path);
        }

        for (char c : file.name) {
            output.PutBitSeq(codes[c]);
        }
        output.PutBitSeq(codes[FILENAME_END]);

        unsigned char curr = 0;
        while (!input.IsEnd()) {
            curr = input.GetChar();
            output.PutBitSeq(codes[curr]);
        }

        ++curr_file;
        if (curr_file == parser_.CountFiles()) {
            output.PutBitSeq(codes[ARCHIVE_END]);
        } else {
            output.PutBitSeq(codes[ONE_MORE_FILE]);
        }
    }
}

void Archiver::Unarchive() const {
    FileIstream input(parser_.ArchivePath());
    if (!input.IsOpen()) {
        throw ProgramException(std::string("Cannot open file: ") + parser_.ArchivePath());
        // throw std::invalid_argument(std::string("Cannot open file: ") + parser_.ArchivePath());
    }

    ExtendedChar curr = ARCHIVE_END;
    do {
        auto all_codes = RestoreCodes(input);
        std::unordered_map<std::vector<bool>, ExtendedChar> symbols;
        for (const auto& [ex, code] : all_codes) {
            symbols[code] = ex;
        }

        std::string file_name;
        curr = NextExtendedChar(input, symbols);
        while (curr != FILENAME_END) {
            file_name += static_cast<char>(curr);
            curr = NextExtendedChar(input, symbols);
        }

        FileOstream output(file_name);
        curr = NextExtendedChar(input, symbols);
        while (curr != ONE_MORE_FILE && curr != ARCHIVE_END) {
            output.PutChar(curr);
            curr = NextExtendedChar(input, symbols);
        }
    } while (curr != ARCHIVE_END);
}

void Archiver::Help() const {
    std::cout << "archiver -c archive_name file1 [file2 ...] - archive files file1, file2, ... and save the result to "
                 "archive_name.\n";
    std::cout << "archiver -d archive_name - unzip files from archive_name archive and put in current directory."
              << std::endl;
}

char Archiver::GetMode() const {
    return parser_.Mode();
}
