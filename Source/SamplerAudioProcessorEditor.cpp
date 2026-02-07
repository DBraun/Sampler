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

#include "SamplerAudioProcessorEditor.h"
#include "SamplerAudioProcessor.h"

SamplerAudioProcessorEditor::SamplerAudioProcessorEditor(SamplerAudioProcessor& p, ProcessorState state, const DataModel& model,
    AudioFormatManager& afManager, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p),
    dataModel(model),
    formatManager(afManager),
    samplerAudioProcessor(p)
#ifndef SAMPLER_SKIP_UI
,mainSamplerView(dataModel,
        [&p]
        {
            std::vector<float> ret;
            auto voices = p.getNumVoices();
            ret.reserve((size_t)voices);

            for (auto i = 0; i != voices; ++i)
                ret.emplace_back(p.getPlaybackPosition(i));

            return ret;
        },
            undoManager,
            vts)
#endif
{
    dataModel.addListener(*this);
    mpeSettings.addListener(*this);

    formatManager.registerBasicFormats();

    addAndMakeVisible(tabbedComponent);

    auto lookFeel = dynamic_cast<LookAndFeel_V4*> (&getLookAndFeel());
    auto bg = lookFeel->getCurrentColourScheme()
        .getUIColour(LookAndFeel_V4::ColourScheme::UIColour::widgetBackground);
#ifndef SAMPLER_SKIP_UI
    tabbedComponent.addTab("Sample Editor", bg, &mainSamplerView, false);
#endif
    tabbedComponent.addTab("MPE Settings", bg, &settingsComponent, false);

    mpeSettings.setSynthVoices(state.synthVoices, nullptr);
    mpeSettings.setLegacyModeEnabled(state.legacyModeEnabled, nullptr);
    mpeSettings.setLegacyFirstChannel(state.legacyChannels.getStart(), nullptr);
    mpeSettings.setLegacyLastChannel(state.legacyChannels.getEnd(), nullptr);
    mpeSettings.setLegacyPitchbendRange(state.legacyPitchbendRange, nullptr);
    mpeSettings.setVoiceStealingEnabled(state.voiceStealingEnabled, nullptr);
    mpeSettings.setMPEZoneLayout(state.mpeZoneLayout, nullptr);

    dataModel.setSampleReader(std::move(state.readerFactory), nullptr);
    dataModel.setLoopPointsSeconds(state.loopPointsSeconds, nullptr);
    dataModel.setCentreFrequencyHz(state.centreFrequencyHz, nullptr);
    dataModel.setLoopMode(state.loopMode, nullptr);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setResizable(true, true);
    setResizeLimits(640, 480, 2560, 1440);
    setSize(640, 480);
}

void SamplerAudioProcessorEditor::resized()
{
    tabbedComponent.setBounds(getLocalBounds());
}

bool SamplerAudioProcessorEditor::keyPressed(const KeyPress& key)
{
    if (key == KeyPress('z', ModifierKeys::commandModifier, 0))
    {
        undoManager.undo();
        return true;
    }

    if (key == KeyPress('z', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0))
    {
        undoManager.redo();
        return true;
    }

    return Component::keyPressed(key);
}

bool SamplerAudioProcessorEditor::isInterestedInFileDrag(const StringArray& files)
{
    WildcardFileFilter filter(formatManager.getWildcardForAllFormats(), {}, "Known Audio Formats");
    return files.size() == 1 && filter.isFileSuitable(files[0]);
}

void SamplerAudioProcessorEditor::filesDropped(const StringArray& files, int, int)
{
    jassert(files.size() == 1);
    undoManager.beginNewTransaction();
    auto r = new FileAudioFormatReaderFactory(files[0]);
    dataModel.setSampleReader(std::unique_ptr<AudioFormatReaderFactory>(r),
        &undoManager);

}

void SamplerAudioProcessorEditor::sampleReaderChanged(std::shared_ptr<AudioFormatReaderFactory> value)
{
    samplerAudioProcessor.setSample(value == nullptr ? nullptr : value->clone(),
        dataModel.getAudioFormatManager());
}

void SamplerAudioProcessorEditor::centreFrequencyHzChanged(double value)
{
    samplerAudioProcessor.setCentreFrequency(value);
}

void SamplerAudioProcessorEditor::loopPointsSecondsChanged(Range<double> value)
{
    samplerAudioProcessor.setLoopPoints(value);
}

void SamplerAudioProcessorEditor::loopModeChanged(LoopMode value)
{
    samplerAudioProcessor.setLoopMode(value);
}

void SamplerAudioProcessorEditor::synthVoicesChanged(int value)
{
    samplerAudioProcessor.setNumberOfVoices(value);
}

void SamplerAudioProcessorEditor::voiceStealingEnabledChanged(bool value)
{
    samplerAudioProcessor.setVoiceStealingEnabled(value);
}

void SamplerAudioProcessorEditor::legacyModeEnabledChanged(bool value)
{
    if (value)
        setProcessorLegacyMode();
    else
        setProcessorMPEMode();
}

void SamplerAudioProcessorEditor::mpeZoneLayoutChanged(const MPEZoneLayout&)
{
    setProcessorMPEMode();
}

void SamplerAudioProcessorEditor::legacyFirstChannelChanged(int)
{
    setProcessorLegacyMode();
}

void SamplerAudioProcessorEditor::legacyLastChannelChanged(int)
{
    setProcessorLegacyMode();
}

void SamplerAudioProcessorEditor::legacyPitchbendRangeChanged(int)
{
    setProcessorLegacyMode();
}

void SamplerAudioProcessorEditor::setProcessorLegacyMode()
{
    samplerAudioProcessor.setLegacyModeEnabled(mpeSettings.getLegacyPitchbendRange(),
        Range<int>(mpeSettings.getLegacyFirstChannel(),
            mpeSettings.getLegacyLastChannel()));
}

void SamplerAudioProcessorEditor::setProcessorMPEMode()
{
    samplerAudioProcessor.setMPEZoneLayout(mpeSettings.getMPEZoneLayout());
}
