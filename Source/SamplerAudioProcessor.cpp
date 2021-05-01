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

#include "SamplerAudioProcessor.h"
#include "SamplerAudioProcessorEditor.h"


SamplerAudioProcessor::SamplerAudioProcessor() 
    : AudioProcessor(BusesProperties().withOutput("Output", AudioChannelSet::stereo(), true))
{
    if (auto inputStream = createAssetInputStream("cello.wav"))
    {
        inputStream->readIntoMemoryBlock(mb);
        readerFactory.reset(new MemoryAudioFormatReaderFactory(mb.getData(), mb.getSize()));
    }

    // Set up initial sample, which we load from a binary resource
    AudioFormatManager manager;
    manager.registerBasicFormats();
    auto reader = readerFactory->make(manager);
    jassert(reader != nullptr); // Failed to load resource!

    auto sound = samplerSound;
    auto sample = std::unique_ptr<Sample>(new Sample(*reader, 10.0));
    auto lengthInSeconds = sample->getLength() / sample->getSampleRate();
    sound->setLoopPointsInSeconds({ lengthInSeconds * 0.1, lengthInSeconds * 0.9 });
    sound->setSample(move(sample));

    // Start with the max number of voices
    for (auto i = 0; i != maxVoices; ++i) {
        synthesiser.addVoice(new MPESamplerVoice(sound, dataModel.samplerVoiceSettings()));
    }
}

void SamplerAudioProcessor::prepareToPlay(double sampleRate, int)
{
    synthesiser.setCurrentPlaybackSampleRate(sampleRate);
}

void SamplerAudioProcessor::releaseResources() {}

bool SamplerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == AudioChannelSet::stereo();
}

//==============================================================================
AudioProcessorEditor* SamplerAudioProcessor::createEditor()
{
    // This function will be called from the message thread. We lock the command
    // queue to ensure that no messages are processed for the duration of this
    // call.
    SpinLock::ScopedLockType lock(commandQueueMutex);

    ProcessorState state;
    state.synthVoices = synthesiser.getNumVoices();
    state.legacyModeEnabled = synthesiser.isLegacyModeEnabled();
    state.legacyChannels = synthesiser.getLegacyModeChannelRange();
    state.legacyPitchbendRange = synthesiser.getLegacyModePitchbendRange();
    state.voiceStealingEnabled = synthesiser.isVoiceStealingEnabled();
    state.mpeZoneLayout = synthesiser.getZoneLayout();
    state.readerFactory = readerFactory == nullptr ? nullptr : readerFactory->clone();

    auto sound = samplerSound;
    state.loopPointsSeconds = sound->getLoopPointsInSeconds();
    state.centreFrequencyHz = sound->getCentreFrequencyInHz();
    state.loopMode = sound->getLoopMode();

    return new SamplerAudioProcessorEditor(*this, std::move(state), this->dataModel, this->formatManager);
}

bool SamplerAudioProcessor::hasEditor() const { return true; }

//==============================================================================
const String SamplerAudioProcessor::getName() const { return "SamplerPlugin"; }
bool SamplerAudioProcessor::acceptsMidi() const { return true; }
bool SamplerAudioProcessor::producesMidi() const { return false; }
bool SamplerAudioProcessor::isMidiEffect() const { return false; }
double SamplerAudioProcessor::getTailLengthSeconds() const { return 0.0; }

//==============================================================================
int SamplerAudioProcessor::getNumPrograms() { return 1; }
int SamplerAudioProcessor::getCurrentProgram() { return 0; }
void SamplerAudioProcessor::setCurrentProgram(int) {}
const String SamplerAudioProcessor::getProgramName(int) { return {}; }
void SamplerAudioProcessor::changeProgramName(int, const String&) {}

//==============================================================================
void SamplerAudioProcessor::getStateInformation(MemoryBlock&) {}
void SamplerAudioProcessor::setStateInformation(const void*, int) {}

//==============================================================================
void SamplerAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midi)
{
    process(buffer, midi);
}

void SamplerAudioProcessor::processBlock(AudioBuffer<double>& buffer, MidiBuffer& midi)
{
    process(buffer, midi);
}

