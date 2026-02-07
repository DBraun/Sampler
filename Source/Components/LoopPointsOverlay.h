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

#include "LoopPointMarker.h"

class LoopPointsOverlay final : public juce::Component,
    private DataModel::Listener,
    private VisibleRangeDataModel::Listener
{
public:
    LoopPointsOverlay(const DataModel& dModel,
        const VisibleRangeDataModel& vModel,
        UndoManager& undoManagerIn)
        : dataModel(dModel),
        visibleRange(vModel),
        beginMarker("B",
            [this](LoopPointMarker& m, const MouseEvent& e) { this->loopPointMouseDown(m, e); },
            [this](LoopPointMarker& m, const MouseEvent& e) { this->loopPointDragged(m, e); },
            [this](LoopPointMarker& m, const MouseEvent& e) { this->loopPointMouseUp(m, e); }),
        endMarker("E",
            [this](LoopPointMarker& m, const MouseEvent& e) { this->loopPointMouseDown(m, e); },
            [this](LoopPointMarker& m, const MouseEvent& e) { this->loopPointDragged(m, e); },
            [this](LoopPointMarker& m, const MouseEvent& e) { this->loopPointMouseUp(m, e); }),
        undoManager(&undoManagerIn)
    {
        dataModel.addListener(*this);
        visibleRange.addListener(*this);

        for (auto ptr : { &beginMarker, &endMarker })
            addAndMakeVisible(ptr);
    }

private:
    void resized() override
    {
        positionLoopPointMarkers();
    }

    void loopPointMouseDown(LoopPointMarker&, const MouseEvent&)
    {
        loopPointsOnMouseDown = dataModel.getLoopPointsSeconds();
        undoManager->beginNewTransaction();
    }

    void loopPointDragged(LoopPointMarker& marker, const MouseEvent& e)
    {
        auto x = xPositionToTime(e.getEventRelativeTo(this).position.x);
        const Range<double> newLoopRange(&marker == &beginMarker ? x : loopPointsOnMouseDown.getStart(),
            &marker == &endMarker ? x : loopPointsOnMouseDown.getEnd());

        dataModel.setLoopPointsSeconds(newLoopRange, undoManager);
    }

    void loopPointMouseUp(LoopPointMarker& marker, const MouseEvent& e)
    {
        auto x = xPositionToTime(e.getEventRelativeTo(this).position.x);
        const Range<double> newLoopRange(&marker == &beginMarker ? x : loopPointsOnMouseDown.getStart(),
            &marker == &endMarker ? x : loopPointsOnMouseDown.getEnd());

        dataModel.setLoopPointsSeconds(newLoopRange, undoManager);
    }

    void loopPointsSecondsChanged(Range<double>) override
    {
        positionLoopPointMarkers();
    }

    void visibleRangeChanged(Range<double>) override
    {
        positionLoopPointMarkers();
    }

    double timeToXPosition(double time) const
    {
        return (time - visibleRange.getVisibleRange().getStart()) * getWidth()
            / visibleRange.getVisibleRange().getLength();
    }

    double xPositionToTime(double xPosition) const
    {
        return ((xPosition * visibleRange.getVisibleRange().getLength()) / getWidth())
            + visibleRange.getVisibleRange().getStart();
    }

    void positionLoopPointMarkers()
    {
        auto halfMarkerWidth = 7;

        for (auto tup : { std::make_tuple(&beginMarker, dataModel.getLoopPointsSeconds().getStart()),
                          std::make_tuple(&endMarker,   dataModel.getLoopPointsSeconds().getEnd()) })
        {
            auto ptr = std::get<0>(tup);
            auto time = std::get<1>(tup);
            ptr->setSize(halfMarkerWidth * 2, getHeight());
            ptr->setTopLeftPosition(roundToInt(timeToXPosition(time) - halfMarkerWidth), 0);
        }
    }

    DataModel dataModel;
    VisibleRangeDataModel visibleRange;
    Range<double> loopPointsOnMouseDown;
    LoopPointMarker beginMarker, endMarker;
    UndoManager* undoManager;
};
