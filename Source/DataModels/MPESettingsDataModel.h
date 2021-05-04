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

#include "../Misc.h"

class MPESettingsDataModel : private ValueTree::Listener
{
public:
    class Listener
    {
    public:
        virtual ~Listener() noexcept = default;
        virtual void synthVoicesChanged(int) {}
        virtual void voiceStealingEnabledChanged(bool) {}
        virtual void legacyModeEnabledChanged(bool) {}
        virtual void mpeZoneLayoutChanged(const MPEZoneLayout&) {}
        virtual void legacyFirstChannelChanged(int) {}
        virtual void legacyLastChannelChanged(int) {}
        virtual void legacyPitchbendRangeChanged(int) {}
    };

    MPESettingsDataModel();

    explicit MPESettingsDataModel(const ValueTree& vt);

    MPESettingsDataModel(const MPESettingsDataModel& other);

    MPESettingsDataModel& operator= (const MPESettingsDataModel& other);

    int getSynthVoices() const;

    void setSynthVoices(int value, UndoManager* undoManager);

    bool getVoiceStealingEnabled() const;

    void setVoiceStealingEnabled(bool value, UndoManager* undoManager);

    bool getLegacyModeEnabled() const;

    void setLegacyModeEnabled(bool value, UndoManager* undoManager);

    MPEZoneLayout getMPEZoneLayout() const;

    void setMPEZoneLayout(MPEZoneLayout value, UndoManager* undoManager);

    int getLegacyFirstChannel() const;

    void setLegacyFirstChannel(int value, UndoManager* undoManager);

    int getLegacyLastChannel() const;

    void setLegacyLastChannel(int value, UndoManager* undoManager);

    int getLegacyPitchbendRange() const;

    void setLegacyPitchbendRange(int value, UndoManager* undoManager);

    void addListener(Listener& listener);

    void removeListener(Listener& listener);

    void swap(MPESettingsDataModel& other) noexcept;

private:
    void valueTreePropertyChanged(ValueTree&, const Identifier& property) override;

    void valueTreeChildAdded(ValueTree&, ValueTree&)      override;
    void valueTreeChildRemoved(ValueTree&, ValueTree&, int) override;
    void valueTreeChildOrderChanged(ValueTree&, int, int)        override;
    void valueTreeParentChanged(ValueTree&)                  override;

    ValueTree valueTree;

    CachedValue<int> synthVoices;
    CachedValue<bool> voiceStealingEnabled;
    CachedValue<bool> legacyModeEnabled;
    CachedValue<MPEZoneLayout> mpeZoneLayout;
    CachedValue<int> legacyFirstChannel;
    CachedValue<int> legacyLastChannel;
    CachedValue<int> legacyPitchbendRange;

    ListenerList<Listener> listenerList;
};