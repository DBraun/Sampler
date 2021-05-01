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

class MPESamplerVoiceDataModel : private ValueTree::Listener
{
public:
    class Listener
    {
    public:
        virtual ~Listener() noexcept = default;

        virtual void ampEnvAttackChanged(double) {}
        virtual void ampEnvDecayChanged(double) {}
        virtual void ampEnvSustainChanged(double) {}
        virtual void ampEnvReleaseChanged(double) {}

        virtual void filterEnvAttackChanged(double) {}
        virtual void filterEnvDecayChanged(double) {}
        virtual void filterEnvSustainChanged(double) {}
        virtual void filterEnvReleaseChanged(double) {}
        virtual void filterEnvModAmtChanged(double) {}
    };

    explicit MPESamplerVoiceDataModel();

    MPESamplerVoiceDataModel(const ValueTree& vt);

    MPESamplerVoiceDataModel(const MPESamplerVoiceDataModel& other);

    MPESamplerVoiceDataModel& operator= (const MPESamplerVoiceDataModel& other);

    void setAmpEnvAttack(double value, UndoManager* undoManager);
    void setAmpEnvDecay(double value, UndoManager* undoManager);
    void setAmpEnvSustain(double value, UndoManager* undoManager);
    void setAmpEnvRelease(double value, UndoManager* undoManager);

    void setFilterEnvAttack(double value, UndoManager* undoManager);
    void setFilterEnvDecay(double value, UndoManager* undoManager);
    void setFilterEnvSustain(double value, UndoManager* undoManager);
    void setFilterEnvRelease(double value, UndoManager* undoManager);
    void setFilterEnvModAmt(double value, UndoManager* undoManager);

    void addListener(Listener& listener);

    void removeListener(Listener& listener);

    void swap(MPESamplerVoiceDataModel& other) noexcept;

private:
    void valueTreePropertyChanged(ValueTree&, const Identifier& property) override;

    void valueTreeChildAdded(ValueTree&, ValueTree&)      override;
    void valueTreeChildRemoved(ValueTree&, ValueTree&, int) override;
    void valueTreeChildOrderChanged(ValueTree&, int, int)        override;
    void valueTreeParentChanged(ValueTree&)                  override;

    ValueTree valueTree;

    CachedValue<double> ampEnvAttack;
    CachedValue<double> ampEnvDecay;
    CachedValue<double> ampEnvSustain;
    CachedValue<double> ampEnvRelease;

    CachedValue<double> filterEnvAttack;
    CachedValue<double> filterEnvDecay;
    CachedValue<double> filterEnvSustain;
    CachedValue<double> filterEnvRelease;
    CachedValue<double> filterEnvModAmt;

    ListenerList<Listener> listenerList;
};