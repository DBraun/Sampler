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

#include "Misc.h"
#include "MemoryAudioFormatReaderFactory.h"
#include "Sample.h"
#include "DataModels/DataModel.h"
#include "MPESamplerSound.h"
#include "MPESamplerVoice.h"
#include "CommandFifo.h"


class SamplerAudioProcessor : public AudioProcessor, public AudioProcessorValueTreeState::Listener
{

public:
    SamplerAudioProcessor();

    ~SamplerAudioProcessor();

    void prepareToPlay(double sampleRate, int) override;

    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;

    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int) override;
    const String getProgramName(int) override;
    void changeProgramName(int, const String&) override;

    //==============================================================================
    void getStateInformation(MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    //==============================================================================
    void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midi) override;

    void processBlock(AudioBuffer<double>& buffer, MidiBuffer& midi) override;

    // These should be called from the GUI thread, and will block until the
    // command buffer has enough room to accept a command.
    void setSample(std::unique_ptr<AudioFormatReaderFactory> fact, AudioFormatManager& formatManager);

    // This method is not thread-safe at all and is only meant to be used by DawDreamer.
    void setSample(std::vector<std::vector<float>> soundData, double sampleRate);

    // Set the sample with an absolute filepath to a wav file. Not thread-safe at all.
    void setSample(const char* path);

    void setCentreFrequency(double centreFrequency);

    void setLoopMode(LoopMode loopMode);

    void setLoopPoints(Range<double> loopPoints);

    void setMPEZoneLayout(MPEZoneLayout layout);

    void setLegacyModeEnabled(int pitchbendRange, Range<int> channelRange);

    void setVoiceStealingEnabled(bool voiceStealingEnabled);

    void setNumberOfVoices(int numberOfVoices);

    // These accessors are just for an 'overview' and won't give the exact
    // state of the audio engine at a particular point in time.
    // If you call getNumVoices(), get the result '10', and then call
    // getPlaybackPosiiton(9), there's a chance the audio engine will have
    // been updated to remove some voices in the meantime, so the returned
    // value won't correspond to an existing voice.
    int getNumVoices() const;
    float getPlaybackPosition(int voice) const;

    void parameterChanged(const String& parameterID, float newValue);

private:
    //==============================================================================
    template <typename Element>
    void process(AudioBuffer<Element>& buffer, MidiBuffer& midiMessages);

    void setSample(juce::InputStream* inputStream);

    CommandFifo<SamplerAudioProcessor> commands;

    MemoryBlock mb;
    std::unique_ptr<AudioFormatReaderFactory> readerFactory;
    std::shared_ptr<MPESamplerSound> samplerSound = std::make_shared<MPESamplerSound>();
    MPESynthesiser synthesiser;

    AudioFormatManager formatManager;
    DataModel dataModel{ formatManager };

    AudioProcessorValueTreeState parameters;
    AudioProcessorValueTreeState::ParameterLayout createParameters();

    // This mutex is used to ensure we don't modify the processor state during
    // a call to createEditor, which would cause the UI to become desynched
    // with the real state of the processor.
    SpinLock commandQueueMutex;

    enum { maxVoices = 20 };

    // This is used for visualising the current playback position of each voice.
    // It stores values in seconds units.
    std::array<std::atomic<float>, maxVoices> playbackPositions;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SamplerAudioProcessor)
};
