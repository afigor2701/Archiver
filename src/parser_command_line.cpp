#include "parser_command_line.h"
#include "program_exception.h"

#include <cstring>
#include <vector>
#include <string>
#include <filesystem>
#include <iostream>

Parser::Parser(int argc, char** argv) {
    if (argc < 2) {
        throw ProgramException("Fewer arguments than expected. See 'archiver -h'");
    }
    if ((std::strcmp(argv[1], "-c") == 0 || std::strcmp(argv[1], "-d") == 0) && argc >= 3) {
        if (std::strcmp(argv[1], "-c") == 0 && argc < 4) {
            throw ProgramException("Fewer arguments than expected. See 'archiver -h'");
        }

        mode_ = argv[1][1];

        std::filesystem::path archive_path = argv[2];
        archive_.name = archive_path.filename();
        archive_.path = std::string(archive_path);

        for (size_t i = 3; i < argc; ++i) {
            std::filesystem::path path = argv[i];
            files_.push_back({path.filename(), std::string(argv[i])});
        }
    } else if (strcmp(argv[1], "-h") == 0) {
        mode_ = argv[1][1];
    } else {
        throw ProgramException("Wrong command. See 'archiver -h'");
    }
}

std::vector<File> Parser::Files() const {
    return files_;
}

std::string Parser::ArchiveName() const {
    return archive_.name;
}

std::string Parser::ArchivePath() const {
    return archive_.path;
}

size_t Parser::CountFiles() const {
    return files_.size();
}

char Parser::Mode() const {
    return mode_;
}
