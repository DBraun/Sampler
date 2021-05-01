/*
  ==============================================================================
   This file is part of the JUCE examples.
   Copyright (c) 2020 - Raw Material Software Limited
   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.
   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.
  ==============================================================================
*/

#pragma once

//==============================================================================
// A class which contains all the information related to sample-playback, such
// as sample data, loop points, and loop kind.
// It is expected that multiple sampler voices will maintain pointers to a
// single instance of this class, to avoid redundant duplication of sample
// data in memory.
class MPESamplerSound final
{
public:
    void setSample(std::unique_ptr<Sample> value)
    {
        sample = move(value);
        setLoopPointsInSeconds(loopPoints);
    }

    Sample* getSample() const
    {
        return sample.get();
    }

    void setLoopPointsInSeconds(Range<double> value)
    {
        loopPoints = sample == nullptr ? value
            : Range<double>(0, sample->getLength() / sample->getSampleRate())
            .constrainRange(value);
    }

    Range<double> getLoopPointsInSeconds() const
    {
        return loopPoints;
    }

    void setCentreFrequencyInHz(double centre)
    {
        centreFrequencyInHz = centre;
    }

    double getCentreFrequencyInHz() const
    {
        return centreFrequencyInHz;
    }

    void setLoopMode(LoopMode type)
    {
        loopMode = type;
    }

    LoopMode getLoopMode() const
    {
        return loopMode;
    }

private:
    std::unique_ptr<Sample> sample;
    double centreFrequencyInHz{ 440.0 };
    Range<double> loopPoints;
    LoopMode loopMode{ LoopMode::none };
};