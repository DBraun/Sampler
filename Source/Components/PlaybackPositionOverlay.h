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

#include "../DataModels/VisibleRangeDataModel.h"

class PlaybackPositionOverlay final : public juce::Component,
    private Timer,
    private VisibleRangeDataModel::Listener
{
public:
    using Provider = std::function<std::vector<float>()>;
    PlaybackPositionOverlay(const VisibleRangeDataModel& model,
        Provider providerIn)
        : visibleRange(model),
        provider(std::move(providerIn))
    {
        visibleRange.addListener(*this);
        startTimer(16);
    }

private:
    void paint(Graphics& g) override
    {
        g.setColour(juce::Colours::red);

        for (auto position : provider())
        {
            g.drawVerticalLine(roundToInt(timeToXPosition(position)), 0.0f, (float)getHeight());
        }
    }

    void timerCallback() override
    {
        repaint();
    }

    void visibleRangeChanged(Range<double>) override
    {
        repaint();
    }

    double timeToXPosition(double time) const
    {
        return (time - visibleRange.getVisibleRange().getStart()) * getWidth()
            / visibleRange.getVisibleRange().getLength();
    }

    VisibleRangeDataModel visibleRange;
    Provider provider;
};
