#pragma once

struct StageEvent
{
    string name;
    Vec2 pos;
    StageEvent(const string n, const double x, const double y) {
        name = n;
        pos.x = x;
        pos.y = y;
    }
};

class Stage
{
public:
    Stage();

    const int getStageEvents() { return csv_.rows; }

    const int getNextEventFrame();

    StageEvent getNextEvent();

    const bool isEnd() { return nextRow_ == csv_.rows; }

private:
    CSVReader csv_;
    int nextRow_;
};
