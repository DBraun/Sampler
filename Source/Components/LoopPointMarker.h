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

class LoopPointMarker : public Component
{
public:
    using MouseCallback = std::function<void(LoopPointMarker&, const MouseEvent&)>;

    LoopPointMarker(String marker,
        MouseCallback onMouseDownIn,
        MouseCallback onMouseDragIn,
        MouseCallback onMouseUpIn)
        : text(std::move(marker)),
        onMouseDown(std::move(onMouseDownIn)),
        onMouseDrag(std::move(onMouseDragIn)),
        onMouseUp(std::move(onMouseUpIn))
    {
        setMouseCursor(MouseCursor::LeftRightResizeCursor);
    }

private:
    void resized() override
    {
        auto height = 20;
        auto triHeight = 6;

        auto bounds = getLocalBounds();
        Path newPath;
        newPath.addRectangle(bounds.removeFromBottom(height));

        newPath.startNewSubPath(bounds.getBottomLeft().toFloat());
        newPath.lineTo(bounds.getBottomRight().toFloat());
        juce::Point<float> apex(static_cast<float> (bounds.getX() + (bounds.getWidth() / 2)),
            static_cast<float> (bounds.getBottom() - triHeight));
        newPath.lineTo(apex);
        newPath.closeSubPath();

        newPath.addLineSegment(Line<float>(apex, juce::Point<float>(apex.getX(), 0)), 1);

        path = newPath;
    }

    void paint(Graphics& g) override
    {
        g.setColour(juce::Colours::deepskyblue);
        g.fillPath(path);

        auto height = 20;
        g.setColour(juce::Colours::white);
        g.drawText(text, getLocalBounds().removeFromBottom(height), Justification::centred);
    }

    bool hitTest(int x, int y) override
    {
        return path.contains((float)x, (float)y);
    }

    void mouseDown(const MouseEvent& e) override
    {
        onMouseDown(*this, e);
    }

    void mouseDrag(const MouseEvent& e) override
    {
        onMouseDrag(*this, e);
    }

    void mouseUp(const MouseEvent& e) override
    {
        onMouseUp(*this, e);
    }

    String text;
    Path path;
    MouseCallback onMouseDown;
    MouseCallback onMouseDrag;
    MouseCallback onMouseUp;
};