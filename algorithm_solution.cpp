#include "algorithm_solution.h"
#include <sstream>

static const int MOD = 1e9 + 7;

void SweepLineAlgorithm::solve(const std::vector<std::vector<int>>& rects)
{
    rectangles = rects;
    totalArea = 0;

    compressCoordinates();
    createEvents();
    computeWorldBounds();
    precomputeStates();
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

// Borders for visualisation
void SweepLineAlgorithm::computeWorldBounds()
{
    wxMin = wyMin = INT_MAX;
    wxMax = wyMax = INT_MIN;

    for (auto& r : rectangles) {
        wxMin = std::min(wxMin, r[0]);
        wyMin = std::min(wyMin, r[1]);
        wxMax = std::max(wxMax, r[2]);
        wyMax = std::max(wyMax, r[3]);
    }

    wxMin -= 1;  wyMin -= 1;
    wxMax += 1;  wyMax += 1;
}




//  Sweep line algorithm and record states after every step
//  Visualiation uses these states
void SweepLineAlgorithm::precomputeStates()
{
    states.clear();
    std::vector<int> cnt(numSegments, 0);
    totalArea = 0;


    // Starting state
    AlgorithmState s;
    s.eventIndex = -1;
    s.sweepX = wxMin;
    s.prevX = wxMin;
    s.cnt = cnt;
    s.activeHeight = 0;
    s.stripArea = 0;
    s.totalArea = 0;
    s.totalAreaMod = totalArea % (int)MOD;
    s.isCompute = false;

    std::ostringstream oss;
    oss << "Starting state. Recatngles: " << rectangles.size()
        << ", Y segments: " << numSegments
        << "\ncnt[] = all zeros. Sweep line starts from the left.";
    s.description = oss.str();

    states.push_back(s);


    int curX = events.empty() ? 0 : events[0].x;

    for (int ei = 0; ei < (int)events.size(); ei++) {
        auto& e = events[ei];

        // Strip Area
        long long width = (long long)(e.x - curX);
        if (width > 0) {
            long long activeH = 0;
            for (int i = 0; i < numSegments; i++)
                if (cnt[i] > 0)
                    activeH += (long long)(yVals[i + 1] - yVals[i]);

            long long stripA = (width% (int)MOD) * (activeH % (int)MOD) % (int)MOD;
            totalArea += stripA; // Total Area for now

            AlgorithmState s;
            s.eventIndex = ei;
            s.sweepX = e.x;
            s.prevX = curX;
            s.cnt = cnt;
            s.activeHeight = activeH;
            s.stripArea = stripA;
            s.totalArea = totalArea;
            s.totalAreaMod = totalArea % (int)MOD;
            s.isCompute = true;

            std::ostringstream oss;
            oss << "Strip [" << curX << ", " << e.x << "): "
                << "width=" << width << ", active height=" << activeH
                << "\nStrip Area: " << width  << " × " << activeH
                << " = " << stripA << " mod (1e9 + 7)"
                << "\nTotal Area for now: " << totalArea % (int)MOD << " mod (1e9 + 7)";
            s.description = oss.str();

            states.push_back(s);
        }
        curX = e.x;

        // Updating cnt
        std::string evtType = (e.type == 0) ? "START" : "END";

        if (e.type == 0)
            for (int i = e.y1i; i < e.y2i; i++) cnt[i]++;
        else
            for (int i = e.y1i; i < e.y2i; i++) cnt[i]--;

        long long activeH = 0;
        for (int i = 0; i < numSegments; i++)
            if (cnt[i] > 0)
                activeH += (long long)(yVals[i + 1] - yVals[i]);

        auto& r = rectangles[e.rectIndex];

        AlgorithmState s;
        s.eventIndex = ei;
        s.sweepX = e.x;
        s.prevX = curX;
        s.cnt = cnt;
        s.activeHeight = activeH;
        s.stripArea = 0;
        s.totalArea = totalArea;
        s.totalAreaMod = totalArea % (int)MOD;
        s.isCompute = false;

        std::ostringstream oss;
        oss << "x=" << e.x << ": " << evtType
            << " rectangle R" << e.rectIndex
            << "=[" << r[0] << "," << r[1] << "," << r[2] << "," << r[3] << "]"
            << "\nY interval: [" << yVals[e.y1i] << ", " << yVals[e.y2i] << ")"
            << "  →  segments [" << e.y1i << ".." << e.y2i << ")"
            << "\ncnt[] updated. Active height now: " << activeH;
        s.description = oss.str();

        states.push_back(s);
    }

    // End state
    {
        AlgorithmState s;
        s.eventIndex = (int)events.size();
        s.sweepX = wxMax;
        s.prevX = curX;
        s.cnt = std::vector<int>(numSegments, 0);
        s.activeHeight = 0;
        s.stripArea = 0;
        s.totalArea = totalArea;
        s.totalAreaMod = totalArea % (int)MOD;
        s.isCompute = false;

        std::ostringstream oss;
        oss << "DONE! Total area = " << totalArea % (int)MOD << " mod (1e9 + 7)";
        s.description = oss.str();

        states.push_back(s);
    }
}

