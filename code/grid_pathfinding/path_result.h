#ifndef PATH_RESULT_H
#define PATH_RESULT_H

#include <string>

struct PathResult {
    bool found;
    long long score;
    std::string path;
    int remaining_lives;
};

#endif
