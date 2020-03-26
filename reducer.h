#pragma once

#include <vector>
#include <stdexcept>
#include <future>

#include "mapper.h"

namespace Yamr {

void saveStrListToFile(const StrList &lst, const std::string &fname);

class Reducer {
    friend struct TestReducer;
public:
    Reducer(StrList string_list, std::size_t thread_count)
            : thread_count_(thread_count)
    {
        if (thread_count_ == 0) {
            throw std::invalid_argument("threads count can't be zero");
        }
        splitted_lists_ = split(string_list, thread_count_);
    }
    Reducer(const std::string& filename, std::size_t thread_count)
            : thread_count_(thread_count)
    {
        if (thread_count_ == 0) {
            throw std::invalid_argument("threads count can't be zero");
        }
        if (!std::filesystem::exists(filename)) {
            throw std::invalid_argument("Cannot find file " + filename + " for reducing");
        }
        splitted_lists_ = split(filename, thread_count_);
    }

    template<typename Func>
    void run(Func func, const std::string &fname_prefix);

private:
    std::size_t thread_count_ = 0;
    std::vector<StrList> splitted_lists_;

    static std::vector<StrList> split(StrList &string_list, std::size_t thread_count);
    static std::vector<StrList> split(const std::string& filename, std::size_t thread_count);

    template<typename Func>
    static StrList thread_work(StrList &str_list, Func func);
};

template<typename Func>
void Reducer::run(Func func, const std::string &fname_prefix) {
    std::vector<std::future<StrList>> futures;
    futures.reserve(thread_count_);
    for (std::size_t i = 0; i < thread_count_; ++i) {
        futures.emplace_back(
            std::async(
                std::launch::async,
                thread_work<Func>,
                ref(splitted_lists_[i]),
                func
            )
        );
    }
    std::size_t i = 0;
    for (auto &task : futures) {
        saveStrListToFile(task.get(), fname_prefix + std::to_string(i++));
    }
}

template<typename Func>
StrList Reducer::thread_work(StrList &str_list, Func func) {
    StrList res;

    // obtain only last result
    for (auto &line : str_list) {
        res = func(std::move(line));
    }

    return res;
}

struct TestReducer {
    static std::vector<StrList> split(StrList &string_list, std::size_t thread_count) {
        return Reducer::split(string_list, thread_count);
    }
};

}