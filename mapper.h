#pragma once

#include <string>
#include <utility>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <future>

namespace Yamr {

using StrList = std::vector<std::string>;

class Mapper {

    struct SplitInfo {
        std::istream::pos_type start = 0;
        std::istream::pos_type end = 0;
    };

    friend struct TestMapper;

public:
    Mapper(std::string fname, std::size_t thread_count)
            : fname_(std::move(fname)), thread_count_(thread_count) {
        if (thread_count_ == 0) {
            throw std::invalid_argument("threads count can't be zero");
        }
        if (!std::filesystem::exists(fname_)) {
            throw std::invalid_argument("Cannot find file " + fname_ + " for mapping");
        }

        split_info_ = split(fname_, thread_count_);
    }

    template<typename Func>
    StrList run(Func func);

private:
    std::string fname_;
    std::size_t thread_count_ = 0;
    std::vector<SplitInfo> split_info_;

    static std::vector<SplitInfo> split(const std::string &fname, std::size_t thread_count);

    static std::vector<SplitInfo> split(std::istream &file, std::size_t thread_count,
                                        std::size_t stream_size);

    template<typename Func>
    static StrList thread_work(const std::string &fname,
                               const SplitInfo &split_info, Func func);

    static StrList mergeLists(std::vector<StrList> &lists);

    static void saveResults(const std::vector<StrList> &lists);
};

/*implementation*/
template<typename Func>
StrList Mapper::run(Func func) {
    std::vector<std::future<StrList>> futures;
    futures.reserve(thread_count_);
    // mapping
    for (std::size_t i = 0; i < thread_count_; ++i) {
        futures.emplace_back(
            std::async(
                std::launch::async,
                thread_work<Func>,
                std::ref(fname_),
                std::ref(split_info_[i]),
                func
             )
        );
    }
    // obtain results
    std::vector<StrList> res_list;
    res_list.reserve(thread_count_);
    for (auto &task : futures) {
        res_list.push_back(task.get());
    }

    // save results to disk
    saveResults(res_list);
    // merge results
    return mergeLists(res_list);
}

template<typename Func>
StrList Mapper::thread_work(const std::string &fname,
                            const Mapper::SplitInfo &split_info, Func func) {
    StrList res;
    std::ifstream f(fname);
    f.seekg(split_info.start);
    for (std::string s; std::getline(f, s); ) {
        auto out_list = func(std::move(s));
        for (auto &line : out_list) {
            res.push_back(move(line));
        }
        if (f.tellg() >= split_info.end) break;
    }
    std::sort(res.begin(), res.end());
    return res;
}

struct TestMapper {
    static std::vector<Mapper::SplitInfo> split(std::istream &file, std::size_t thread_count,
                                                std::size_t stream_size) {
        return Mapper::split(file, thread_count, stream_size);
    }

    static StrList mergeLists(std::vector<StrList> &lists) {
        return Mapper::mergeLists(lists);
    }
};

}