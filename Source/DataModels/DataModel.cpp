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

#include "DataModel.h"

DataModel::DataModel(AudioFormatManager& audioFormatManagerIn)
    : DataModel(audioFormatManagerIn, ValueTree(IDs::DATA_MODEL))
{}

DataModel::DataModel(AudioFormatManager& audioFormatManagerIn, const ValueTree& vt)
    : audioFormatManager(&audioFormatManagerIn),
    valueTree(vt),
    sampleReader(valueTree, IDs::sampleReader, nullptr),
    centreFrequencyHz(valueTree, IDs::centreFrequencyHz, nullptr),
    loopMode(valueTree, IDs::loopMode, nullptr, LoopMode::none),
    loopPointsSeconds(valueTree, IDs::loopPointsSeconds, nullptr)
{
    jassert(valueTree.hasType(IDs::DATA_MODEL));
    valueTree.addListener(this);
}

DataModel::DataModel(const DataModel& other)
    : DataModel(*other.audioFormatManager, other.valueTree)
{}

DataModel& DataModel::operator= (const DataModel& other)
{
    auto copy(other);
    swap(copy);
    return *this;
}

std::unique_ptr<AudioFormatReader> DataModel::getSampleReader() const
{
    return sampleReader != nullptr ? sampleReader.get()->make(*audioFormatManager) : nullptr;
}

void DataModel::setSampleReader(std::unique_ptr<AudioFormatReaderFactory> readerFactory,
    UndoManager* undoManager)
{
    sampleReader.setValue(move(readerFactory), undoManager);
    setLoopPointsSeconds(Range<double>(0, getSampleLengthSeconds()).constrainRange(loopPointsSeconds),
        undoManager);
}

double DataModel::getSampleLengthSeconds() const
{
    if (auto r = getSampleReader())
        return (double)r->lengthInSamples / r->sampleRate;

    return 1.0;
}

double DataModel::getCentreFrequencyHz() const
{
    return centreFrequencyHz;
}

void DataModel::setCentreFrequencyHz(double value, UndoManager* undoManager)
{
    centreFrequencyHz.setValue(Range<double>(2, 20000).clipValue(value),
        undoManager);
}

LoopMode DataModel::getLoopMode() const
{
    return loopMode;
}

void DataModel::setLoopMode(LoopMode value, UndoManager* undoManager)
{
    loopMode.setValue(value, undoManager);
}

Range<double> DataModel::getLoopPointsSeconds() const
{
    return loopPointsSeconds;
}

void DataModel::setLoopPointsSeconds(Range<double> value, UndoManager* undoManager)
{
    loopPointsSeconds.setValue(Range<double>(0, getSampleLengthSeconds()).constrainRange(value),
        undoManager);
}

MPESettingsDataModel DataModel::mpeSettings()
{
    return MPESettingsDataModel(valueTree.getOrCreateChildWithName(IDs::MPE_SETTINGS, nullptr));
}

void DataModel::addListener(Listener& listener)
{
    listenerList.add(&listener);
}

void DataModel::removeListener(Listener& listener)
{
    listenerList.remove(&listener);
}

void DataModel::swap(DataModel& other) noexcept
{
    using std::swap;
    swap(other.valueTree, valueTree);
}

AudioFormatManager& DataModel::getAudioFormatManager() const
{
    return *audioFormatManager;
}

void DataModel::valueTreePropertyChanged(ValueTree&, const Identifier& property)
{
    if (property == IDs::sampleReader)
    {
        sampleReader.forceUpdateOfCachedValue();
        listenerList.call([this](Listener& l) { l.sampleReaderChanged(sampleReader); });
    }
    else if (property == IDs::centreFrequencyHz)
    {
        centreFrequencyHz.forceUpdateOfCachedValue();
        listenerList.call([this](Listener& l) { l.centreFrequencyHzChanged(centreFrequencyHz); });
    }
    else if (property == IDs::loopMode)
    {
        loopMode.forceUpdateOfCachedValue();
        listenerList.call([this](Listener& l) { l.loopModeChanged(loopMode); });
    }
    else if (property == IDs::loopPointsSeconds)
    {
        loopPointsSeconds.forceUpdateOfCachedValue();
        listenerList.call([this](Listener& l) { l.loopPointsSecondsChanged(loopPointsSeconds); });
    }
}

void DataModel::valueTreeChildAdded(ValueTree&, ValueTree&)       {}
void DataModel::valueTreeChildRemoved(ValueTree&, ValueTree&, int)  { jassertfalse; }
void DataModel::valueTreeChildOrderChanged(ValueTree&, int, int)         { jassertfalse; }
void DataModel::valueTreeParentChanged(ValueTree&)                   { jassertfalse; }
