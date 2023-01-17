#pragma once

#include "parser_command_line.h"
#include "trie.h"
#include "file_stream.h"

#include <string>
#include <vector>
#include <utility>

class Archiver {
private:
    Parser parser_;

    void Increment(std::vector<bool>::iterator begin, std::vector<bool>::iterator end) const;
    void MakeCanonical(std::vector<std::pair<ExtendedChar, std::vector<bool>>>& all_codes) const;
    void WriteHaffmanInformation(FileOstream& output,
                                 const std::vector<std::pair<ExtendedChar, std::vector<bool>>>& all_codes) const;
    std::vector<std::pair<ExtendedChar, std::vector<bool>>> RestoreCodes(FileIstream& input) const;

    ExtendedChar NextExtendedChar(FileIstream& input,
                                  const std::unordered_map<std::vector<bool>, ExtendedChar>& symbols) const;

    std::vector<std::pair<ExtendedChar, std::vector<bool>>> BuildHaffman(const File& file) const;

public:
    explicit Archiver(Parser parser);

    void Archive() const;

    void Unarchive() const;

    void Help() const;

    char GetMode() const;
};
