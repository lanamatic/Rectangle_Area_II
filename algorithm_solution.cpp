#include "algorithm_solution.h"
#include <sstream>

static const int MOD = 1e9 + 7;

void SweepLineAlgorithm::solve(const std::vector<std::vector<int>>& rects)
{
    rectangles = rects;
    compressCoordinates();
    createEvents();
}


int SweepLineAlgorithm::resultMod() const
{
    long long ans = 0;
    int numSeg = numSegments;
    std::vector<int> cnt(numSeg, 0);
    int curX = events.empty() ? 0 : events[0].x;

    for (auto& e : events) {
        long long width = (long long)(e.x - curX);
        if (width > 0) {
            long long activeH = 0;
            for (int i = 0; i < numSeg; i++)
                if (cnt[i] > 0)
                    activeH += (long long)(yVals[i + 1] - yVals[i]);
            ans = (ans + (width % MOD) * (activeH % MOD)) % MOD;
        }
        curX = e.x;
        if (e.type == 0)
            for (int i = e.y1i; i < e.y2i; i++) cnt[i]++;
        else
            for (int i = e.y1i; i < e.y2i; i++) cnt[i]--;
    }
    return (int)ans;
}

int SweepLineAlgorithm::rectangleArea(std::vector<std::vector<int>>& rects)
{
    SweepLineAlgorithm algo;
    algo.solve(rects);
    return algo.resultMod();
}

void SweepLineAlgorithm::compressCoordinates()
{
    yVals.clear();
    yIndex.clear();

    for (auto& r : rectangles) {
        yVals.push_back(r[1]);  // y1
        yVals.push_back(r[3]);  // y2
    }

    std::sort(yVals.begin(), yVals.end());
    yVals.erase(std::unique(yVals.begin(), yVals.end()), yVals.end());

    for (int i = 0; i < (int)yVals.size(); i++) {
        yIndex[yVals[i]] = i;
    }

    numSegments = (int)yVals.size() - 1;
}

void SweepLineAlgorithm::createEvents()
{
    events.clear();

    for (int ri = 0; ri < (int)rectangles.size(); ri++) {
        auto& r = rectangles[ri];
        int y1i = yIndex[r[1]];
        int y2i = yIndex[r[3]];
        events.push_back({r[0], 0, y1i, y2i, ri});   // START on x1
        events.push_back({r[2], 1, y1i, y2i, ri});   // END on x2
    }

    std::sort(events.begin(), events.end());
}

