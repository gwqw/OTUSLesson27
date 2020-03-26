#pragma once

#include <string>

#include "mapper.h"

/** mapper func. objects
 * interface:
 * Yamr::StrList (*) (std::string)
*/

/// returns line copy
Yamr::StrList copy_task(std::string line);

/// returns line first letter
Yamr::StrList first_letter_task(std::string line);

/// returns n first letters from line
struct NLetter {
    explicit NLetter(std::size_t letter_count) : letter_count_(letter_count) {}

    Yamr::StrList operator()(const std::string& line) const;

    std::size_t letter_count_ = 1;
};

/// returns first word in line
Yamr::StrList get_first_word(std::string line);

/// returns list of all words in line
Yamr::StrList get_all_words(std::string line);

/// returns list of all prefixes of line
Yamr::StrList all_prefixes(std::string line);


/** reducer func. objects
 * interface:
 * Yamr::StrList (*)(std::string)
*/

/// calculates duplicates
/// outputs found duplicates for every line
struct CalcDuplicates {
    Yamr::StrList operator()(std::string line);

    std::string prev_;
    std::size_t counter_ = 0;
    Yamr::StrList res_;
};

/// get for every line min unique prefix size
/// returns max duplicate prefix size if all input lines are equal
struct GetMinUniquePrefixSize {
    Yamr::StrList operator()(std::string line);

    std::string prev_;
    std::size_t counter_ = 0;

    std::size_t max_duplicate_size_ = 0;
    std::size_t max_size_ = 0;
};

/// get for every line min unique prefix size
/// returns max duplicate prefix size+1 if all input lines are equal
struct GetMinVirtualUniquePrefixSize {
    Yamr::StrList operator()(std::string line);

    std::string prev_;
    std::size_t counter_ = 0;

    std::size_t max_duplicate_size_ = 0;
};

/// get for every line max duplicate prefix with its size
struct GetMaxDuplicatePrefix {
    Yamr::StrList operator()(std::string line);

    std::string prev_;
    std::size_t counter_ = 0;

    std::size_t max_duplicate_size_ = 0;
    std::string max_duplicate_prefix_;
    std::size_t max_size_ = 0;
};

