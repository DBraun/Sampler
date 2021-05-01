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

#include "MPESettingsDataModel.h"
#include "MPESamplerVoiceDataModel.h"

class DataModel : private ValueTree::Listener
{
public:
    class Listener
    {
    public:
        virtual ~Listener() noexcept = default;
        virtual void sampleReaderChanged(std::shared_ptr<AudioFormatReaderFactory>) {}
        virtual void centreFrequencyHzChanged(double) {}
        virtual void loopModeChanged(LoopMode) {}
        virtual void loopPointsSecondsChanged(Range<double>) {}
    };

    explicit DataModel(AudioFormatManager& audioFormatManagerIn);

    DataModel(AudioFormatManager& audioFormatManagerIn, const ValueTree& vt);

    DataModel(const DataModel& other);

    DataModel& operator= (const DataModel& other);

    std::unique_ptr<AudioFormatReader> getSampleReader() const;

    void setSampleReader(std::unique_ptr<AudioFormatReaderFactory> readerFactory,
        UndoManager* undoManager);

    double getSampleLengthSeconds() const;

    double getCentreFrequencyHz() const;

    void setCentreFrequencyHz(double value, UndoManager* undoManager);

    LoopMode getLoopMode() const;

    void setLoopMode(LoopMode value, UndoManager* undoManager);

    Range<double> getLoopPointsSeconds() const;

    void setLoopPointsSeconds(Range<double> value, UndoManager* undoManager);

    MPESettingsDataModel mpeSettings();

    MPESamplerVoiceDataModel samplerVoiceSettings();

    void addListener(Listener& listener);

    void removeListener(Listener& listener);

    void swap(DataModel& other) noexcept;

    AudioFormatManager& getAudioFormatManager() const;

private:
    void valueTreePropertyChanged(ValueTree&, const Identifier& property) override;

    void valueTreeChildAdded(ValueTree&, ValueTree&)      override;
    void valueTreeChildRemoved(ValueTree&, ValueTree&, int) override;
    void valueTreeChildOrderChanged(ValueTree&, int, int)        override;
    void valueTreeParentChanged(ValueTree&)                  override;

    AudioFormatManager* audioFormatManager;

    ValueTree valueTree;

    CachedValue<std::shared_ptr<AudioFormatReaderFactory>> sampleReader;
    CachedValue<double> centreFrequencyHz;
    CachedValue<LoopMode> loopMode;
    CachedValue<Range<double>> loopPointsSeconds;

    ListenerList<Listener> listenerList;
};