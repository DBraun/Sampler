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

#include "MPESettingsDataModel.h"

MPESettingsDataModel::MPESettingsDataModel()
    : MPESettingsDataModel(ValueTree(IDs::MPE_SETTINGS))
{}

MPESettingsDataModel::MPESettingsDataModel(const ValueTree& vt)
    : valueTree(vt),
    synthVoices(valueTree, IDs::synthVoices, nullptr, 15),
    voiceStealingEnabled(valueTree, IDs::voiceStealingEnabled, nullptr, false),
    legacyModeEnabled(valueTree, IDs::legacyModeEnabled, nullptr, true),
    mpeZoneLayout(valueTree, IDs::mpeZoneLayout, nullptr, {}),
    legacyFirstChannel(valueTree, IDs::legacyFirstChannel, nullptr, 1),
    legacyLastChannel(valueTree, IDs::legacyLastChannel, nullptr, 15),
    legacyPitchbendRange(valueTree, IDs::legacyPitchbendRange, nullptr, 48)
{
    jassert(valueTree.hasType(IDs::MPE_SETTINGS));
    valueTree.addListener(this);
}

MPESettingsDataModel::MPESettingsDataModel(const MPESettingsDataModel& other)
    : MPESettingsDataModel(other.valueTree)
{}

int MPESettingsDataModel::getSynthVoices() const
{
    return synthVoices;
}

void MPESettingsDataModel::setSynthVoices(int value, UndoManager* undoManager)
{
    synthVoices.setValue(Range<int>(1, 20).clipValue(value), undoManager);
}

bool MPESettingsDataModel::getVoiceStealingEnabled() const
{
    return voiceStealingEnabled;
}

void MPESettingsDataModel::setVoiceStealingEnabled(bool value, UndoManager* undoManager)
{
    voiceStealingEnabled.setValue(value, undoManager);
}

bool MPESettingsDataModel::getLegacyModeEnabled() const
{
    return legacyModeEnabled;
}

void MPESettingsDataModel::setLegacyModeEnabled(bool value, UndoManager* undoManager)
{
    legacyModeEnabled.setValue(value, undoManager);
}

MPEZoneLayout MPESettingsDataModel::getMPEZoneLayout() const
{
    return mpeZoneLayout;
}

void MPESettingsDataModel::setMPEZoneLayout(MPEZoneLayout value, UndoManager* undoManager)
{
    mpeZoneLayout.setValue(value, undoManager);
}

int MPESettingsDataModel::getLegacyFirstChannel() const
{
    return legacyFirstChannel;
}

void MPESettingsDataModel::setLegacyFirstChannel(int value, UndoManager* undoManager)
{
    legacyFirstChannel.setValue(Range<int>(1, legacyLastChannel).clipValue(value), undoManager);
}

int MPESettingsDataModel::getLegacyLastChannel() const
{
    return legacyLastChannel;
}

void MPESettingsDataModel::setLegacyLastChannel(int value, UndoManager* undoManager)
{
    legacyLastChannel.setValue(Range<int>(legacyFirstChannel, 15).clipValue(value), undoManager);
}

int MPESettingsDataModel::getLegacyPitchbendRange() const
{
    return legacyPitchbendRange;
}

void MPESettingsDataModel::setLegacyPitchbendRange(int value, UndoManager* undoManager)
{
    legacyPitchbendRange.setValue(Range<int>(0, 95).clipValue(value), undoManager);
}

void MPESettingsDataModel::addListener(Listener& listener)
{
    listenerList.add(&listener);
}

void MPESettingsDataModel::removeListener(Listener& listener)
{
    listenerList.remove(&listener);
}

void MPESettingsDataModel::swap(MPESettingsDataModel& other) noexcept
{
    using std::swap;
    swap(other.valueTree, valueTree);
}

void MPESettingsDataModel::valueTreePropertyChanged(ValueTree&, const Identifier& property)
{
    if (property == IDs::synthVoices)
    {
        synthVoices.forceUpdateOfCachedValue();
        listenerList.call([this](Listener& l) { l.synthVoicesChanged(synthVoices); });
    }
    else if (property == IDs::voiceStealingEnabled)
    {
        voiceStealingEnabled.forceUpdateOfCachedValue();
        listenerList.call([this](Listener& l) { l.voiceStealingEnabledChanged(voiceStealingEnabled); });
    }
    else if (property == IDs::legacyModeEnabled)
    {
        legacyModeEnabled.forceUpdateOfCachedValue();
        listenerList.call([this](Listener& l) { l.legacyModeEnabledChanged(legacyModeEnabled); });
    }
    else if (property == IDs::mpeZoneLayout)
    {
        mpeZoneLayout.forceUpdateOfCachedValue();
        listenerList.call([this](Listener& l) { l.mpeZoneLayoutChanged(mpeZoneLayout); });
    }
    else if (property == IDs::legacyFirstChannel)
    {
        legacyFirstChannel.forceUpdateOfCachedValue();
        listenerList.call([this](Listener& l) { l.legacyFirstChannelChanged(legacyFirstChannel); });
    }
    else if (property == IDs::legacyLastChannel)
    {
        legacyLastChannel.forceUpdateOfCachedValue();
        listenerList.call([this](Listener& l) { l.legacyLastChannelChanged(legacyLastChannel); });
    }
    else if (property == IDs::legacyPitchbendRange)
    {
        legacyPitchbendRange.forceUpdateOfCachedValue();
        listenerList.call([this](Listener& l) { l.legacyPitchbendRangeChanged(legacyPitchbendRange); });
    }
}

void MPESettingsDataModel::valueTreeChildAdded(ValueTree&, ValueTree&)        { jassertfalse; }
void MPESettingsDataModel::valueTreeChildRemoved(ValueTree&, ValueTree&, int) { jassertfalse; }
void MPESettingsDataModel::valueTreeChildOrderChanged(ValueTree&, int, int)   { jassertfalse; }
void MPESettingsDataModel::valueTreeParentChanged(ValueTree&)                 { jassertfalse; }


