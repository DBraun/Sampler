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

#include "SamplerAudioProcessor.h"
#include "Components/MPESettingsComponent.h"
#ifndef SAMPLER_SKIP_UI
#include "Components/MainSamplerView.h"
#endif
#include "ProcessorState.h"

using namespace std;

class SamplerAudioProcessorEditor : public AudioProcessorEditor,
    public FileDragAndDropTarget,
    private DataModel::Listener,
    private MPESettingsDataModel::Listener
{
public:
    SamplerAudioProcessorEditor(SamplerAudioProcessor& p, ProcessorState state, const DataModel& model, AudioFormatManager& afManager, AudioProcessorValueTreeState& vts);

private:
    void resized() override;

    bool keyPressed(const KeyPress& key) override;

    bool isInterestedInFileDrag(const StringArray& files) override;

    void filesDropped(const StringArray& files, int, int) override;

    void sampleReaderChanged(std::shared_ptr<AudioFormatReaderFactory> value) override;

    void centreFrequencyHzChanged(double value) override;

    void loopPointsSecondsChanged(Range<double> value) override;

    void loopModeChanged(LoopMode value) override;

    void synthVoicesChanged(int value) override;

    void voiceStealingEnabledChanged(bool value) override;

    void legacyModeEnabledChanged(bool value) override;

    void mpeZoneLayoutChanged(const MPEZoneLayout&) override;

    void legacyFirstChannelChanged(int) override;

    void legacyLastChannelChanged(int) override;

    void legacyPitchbendRangeChanged(int) override;

    void setProcessorLegacyMode();

    void setProcessorMPEMode();

    SamplerAudioProcessor& samplerAudioProcessor;
    AudioFormatManager& formatManager;
    DataModel dataModel;
    UndoManager undoManager;
    MPESettingsDataModel mpeSettings{ dataModel.mpeSettings() };

    TabbedComponent tabbedComponent{ TabbedButtonBar::Orientation::TabsAtTop };
    MPESettingsComponent settingsComponent{ dataModel.mpeSettings(), undoManager };
#ifndef SAMPLER_SKIP_UI
    MainSamplerView mainSamplerView;
#endif
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SamplerAudioProcessorEditor)
};
