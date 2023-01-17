#include "manager.h"

#include "archiver_class.h"
#include "extended_char.h"
#include "parser_command_line.h"
#include "program_exception.h"

#include <iostream>

int Manager(int argc, char** argv) {
    try {
        Parser parser(argc, argv);

        Archiver archiver(parser);
        if (archiver.GetMode() == 'c') {
            archiver.Archive();
        } else if (archiver.GetMode() == 'd') {
            archiver.Unarchive();
        } else {
            archiver.Help();
        }
    } catch (ProgramException& error) {
        std::cout << error.what() << std::endl;
        return 111;
    } catch (...) {
        std::cout << "Something went wrong..." << std::endl;
        return 111;
    }
    return 0;
}
