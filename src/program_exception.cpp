#include "program_exception.h"

#include <string>

ProgramException::ProgramException() {
}

ProgramException::ProgramException(const char* what) : what_(what) {
}

ProgramException::ProgramException(const std::string& what) : what_(what) {
}

const char* ProgramException::what() const noexcept {
    return what_.c_str();
}

ProgramException::~ProgramException() {
}
