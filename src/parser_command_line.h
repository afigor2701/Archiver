#pragma once

#include <vector>
#include <string>

struct File {
    std::string name;
    std::string path;
};

class Parser {
private:
    char mode_;
    File archive_;
    std::vector<File> files_;

public:
    Parser(int argc, char** argv);

    std::vector<File> Files() const;
    std::string ArchiveName() const;
    std::string ArchivePath() const;
    size_t CountFiles() const;
    char Mode() const;
};
