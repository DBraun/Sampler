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

#include "MPESamplerSound.h"
#include "MPESamplerVoiceDataModel.h"

class MPESamplerVoice : public MPESynthesiserVoice,
    private MPESamplerVoiceDataModel::Listener
{
public:
    explicit MPESamplerVoice(std::shared_ptr<const MPESamplerSound> sound, const MPESamplerVoiceDataModel& model)
        : samplerSound(std::move(sound)), dataModel(model)
    {
        jassert(samplerSound != nullptr);

        m_Buffer.setSize(2, 1, false, true, false);

        // todo: adsr.setParameters;
        // todo: filterEnv.setParameters;
        dataModel.addListener(*this);

        setFilterActive(true);

        setCurrentPlaybackSampleRate(44100.); // todo: this needs to be dynamic

    }

    void setCurrentPlaybackSampleRate(double newRate) {
        m_sampleRate = newRate;
        ampEnv.setSampleRate(newRate);
        filterEnv.setSampleRate(newRate);

        int numChannels = 2;
        int samplesPerBlock = 1;
        juce::dsp::ProcessSpec spec{ newRate, static_cast<juce::uint32> (samplesPerBlock), static_cast<juce::uint32> (numChannels) };
        m_Filter.prepare(spec); 
    }

    void noteStarted() override
    {
        jassert(currentlyPlayingNote.isValid());
        jassert(currentlyPlayingNote.keyState == MPENote::keyDown
            || currentlyPlayingNote.keyState == MPENote::keyDownAndSustained);


        // NB: The line below is commented out because of https://forum.juce.com/t/samplerplugin-demo-no-audio/32584
        //level    .setTargetValue (currentlyPlayingNote.pressure.asUnsignedFloat());
        level.setTargetValue(currentlyPlayingNote.noteOnVelocity.asUnsignedFloat());
        frequency.setTargetValue(currentlyPlayingNote.getFrequencyInHertz());

        auto loopPoints = samplerSound->getLoopPointsInSeconds();
        loopBegin.setTargetValue(loopPoints.getStart() * samplerSound->getSample()->getSampleRate());
        loopEnd.setTargetValue(loopPoints.getEnd() * samplerSound->getSample()->getSampleRate());

        for (auto smoothed : { &level, &frequency, &loopBegin, &loopEnd })
            smoothed->reset(currentSampleRate, smoothingLengthInSeconds);

        currentSamplePos = 0.0;
        tailOff = 0.0;

        // todo: should these be here?
        ampEnv.noteOn();
        filterEnv.noteOn();
    }

    void noteStopped(bool allowTailOff) override
    {
        jassert(currentlyPlayingNote.keyState == MPENote::off);

        // todo: should these be here?
        ampEnv.noteOff();
        filterEnv.noteOff();

        if (allowTailOff && tailOff == 0.0)
            tailOff = 1.0;
        else
            stopNote();
    }

    void notePressureChanged() override
    {
        level.setTargetValue(currentlyPlayingNote.pressure.asUnsignedFloat());
    }

    void notePitchbendChanged() override
    {
        frequency.setTargetValue(currentlyPlayingNote.getFrequencyInHertz());
    }

    void noteTimbreChanged()   override {}
    void noteKeyStateChanged() override {}

    void renderNextBlock(AudioBuffer<float>& outputBuffer,
        int startSample,
        int numSamples) override
    {
        render(outputBuffer, startSample, numSamples);
    }

    void renderNextBlock(AudioBuffer<double>& outputBuffer,
        int startSample,
        int numSamples) override
    {
        render(outputBuffer, startSample, numSamples);
    }

    double getCurrentSamplePosition() const
    {
        return currentSamplePos;
    }

    void sampleReaderChanged(std::shared_ptr<AudioFormatReaderFactory>) {}
    void centreFrequencyHzChanged(double) {}
    void loopModeChanged(LoopMode) {}
    void loopPointsSecondsChanged(Range<double>) {}

    void ampEnvAttackChanged(double newValue) {
        auto params = ampEnv.getParameters();
        params.attack = newValue;
        ampEnv.setParameters(params);
    }
    void ampEnvDecayChanged(double newValue) {
        auto params = ampEnv.getParameters();
        params.decay = newValue;
        ampEnv.setParameters(params);
    }
    void ampEnvSustainChanged(double newValue) {
        auto params = ampEnv.getParameters();
        params.sustain = newValue;
        ampEnv.setParameters(params);
    }
    void ampEnvReleaseChanged(double newValue) {
        auto params = ampEnv.getParameters();
        params.release = newValue;
        ampEnv.setParameters(params);
    }

    void filterEnvAttackChanged(double newValue) {
        auto params = filterEnv.getParameters();
        params.attack = newValue;
        filterEnv.setParameters(params);
    }
    void filterEnvDecayChanged(double newValue) {
        auto params = filterEnv.getParameters();
        params.decay = newValue;
        filterEnv.setParameters(params);
    }
    void filterEnvSustainChanged(double newValue) {
        auto params = filterEnv.getParameters();
        params.sustain = newValue;
        filterEnv.setParameters(params);
    }
    void filterEnvReleaseChanged(double newValue) {
        auto params = filterEnv.getParameters();
        params.release = newValue;
        filterEnv.setParameters(params);
    }
    void filterEnvModAmtChanged(double newValue) {
        // todo: smooth this?
        filterCutoffModAmt = newValue;
    }

    void setFilterActive(bool activeState) {
        // todo:
    }


private:
    template <typename Element>
    void render(AudioBuffer<Element>& outputBuffer, int startSample, int numSamples)
    {
        jassert(samplerSound->getSample() != nullptr);

        auto loopPoints = samplerSound->getLoopPointsInSeconds();
        loopBegin.setTargetValue(loopPoints.getStart() * samplerSound->getSample()->getSampleRate());
        loopEnd.setTargetValue(loopPoints.getEnd() * samplerSound->getSample()->getSampleRate());

        auto& data = samplerSound->getSample()->getBuffer();

        auto inL = data.getReadPointer(0);
        auto inR = data.getNumChannels() > 1 ? data.getReadPointer(1) : nullptr;

        auto outL = outputBuffer.getWritePointer(0, startSample);

        if (outL == nullptr)
            return;

        auto outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer(1, startSample)
            : nullptr;

        size_t writePos = 0;

        while (--numSamples >= 0 && renderNextSample(inL, inR, outL, outR, writePos))
            writePos += 1;
    }

    template <typename Element>
    bool renderNextSample(const float* inL,
        const float* inR,
        Element* outL,
        Element* outR,
        size_t writePos)
    {
        auto currentLevel = level.getNextValue();
        auto currentFrequency = frequency.getNextValue();
        auto currentLoopBegin = loopBegin.getNextValue();
        auto currentLoopEnd = loopEnd.getNextValue();

        if (isTailingOff())
        {
            currentLevel *= tailOff;
            tailOff *= 0.9999;

            if (tailOff < 0.005)
            {
                stopNote();
                return false;
            }
        }

        auto pos = (int)currentSamplePos;
        auto nextPos = pos + 1;
        auto alpha = (Element)(currentSamplePos - pos);
        auto invAlpha = 1.0f - alpha;

        // just using a very simple linear interpolation here..
        // todo: use Catmull interpolation or Lagrange Interpolation.
        auto l = static_cast<Element> (currentLevel * (inL[pos] * invAlpha + inL[nextPos] * alpha));
        auto r = static_cast<Element> ((inR != nullptr) ? currentLevel * (inR[pos] * invAlpha + inR[nextPos] * alpha)
            : l);

        m_Buffer.setSample(0, 0, l);
        m_Buffer.setSample(1, 0, r);

        // apply amplitude
        m_Buffer.applyGain(ampEnv.getNextSample());

        float cutoff = filterCutoff + filterCutoffModAmt*filterEnv.getNextSample();
        cutoff = fmax(40., fmin(20000., cutoff));

        float q_val = 0.7;
        *m_Filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(m_sampleRate, cutoff, q_val);

        // apply low pass filter
        juce::dsp::AudioBlock<float> block(m_Buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        m_Filter.process(context);

        if (outR != nullptr)
        {
            outL[writePos] += m_Buffer.getSample(0, 0);
            outR[writePos] += m_Buffer.getSample(1, 0);
        }
        else
        {
            outL[writePos] += (m_Buffer.getSample(0, 0) + m_Buffer.getSample(1, 0)) * 0.5f;
        }

        std::tie(currentSamplePos, currentDirection) = getNextState(currentFrequency,
            currentLoopBegin,
            currentLoopEnd);

        if (currentSamplePos > samplerSound->getSample()->getLength())
        {
            stopNote();
            return false;
        }

        return true;
    }

    double getSampleValue() const;

    bool isTailingOff() const
    {
        return tailOff != 0.0;
    }

    void stopNote()
    {

        // todo: are these necessary?
        if (ampEnv.isActive()) {
            ampEnv.reset();
        }
        if (filterEnv.isActive()) {
            filterEnv.reset();
        }

        m_Filter.reset();

        clearCurrentNote();
        currentSamplePos = 0.0;
    }

    enum class Direction
    {
        forward,
        backward
    };

    std::tuple<double, Direction> getNextState(double freq,
        double begin,
        double end) const
    {
        auto nextPitchRatio = freq / samplerSound->getCentreFrequencyInHz();

        auto nextSamplePos = currentSamplePos;
        auto nextDirection = currentDirection;

        // Move the current sample pos in the correct direction
        switch (currentDirection)
        {
        case Direction::forward:
            nextSamplePos += nextPitchRatio;
            break;

        case Direction::backward:
            nextSamplePos -= nextPitchRatio;
            break;

        default:
            break;
        }

        // Update current sample position, taking loop mode into account
        // If the loop mode was changed while we were travelling backwards, deal
        // with it gracefully.
        if (nextDirection == Direction::backward && nextSamplePos < begin)
        {
            nextSamplePos = begin;
            nextDirection = Direction::forward;

            return std::tuple<double, Direction>(nextSamplePos, nextDirection);
        }

        if (samplerSound->getLoopMode() == LoopMode::none)
            return std::tuple<double, Direction>(nextSamplePos, nextDirection);

        if (nextDirection == Direction::forward && end < nextSamplePos && !isTailingOff())
        {
            if (samplerSound->getLoopMode() == LoopMode::forward)
                nextSamplePos = begin;
            else if (samplerSound->getLoopMode() == LoopMode::pingpong)
            {
                nextSamplePos = end;
                nextDirection = Direction::backward;
            }
        }
        return std::tuple<double, Direction>(nextSamplePos, nextDirection);
    }

    std::shared_ptr<const MPESamplerSound> samplerSound;
    SmoothedValue<double> level{ 0 };
    SmoothedValue<double> frequency{ 0 };
    SmoothedValue<double> loopBegin;
    SmoothedValue<double> loopEnd;
    double currentSamplePos{ 0 };
    double tailOff{ 0 };
    Direction currentDirection{ Direction::forward };
    double smoothingLengthInSeconds{ 0.01 };

    MPESamplerVoiceDataModel dataModel;
    double m_sampleRate;

    ADSR ampEnv;

    ADSR filterEnv;
    double filterCutoff = 1000.;
    double filterCutoffModAmt = 8000.;

    dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> m_Filter;
    AudioBuffer<float> m_Buffer;
};