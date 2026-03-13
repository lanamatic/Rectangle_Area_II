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

// Algorithm state for visualisation
struct AlgorithmState {
    int eventIndex;             // which event (-1 = starting index)
    int sweepX;                 // X position  of sweep line
    int prevX;                  // last X position for strip visualisation
    std::vector<int> cnt;       // cnt[i] number of rectangles covering of the segment i
    long long activeHeight;     // active height in the strip [prevX, sweepX)
    long long stripArea;        // area of the strip  = width * activeHeight
    long long totalArea;
    long long totalAreaMod;
    std::string description;
    bool isCompute;
};


class SweepLineAlgorithm {
public:

    std::vector<std::vector<int>> rectangles;

    std::vector<int> yVals;         // Sorted unique Y coordinates
    std::map<int, int> yIndex;      // Real Y -> Compressed index
    int numSegments = 0;            // Number of Y segments = yVals.size() - 1


    std::vector<Event> events;
    std::vector<AlgorithmState> states; // States for visualisation

    // World borders for visualisation
    int wxMin = 0, wxMax = 0, wyMin = 0, wyMax = 0;

    long long totalArea = 0;

    void solve(const std::vector<std::vector<int>>& rects);

    // Returns result mod 10^9+7
    int resultMod() const;

    static int rectangleArea(std::vector<std::vector<int>>& rects);

private:
    void compressCoordinates();
    void createEvents();
    void computeWorldBounds();
    void precomputeStates();
};

#endif // ALGORITHM_SOLUTION_H
