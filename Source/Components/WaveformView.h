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

class WaveformView : public juce::Component,
    private ChangeListener,
    private DataModel::Listener,
    private VisibleRangeDataModel::Listener
{
public:
    WaveformView(const DataModel& model,
        const VisibleRangeDataModel& vr)
        : dataModel(model),
        visibleRange(vr),
        thumbnailCache(4),
        thumbnail(4, dataModel.getAudioFormatManager(), thumbnailCache)
    {
        dataModel.addListener(*this);
        visibleRange.addListener(*this);
        thumbnail.addChangeListener(this);
    }

private:
    void paint(Graphics& g) override
    {
        // Draw the waveforms
        g.fillAll(juce::Colours::black);
        auto numChannels = thumbnail.getNumChannels();

        if (numChannels == 0)
        {
            g.setColour(juce::Colours::white);
            g.drawFittedText("No File Loaded", getLocalBounds(), Justification::centred, 1);
            return;
        }

        auto bounds = getLocalBounds();
        auto channelHeight = bounds.getHeight() / numChannels;

        for (auto i = 0; i != numChannels; ++i)
        {
            drawChannel(g, i, bounds.removeFromTop(channelHeight));
        }
    }

    void changeListenerCallback(ChangeBroadcaster* source) override
    {
        if (source == &thumbnail)
            repaint();
    }

    void sampleReaderChanged(std::shared_ptr<AudioFormatReaderFactory> value) override
    {
        if (value != nullptr)
        {
            if (auto reader = value->make(dataModel.getAudioFormatManager()))
            {
                thumbnail.setReader(reader.release(), currentHashCode);
                currentHashCode += 1;

                return;
            }
        }

        thumbnail.clear();
    }

    void visibleRangeChanged(Range<double>) override
    {
        repaint();
    }

    void drawChannel(Graphics& g, int channel, juce::Rectangle<int> bounds)
    {
        g.setGradientFill(ColourGradient(juce::Colours::lightblue,
            bounds.getTopLeft().toFloat(),
            juce::Colours::darkgrey,
            bounds.getBottomLeft().toFloat(),
            false));
        thumbnail.drawChannel(g,
            bounds,
            visibleRange.getVisibleRange().getStart(),
            visibleRange.getVisibleRange().getEnd(),
            channel,
            1.0f);
    }

    DataModel dataModel;
    VisibleRangeDataModel visibleRange;
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;
    int64 currentHashCode = 0;
};
