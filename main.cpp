#include <iostream>
#include <fstream>

#include "mapper.h"
#include "reducer.h"
#include "min_prefix.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cout << "# yamr <src> <mnum> <rnum>\n"
             << "src -- path to input file\n"
             << "mnum -- threads number for mapper\n"
             << "rnum -- threads number for reducer"
             << endl;
        return 0;
    }

    const string mapper_out_fname {"mapper_merged.out"};
    try {
        const string input_file_name{argv[1]};
        size_t mnum = stol(argv[2]);
        size_t rnum = stol(argv[3]);

        Yamr::Mapper m(input_file_name, mnum);
        auto res = m.run(all_prefixes);

        ofstream out(mapper_out_fname);
        for (const auto &line : res) {
            out << line << '\n';
        }
        out.close();

//        Yamr::Reducer r(move(res), rnum);
        Yamr::Reducer r(mapper_out_fname, rnum);
        r.run(GetMinUniquePrefixSize(), "output_");
        return 0;
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }
}
