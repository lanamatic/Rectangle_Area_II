#ifndef ALGORITHM_SOLUTION_H
#define ALGORITHM_SOLUTION_H

#include <vector>
#include <map>

struct Event {
    int x;          // X coordinate
    int type;       // 0 = StART (left rectangle edge), 1 = END (right rectangle edge)
    int y1i, y2i;   // Y index intervals [y1i, y2i)
    int rectIndex;  // index  of original recatngel (for visualisation)

    // Sort by X, then START before end if X is the same
    bool operator<(const Event& o) const {
        if (x != o.x) return x < o.x;
        return type < o.type;
    }
};


class SweepLineAlgorithm {
public:

    std::vector<std::vector<int>> rectangles;

    std::vector<int> yVals;         // Sorted unique Y coordinates
    std::map<int, int> yIndex;      // Real Y -> Compressed index
    int numSegments = 0;            // Number of Y segments = yVals.size() - 1


    std::vector<Event> events;

    void solve(const std::vector<std::vector<int>>& rects);

    // Returns result mod 10^9+7
    int resultMod() const;

    static int rectangleArea(std::vector<std::vector<int>>& rects);

private:
    void compressCoordinates();
    void createEvents();
};

#endif // ALGORITHM_SOLUTION_H