// These should be called from the GUI thread, and will block until the
// command buffer has enough room to accept a command.
void SamplerAudioProcessor::setSample(std::unique_ptr<AudioFormatReaderFactory> fact, AudioFormatManager& formatManager)
{
    class SetSampleCommand
    {
    public:
        SetSampleCommand(std::unique_ptr<AudioFormatReaderFactory> r,
            std::unique_ptr<Sample> sampleIn,
            std::vector<std::unique_ptr<MPESamplerVoice>> newVoicesIn)
            : readerFactory(std::move(r)),
            sample(std::move(sampleIn)),
            newVoices(std::move(newVoicesIn))
        {}

        void operator() (SamplerAudioProcessor& proc)
        {
            proc.readerFactory = move(readerFactory);
            auto sound = proc.samplerSound;
            sound->setSample(std::move(sample));
            auto numberOfVoices = proc.synthesiser.getNumVoices();
            proc.synthesiser.clearVoices();

            for (auto it = begin(newVoices); proc.synthesiser.getNumVoices() < numberOfVoices; ++it)
            {
                proc.synthesiser.addVoice(it->release());
            }
        }

    private:
        std::unique_ptr<AudioFormatReaderFactory> readerFactory;
        std::unique_ptr<Sample> sample;
        std::vector<std::unique_ptr<MPESamplerVoice>> newVoices;
    };

    // Note that all allocation happens here, on the main message thread. Then,
    // we transfer ownership across to the audio thread.
    auto loadedSamplerSound = samplerSound;
    std::vector<std::unique_ptr<MPESamplerVoice>> newSamplerVoices;
    newSamplerVoices.reserve(maxVoices);

    for (auto i = 0; i != maxVoices; ++i)
        newSamplerVoices.emplace_back(new MPESamplerVoice(loadedSamplerSound, dataModel.samplerVoiceSettings()));

    if (fact == nullptr)
    {
        commands.push(SetSampleCommand(move(fact),
            nullptr,
            move(newSamplerVoices)));
    }
    else if (auto reader = fact->make(formatManager))
    {
        commands.push(SetSampleCommand(move(fact),
            std::unique_ptr<Sample>(new Sample(*reader, 10.0)),
            move(newSamplerVoices)));
    }
}

void SamplerAudioProcessor::setSample(std::vector<std::vector<float>> soundData, double sampleRate) {
    
    while (synthesiser.getNumVoices())
    {
        synthesiser.removeVoice(0);
    }

    auto sound = samplerSound;
    auto sample = std::unique_ptr<Sample>(new Sample(soundData, sampleRate));
    auto lengthInSeconds = sample->getLength() / sample->getSampleRate();
    sound->setLoopPointsInSeconds({ lengthInSeconds * 0.1, lengthInSeconds * 0.9 });
    sound->setSample(move(sample));

    // Start with the max number of voices
    for (auto i = 0; i != maxVoices; ++i) {
        synthesiser.addVoice(new MPESamplerVoice(sound, dataModel.samplerVoiceSettings()));
    }

}

void SamplerAudioProcessor::setCentreFrequency(double centreFrequency)
{
    commands.push([centreFrequency](SamplerAudioProcessor& proc)
        {
            auto loaded = proc.samplerSound;
            if (loaded != nullptr)
                loaded->setCentreFrequencyInHz(centreFrequency);
        });
}

void SamplerAudioProcessor::setLoopMode(LoopMode loopMode)
{
    commands.push([loopMode](SamplerAudioProcessor& proc)
        {
            auto loaded = proc.samplerSound;
            if (loaded != nullptr)
                loaded->setLoopMode(loopMode);
        });
}

void SamplerAudioProcessor::setLoopPoints(Range<double> loopPoints)
{
    commands.push([loopPoints](SamplerAudioProcessor& proc)
        {
            auto loaded = proc.samplerSound;
            if (loaded != nullptr)
                loaded->setLoopPointsInSeconds(loopPoints);
        });
}

void SamplerAudioProcessor::setMPEZoneLayout(MPEZoneLayout layout)
{
    commands.push([layout](SamplerAudioProcessor& proc)
        {
            // setZoneLayout will lock internally, so we don't care too much about
            // ensuring that the layout doesn't get copied or destroyed on the
            // audio thread. If the audio glitches while updating midi settings
            // it doesn't matter too much.
            proc.synthesiser.setZoneLayout(layout);
        });
}

