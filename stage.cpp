#include "stage.h"

Stage::Stage() : nextRow_(0)
{
    csv_.open(L"Assets/stage.csv");
}

const int Stage::getNextEventFrame()
{
    return csv_.get<int>(nextRow_, 0);
}

StageEvent Stage::getNextEvent()
{

    StageEvent ev(Narrow(csv_.get<String>(nextRow_, 1)), csv_.get<int>(nextRow_, 2), csv_.get<int>(nextRow_, 3));
    nextRow_++;
    return ev;
}

