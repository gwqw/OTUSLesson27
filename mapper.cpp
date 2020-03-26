#include "mapper.h"

#include <optional>

#include "file_utils.h"
#include "iterator_range.h"

using namespace std;

namespace Yamr {

std::vector<Mapper::SplitInfo> Mapper::split(const std::string &fname, std::size_t thread_count) {
    auto size = get_file_size(fname);
    if (size == 0) {
        throw invalid_argument("Empty input file");
    }
    ifstream f(fname);
    return split(f, thread_count, size);
}

vector<Mapper::SplitInfo> Mapper::split(std::istream &file, std::size_t thread_count,
                                        std::size_t stream_size) {
    vector<SplitInfo> res(thread_count);
    size_t chunk_size = max(stream_size / thread_count, size_t(1));
    for (size_t i = 1; i < thread_count; ++i) {
        res[i - 1].end = findEol(file, i * chunk_size);
        res[i].start = res[i - 1].end;
    }
    res.back().end = stream_size;
    return res;
}


StrList Mapper::mergeLists(vector<StrList> &lists) {
    using Iterator = StrList::iterator;
    StrList res;

    // init
    vector<IteratorRange<Iterator>> iterators;
    for (auto &list : lists) {
        iterators.emplace_back(list.begin(), list.end());
        res.reserve(res.capacity() + list.size());
    }

    // merge cycle
    while (!isAllEmpty(iterators)) {
        optional<size_t> min_it_idx = nullopt;
        string_view min_value;

        // find list with min string
        for (size_t i = 0; i < iterators.size(); ++i) {
            if (iterators[i].empty()) continue;
            if (!min_it_idx || min_value > *(iterators[i].begin())) {
                min_it_idx = i;
                min_value = *(iterators[i].begin());
            }
        }

        // add it(min_str) to res and move forward min_it
        res.push_back(
            move(
                *(iterators[*min_it_idx].begin())
            )
        );
        ++(iterators[*min_it_idx].begin());
    }

    return res;
}

void Mapper::saveResults(const std::vector<StrList> &lists) {
    const std::string out_fname_prefix = "mapper_";
    const std::string out_fname_suffix = ".out";
    size_t idx = 0;
    for (const auto& list : lists) {
        std::string filename = out_fname_prefix;
        filename += to_string(idx++);
        filename += out_fname_suffix;
        ofstream out(filename);
        for (const auto& line : list) {
            out << line << '\n';
        }
    }
}

}