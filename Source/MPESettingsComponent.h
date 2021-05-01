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

#include "MPELegacySettingsComponent.h"
#include "MPENewSettingsComponent.h"

//==============================================================================
class MPESettingsComponent final : public Component,
    private MPESettingsDataModel::Listener
{
public:
    MPESettingsComponent(const MPESettingsDataModel& model,
        UndoManager& um)
        : dataModel(model),
        legacySettings(dataModel, um),
        newSettings(dataModel, um),
        undoManager(&um)
    {
        dataModel.addListener(*this);

        addAndMakeVisible(newSettings);
        addChildComponent(legacySettings);

        initialiseComboBoxWithConsecutiveIntegers(*this, numberOfVoices, numberOfVoicesLabel, 1, 20, 15);
        numberOfVoices.onChange = [this]
        {
            undoManager->beginNewTransaction();
            dataModel.setSynthVoices(numberOfVoices.getText().getIntValue(), undoManager);
        };

        for (auto& button : { &legacyModeEnabledToggle, &voiceStealingEnabledToggle })
        {
            addAndMakeVisible(button);
        }

        legacyModeEnabledToggle.onClick = [this]
        {
            undoManager->beginNewTransaction();
            dataModel.setLegacyModeEnabled(legacyModeEnabledToggle.getToggleState(), undoManager);
        };

        voiceStealingEnabledToggle.onClick = [this]
        {
            undoManager->beginNewTransaction();
            dataModel.setVoiceStealingEnabled(voiceStealingEnabledToggle.getToggleState(), undoManager);
        };
    }

private:
    void resized() override
    {
        auto topHeight = jmax(legacySettings.getMinHeight(), newSettings.getMinHeight());
        auto r = getLocalBounds();
        r.removeFromTop(15);
        auto top = r.removeFromTop(topHeight);
        legacySettings.setBounds(top);
        newSettings.setBounds(top);

        r.removeFromLeft(proportionOfWidth(0.65f));
        r = r.removeFromLeft(proportionOfWidth(0.25f));

        auto toggleLeft = proportionOfWidth(0.25f);

        legacyModeEnabledToggle.setBounds(r.removeFromTop(controlHeight).withLeft(toggleLeft));
        r.removeFromTop(controlSeparation);
        voiceStealingEnabledToggle.setBounds(r.removeFromTop(controlHeight).withLeft(toggleLeft));
        r.removeFromTop(controlSeparation);
        numberOfVoices.setBounds(r.removeFromTop(controlHeight));
    }

    void legacyModeEnabledChanged(bool value) override
    {
        legacySettings.setVisible(value);
        newSettings.setVisible(!value);
        legacyModeEnabledToggle.setToggleState(value, dontSendNotification);
    }

    void voiceStealingEnabledChanged(bool value) override
    {
        voiceStealingEnabledToggle.setToggleState(value, dontSendNotification);
    }

    void synthVoicesChanged(int value) override
    {
        numberOfVoices.setSelectedId(value, dontSendNotification);
    }

    MPESettingsDataModel dataModel;
    MPELegacySettingsComponent legacySettings;
    MPENewSettingsComponent newSettings;

    ToggleButton legacyModeEnabledToggle{ "Enable Legacy Mode" },
        voiceStealingEnabledToggle{ "Enable synth voice stealing" };

    ComboBox numberOfVoices;
    Label numberOfVoicesLabel{ {}, "Number of synth voices" };

    UndoManager* undoManager;
};