#pragma once

#include <exception>
#include <string>

class ProgramException : std::exception {
private:
    std::string what_;
public:
    ProgramException();
    explicit ProgramException(const char* what);
    explicit ProgramException(const std::string& what);

    const char* what() const noexcept override;

    ~ProgramException();
};
