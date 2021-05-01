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

#include "MPESamplerVoiceDataModel.h"

MPESamplerVoiceDataModel::MPESamplerVoiceDataModel()
    : MPESamplerVoiceDataModel(ValueTree(IDs::samplerVoice))
{}

MPESamplerVoiceDataModel::MPESamplerVoiceDataModel(const ValueTree& vt)
    : valueTree(vt),

    ampEnvAttack(valueTree, IDs::ampEnvAttack, nullptr),
    ampEnvDecay(valueTree, IDs::ampEnvDecay, nullptr),
    ampEnvSustain(valueTree, IDs::ampEnvSustain, nullptr),
    ampEnvRelease(valueTree, IDs::ampEnvRelease, nullptr),

    filterEnvAttack(valueTree, IDs::filterEnvAttack, nullptr),
    filterEnvDecay(valueTree, IDs::filterEnvDecay, nullptr),
    filterEnvSustain(valueTree, IDs::filterEnvSustain, nullptr),
    filterEnvRelease(valueTree, IDs::filterEnvRelease, nullptr),
    filterEnvModAmt(valueTree, IDs::filterEnvModAmt, nullptr)
{
    jassert(valueTree.hasType(IDs::samplerVoice));
    valueTree.addListener(this);
}

MPESamplerVoiceDataModel::MPESamplerVoiceDataModel(const MPESamplerVoiceDataModel& other)
    : MPESamplerVoiceDataModel(other.valueTree)
{}

MPESamplerVoiceDataModel& MPESamplerVoiceDataModel::operator= (const MPESamplerVoiceDataModel& other)
{
    auto copy(other);
    swap(copy);
    return *this;
}

void
MPESamplerVoiceDataModel::setAmpEnvAttack(double value, UndoManager* undoManager) {
    ampEnvAttack.setValue(Range<double>(0., 500.).clipValue(value), undoManager);
}

void
MPESamplerVoiceDataModel::setAmpEnvDecay(double value, UndoManager* undoManager) {
    ampEnvDecay.setValue(Range<double>(0., 500.).clipValue(value), undoManager);
}

void
MPESamplerVoiceDataModel::setAmpEnvSustain(double value, UndoManager* undoManager) {
    ampEnvSustain.setValue(Range<double>(0., 1.).clipValue(value), undoManager);
}

void
MPESamplerVoiceDataModel::setAmpEnvRelease(double value, UndoManager* undoManager) {
    ampEnvRelease.setValue(Range<double>(0., 500.).clipValue(value), undoManager);
}

void
MPESamplerVoiceDataModel::setFilterEnvAttack(double value, UndoManager* undoManager) {
    filterEnvAttack.setValue(Range<double>(0., 500.).clipValue(value), undoManager);
}

void
MPESamplerVoiceDataModel::setFilterEnvDecay(double value, UndoManager* undoManager) {
    filterEnvDecay.setValue(Range<double>(0., 500.).clipValue(value), undoManager);
}

void
MPESamplerVoiceDataModel::setFilterEnvSustain(double value, UndoManager* undoManager) {
    filterEnvSustain.setValue(Range<double>(0., 1.).clipValue(value), undoManager);
}

void
MPESamplerVoiceDataModel::setFilterEnvRelease(double value, UndoManager* undoManager) {
    filterEnvRelease.setValue(Range<double>(0., 500.).clipValue(value), undoManager);
}

void
MPESamplerVoiceDataModel::setFilterEnvModAmt(double value, UndoManager* undoManager) {
    filterEnvModAmt.setValue(Range<double>(-20000., 20000.).clipValue(value), undoManager);
}

void MPESamplerVoiceDataModel::addListener(Listener& listener)
{
    listenerList.add(&listener);
}

void MPESamplerVoiceDataModel::removeListener(Listener& listener)
{
    listenerList.remove(&listener);
}

void MPESamplerVoiceDataModel::swap(MPESamplerVoiceDataModel& other) noexcept
{
    using std::swap;
    swap(other.valueTree, valueTree);
}

void MPESamplerVoiceDataModel::valueTreePropertyChanged(ValueTree&, const Identifier& property)
{
    if (property == IDs::ampEnvAttack) {
        ampEnvAttack.forceUpdateOfCachedValue();
        listenerList.call([this](Listener& l) { l.ampEnvAttackChanged(ampEnvAttack); });
    }
    else if (property == IDs::ampEnvDecay) {
        ampEnvDecay.forceUpdateOfCachedValue();
        listenerList.call([this](Listener& l) { l.ampEnvDecayChanged(ampEnvDecay); });
    }
    else if (property == IDs::ampEnvSustain) {
        ampEnvSustain.forceUpdateOfCachedValue();
        listenerList.call([this](Listener& l) { l.ampEnvSustainChanged(ampEnvSustain); });
    }
    else if (property == IDs::ampEnvRelease) {
        ampEnvRelease.forceUpdateOfCachedValue();
        listenerList.call([this](Listener& l) { l.ampEnvReleaseChanged(ampEnvRelease); });
    }

    else if (property == IDs::filterEnvAttack) {
        filterEnvAttack.forceUpdateOfCachedValue();
        listenerList.call([this](Listener& l) { l.filterEnvAttackChanged(filterEnvAttack); });
    }
    else if (property == IDs::filterEnvDecay) {
        filterEnvDecay.forceUpdateOfCachedValue();
        listenerList.call([this](Listener& l) { l.filterEnvDecayChanged(filterEnvDecay); });
    }
    else if (property == IDs::filterEnvSustain) {
        filterEnvSustain.forceUpdateOfCachedValue();
        listenerList.call([this](Listener& l) { l.filterEnvSustainChanged(filterEnvSustain); });
    }
    else if (property == IDs::filterEnvRelease) {
        filterEnvRelease.forceUpdateOfCachedValue();
        listenerList.call([this](Listener& l) { l.filterEnvReleaseChanged(filterEnvRelease); });
    }

    else if (property == IDs::filterEnvModAmt) {
        filterEnvModAmt.forceUpdateOfCachedValue();
        listenerList.call([this](Listener& l) { l.filterEnvModAmtChanged(filterEnvModAmt); });
    }
}

void MPESamplerVoiceDataModel::valueTreeChildAdded(ValueTree&, ValueTree&) {}
void MPESamplerVoiceDataModel::valueTreeChildRemoved(ValueTree&, ValueTree&, int) { jassertfalse; }
void MPESamplerVoiceDataModel::valueTreeChildOrderChanged(ValueTree&, int, int) { jassertfalse; }
void MPESamplerVoiceDataModel::valueTreeParentChanged(ValueTree&) { jassertfalse; }
