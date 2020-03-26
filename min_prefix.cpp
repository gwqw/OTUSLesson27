#include "min_prefix.h"

#include <string_view>

#include "str_utils.h"

using namespace std;

// mappers
Yamr::StrList copy_task(std::string line) {
    return {move(line)};
}

Yamr::StrList first_letter_task(const std::string& line) {
    return {string{line[0]}};
}

Yamr::StrList NLetter::operator()(const std::string& line) const {
    return {line.substr(0, min(letter_count_, line.size()))};
}

Yamr::StrList get_first_word(std::string line) {
    return {line.substr(0, line.find(' '))};
}
Yamr::StrList get_all_words(std::string line) {
    string_view data{line};
    Yamr::StrList res;
    for (string_view word = read_token(data, " ");
         !word.empty();
         word = read_token(data, " ")
        )
    {
        res.emplace_back(word);
    }
    return res;
}

Yamr::StrList all_prefixes(std::string line) {
    if (line.empty()) return {};
    Yamr::StrList res(line.size());
    for (size_t i = 0; i+1 < line.size(); ++i) {
        res[i] = line.substr(0, i+1);
    }
    res.back() = move(line);
    return res;
}


// reducers
Yamr::StrList CalcDuplicates::operator()(std::string line) {
    if (line == prev_) {
        ++counter_;
    } else {
        if (!prev_.empty()) {
            res_.push_back(std::to_string(counter_) + ' ' + prev_);
        }
        counter_ = 1;
        prev_ = move(line);
    }
    auto res = res_;
    res.push_back(std::to_string(counter_) + ' ' + prev_);
    return res;
}

Yamr::StrList GetMinUniquePrefixSize::operator()(std::string line) {
    if (line == prev_) {
        ++counter_;
    } else {
        counter_ = 1;
        prev_ = std::move(line);
    }
    max_size_ = max(max_size_, prev_.size());

    if (counter_ != 1) {
        if (prev_.size() > max_duplicate_size_) {
            max_duplicate_size_ = prev_.size();
        }
    }
    return {to_string(min(max_duplicate_size_ + 1, max_size_))};
}

Yamr::StrList GetMinVirtualUniquePrefixSize::operator()(std::string line) {
    if (line == prev_) {
        ++counter_;
    } else {
        counter_ = 1;
        prev_ = std::move(line);
    }

    if (counter_ != 1) {
        if (prev_.size() > max_duplicate_size_) {
            max_duplicate_size_ = prev_.size();
        }
    }
    return {to_string(max_duplicate_size_ + 1)};
}

Yamr::StrList GetMaxDuplicatePrefix::operator()(std::string line) {
    if (line == prev_) {
        ++counter_;
    } else {
        counter_ = 1;
        prev_ = std::move(line);
    }
    max_size_ = max(max_size_, prev_.size());

    if (counter_ != 1) {
        if (prev_.size() > max_duplicate_size_) {
            max_duplicate_size_ = prev_.size();
            max_duplicate_prefix_ = prev_;
        }
    }
    return {to_string(max_duplicate_size_) + ' ' + max_duplicate_prefix_};
}


