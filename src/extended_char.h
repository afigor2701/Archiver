#pragma once

#include <cstdint>
#include <cstdlib>

using ExtendedChar = int16_t;

const ExtendedChar FILENAME_END = 256;
const ExtendedChar ONE_MORE_FILE = 257;
const ExtendedChar ARCHIVE_END = 258;
const size_t CNT_EXTENDED_CHAR_BITS = 9;
