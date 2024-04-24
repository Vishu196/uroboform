#pragma once

enum class DebugMode
{
    AllValues,
    AllTimes,
    FinalTime,
    FinalValue,
    AllTimesValues,
    FinalTimeValue
};

#define DEBUGMODE DebugMode::FinalTime