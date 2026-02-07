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

class Ruler final : public juce::Component,
    private VisibleRangeDataModel::Listener
{
public:
    explicit Ruler(const VisibleRangeDataModel& model)
        : visibleRange(model)
    {
        visibleRange.addListener(*this);
        setMouseCursor(MouseCursor::LeftRightResizeCursor);
    }

private:
    void paint(Graphics& g) override
    {
        auto minDivisionWidth = 50.0f;
        auto maxDivisions = (float)getWidth() / minDivisionWidth;

        auto lookFeel = dynamic_cast<LookAndFeel_V4*> (&getLookAndFeel());
        auto bg = lookFeel->getCurrentColourScheme()
            .getUIColour(LookAndFeel_V4::ColourScheme::UIColour::widgetBackground);

        g.setGradientFill(ColourGradient(bg.brighter(),
            0,
            0,
            bg.darker(),
            0,
            (float)getHeight(),
            false));

        g.fillAll();
        g.setColour(bg.brighter());
        g.drawHorizontalLine(0, 0.0f, (float)getWidth());
        g.setColour(bg.darker());
        g.drawHorizontalLine(1, 0.0f, (float)getWidth());
        g.setColour(juce::Colours::lightgrey);

        auto minLog = std::ceil(std::log10(visibleRange.getVisibleRange().getLength() / maxDivisions));
        auto precision = 2 + std::abs(minLog);
        auto divisionMagnitude = std::pow(10, minLog);
        auto startingDivision = std::ceil(visibleRange.getVisibleRange().getStart() / divisionMagnitude);

        for (auto div = startingDivision; div * divisionMagnitude < visibleRange.getVisibleRange().getEnd(); ++div)
        {
            auto time = div * divisionMagnitude;
            auto xPos = (time - visibleRange.getVisibleRange().getStart()) * getWidth()
                / visibleRange.getVisibleRange().getLength();

            std::ostringstream outStream;
            outStream << std::setprecision(roundToInt(precision)) << time;

            const auto bounds = juce::Rectangle<int>(juce::Point<int>(roundToInt(xPos) + 3, 0),
                juce::Point<int>(roundToInt(xPos + minDivisionWidth), getHeight()));

            g.drawText(outStream.str(), bounds, Justification::centredLeft, false);

            g.drawVerticalLine(roundToInt(xPos), 2.0f, (float)getHeight());
        }
    }

    void mouseDown(const MouseEvent& e) override
    {
        visibleRangeOnMouseDown = visibleRange.getVisibleRange();
        timeOnMouseDown = visibleRange.getVisibleRange().getStart()
            + (visibleRange.getVisibleRange().getLength() * e.getMouseDownX()) / getWidth();
    }

    void mouseDrag(const MouseEvent& e) override
    {
        // Work out the scale of the new range
        auto unitDistance = 100.0f;
        auto scaleFactor = 1.0 / std::pow(2, (float)e.getDistanceFromDragStartY() / unitDistance);

        // Now position it so that the mouse continues to point at the same
        // place on the ruler.
        auto visibleLength = std::max(0.12, visibleRangeOnMouseDown.getLength() * scaleFactor);
        auto rangeBegin = timeOnMouseDown - visibleLength * e.x / getWidth();
        const Range<double> range(rangeBegin, rangeBegin + visibleLength);
        visibleRange.setVisibleRange(range, nullptr);
    }

    void visibleRangeChanged(Range<double>) override
    {
        repaint();
    }

    VisibleRangeDataModel visibleRange;
    Range<double> visibleRangeOnMouseDown;
    double timeOnMouseDown;
};