void SamplerAudioProcessor::setLegacyModeEnabled(int pitchbendRange, Range<int> channelRange)
{
    commands.push([pitchbendRange, channelRange](SamplerAudioProcessor& proc)
        {
            proc.synthesiser.enableLegacyMode(pitchbendRange, channelRange);
        });
}

void SamplerAudioProcessor::setVoiceStealingEnabled(bool voiceStealingEnabled)
{
    commands.push([voiceStealingEnabled](SamplerAudioProcessor& proc)
        {
            proc.synthesiser.setVoiceStealingEnabled(voiceStealingEnabled);
        });
}

void SamplerAudioProcessor::setNumberOfVoices(int numberOfVoices)
{
    // We don't want to call 'new' on the audio thread. Normally, we'd
    // construct things here, on the GUI thread, and then move them into the
    // command lambda. Unfortunately, C++11 doesn't have extended lambda
    // capture, so we use a custom struct instead.

    class SetNumVoicesCommand
    {
    public:
        SetNumVoicesCommand(std::vector<std::unique_ptr<MPESamplerVoice>> newVoicesIn)
            : newVoices(std::move(newVoicesIn))
        {}

        void operator() (SamplerAudioProcessor& proc)
        {
            if ((int)newVoices.size() < proc.synthesiser.getNumVoices())
                proc.synthesiser.reduceNumVoices(int(newVoices.size()));
            else
                for (auto it = begin(newVoices); (size_t)proc.synthesiser.getNumVoices() < newVoices.size(); ++it)
                    proc.synthesiser.addVoice(it->release());
        }

    private:
        std::vector<std::unique_ptr<MPESamplerVoice>> newVoices;
    };

    numberOfVoices = min((int)maxVoices, numberOfVoices);
    auto loadedSamplerSound = samplerSound;
    std::vector<std::unique_ptr<MPESamplerVoice>> newSamplerVoices;
    newSamplerVoices.reserve((size_t)numberOfVoices);

    for (auto i = 0; i != numberOfVoices; ++i)
        newSamplerVoices.emplace_back(new MPESamplerVoice(loadedSamplerSound, dataModel.samplerVoiceSettings()));

    commands.push(SetNumVoicesCommand(move(newSamplerVoices)));
}

// These accessors are just for an 'overview' and won't give the exact
// state of the audio engine at a particular point in time.
// If you call getNumVoices(), get the result '10', and then call
// getPlaybackPosiiton(9), there's a chance the audio engine will have
// been updated to remove some voices in the meantime, so the returned
// value won't correspond to an existing voice.
int SamplerAudioProcessor::getNumVoices() const { return synthesiser.getNumVoices(); }
float SamplerAudioProcessor::getPlaybackPosition(int voice) const { return playbackPositions.at((size_t)voice); }

// todo: more stuff here
void SamplerAudioProcessor::parameterChanged(const String& parameterID, float newValue)
{
    if (parameterID == String("Choke ON/OFF"))
    {
        //isChokeGroupActive = static_cast<bool> (newValue);

        //For Debugging Purposes
        /*String value;
        value = (isChokeGroupActive == true) ? "true" : "false";
        Logger::outputDebugString(value);*/
    }
}

//==============================================================================
template <typename Element>
void SamplerAudioProcessor::process(AudioBuffer<Element>& buffer, MidiBuffer& midiMessages)
{
    // Try to acquire a lock on the command queue.
    // If we were successful, we pop all pending commands off the queue and
    // apply them to the processor.
    // If we weren't able to acquire the lock, it's because someone called
    // createEditor, which requires that the processor data model stays in
    // a valid state for the duration of the call.
    const GenericScopedTryLock<SpinLock> lock(commandQueueMutex);

    if (lock.isLocked())
        commands.call(*this);

    synthesiser.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    auto loadedSamplerSound = samplerSound;

    if (loadedSamplerSound->getSample() == nullptr)
        return;

    auto numVoices = synthesiser.getNumVoices();

    // Update the current playback positions
    for (auto i = 0; i < maxVoices; ++i)
    {
        auto* voicePtr = dynamic_cast<MPESamplerVoice*> (synthesiser.getVoice(i));

        if (i < numVoices && voicePtr != nullptr)
            playbackPositions[(size_t)i] = static_cast<float> (voicePtr->getCurrentSamplePosition() / loadedSamplerSound->getSample()->getSampleRate());
        else
            playbackPositions[(size_t)i] = 0.0f;
    }

}