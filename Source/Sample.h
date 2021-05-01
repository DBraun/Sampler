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

//==============================================================================
// Represents the constant parts of an audio sample: its name, sample rate,
// length, and the audio sample data itself.
// Samples might be pretty big, so we'll keep shared_ptrs to them most of the
// time, to reduce duplication and copying.
class Sample final
{
public:
    Sample(AudioFormatReader& source, double maxSampleLengthSecs)
        : m_sourceSampleRate(source.sampleRate),
        m_length(jmin(int(source.lengthInSamples),
            int(maxSampleLengthSecs* m_sourceSampleRate))),
        m_temp_data(jmin(2, int(source.numChannels)), m_length + 4)
    {
        if (m_length == 0)
            throw std::runtime_error("Unable to load sample");

        source.read(&m_temp_data, 0, m_length + 4, 0, true, true);

        upsample(8);
    }

    Sample(std::vector<std::vector<float>> soundData, double sr) : m_sourceSampleRate{ sr },
        m_length(soundData.at(0).size()) {

        int numChans = soundData.size();
        int numSamples = soundData.at(0).size();

        m_temp_data.setSize(soundData.size(), numSamples, false, true, false);

        for (int chan = 0; chan < numChans; chan++) {
            m_temp_data.copyFrom(chan, 0, soundData.at(chan).data(), m_length);
        }

        upsample(8);
    }

    double getSampleRate() const { return m_sourceSampleRate; }
    int getLength() const { return m_length; }
    const AudioBuffer<float>& getBuffer() const { return m_data; }

private:
    double m_sourceSampleRate;
    int m_length;
    AudioBuffer<float> m_temp_data;
    AudioBuffer<float> m_data;

    LagrangeInterpolator m_interpolator;

    // Whenever sample data is given to the Sample class, a Lagrange interpolator upsamples it in order
    // to be able to play it back at at different speeds with low aliasing artifacts. Therefore, upsample()
    // must be called in each constructor to Sample().
    void upsample(int upSampleRatio) {

        int numInputSamples = m_temp_data.getNumSamples();
        int numOutputSamples = upSampleRatio * numInputSamples;

        m_data.setSize(2, numOutputSamples, false, true, false);

        for (int outChan = 0; outChan < 2; outChan++) {
            int inChan = m_temp_data.getNumChannels() > 1 ? outChan : 0;
            m_interpolator.process(1., m_temp_data.getReadPointer(inChan), m_data.getWritePointer(outChan), numOutputSamples, numInputSamples, 0);
        }

        m_length *= upSampleRatio;

        m_temp_data.clear();
    }
};