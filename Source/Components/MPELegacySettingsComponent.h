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

#include "../DataModels/VisibleRangeDataModel.h"

class MPELegacySettingsComponent final : public Component,
    private MPESettingsDataModel::Listener
{
public:
    explicit MPELegacySettingsComponent(const MPESettingsDataModel& model,
        UndoManager& um)
        : dataModel(model),
        undoManager(&um)
    {
        dataModel.addListener(*this);

        initialiseComboBoxWithConsecutiveIntegers(*this, legacyStartChannel, legacyStartChannelLabel, 1, 16, 1);
        initialiseComboBoxWithConsecutiveIntegers(*this, legacyEndChannel, legacyEndChannelLabel, 1, 16, 16);
        initialiseComboBoxWithConsecutiveIntegers(*this, legacyPitchbendRange, legacyPitchbendRangeLabel, 0, 96, 2);

        legacyStartChannel.onChange = [this]
        {
            if (isLegacyModeValid())
            {
                undoManager->beginNewTransaction();
                dataModel.setLegacyFirstChannel(getFirstChannel(), undoManager);
            }
        };

        legacyEndChannel.onChange = [this]
        {
            if (isLegacyModeValid())
            {
                undoManager->beginNewTransaction();
                dataModel.setLegacyLastChannel(getLastChannel(), undoManager);
            }
        };

        legacyPitchbendRange.onChange = [this]
        {
            if (isLegacyModeValid())
            {
                undoManager->beginNewTransaction();
                dataModel.setLegacyPitchbendRange(legacyPitchbendRange.getText().getIntValue(), undoManager);
            }
        };
    }

    int getMinHeight() const
    {
        return (controlHeight * 3) + (controlSeparation * 2);
    }

private:
    void resized() override
    {
        juce::Rectangle<int> r(proportionOfWidth(0.65f), 0, proportionOfWidth(0.25f), getHeight());

        for (auto& comboBox : { &legacyStartChannel, &legacyEndChannel, &legacyPitchbendRange })
        {
            comboBox->setBounds(r.removeFromTop(controlHeight));
            r.removeFromTop(controlSeparation);
        }
    }

    bool isLegacyModeValid() const
    {
        if (!areLegacyModeParametersValid())
        {
            handleInvalidLegacyModeParameters();
            return false;
        }

        return true;
    }

    void legacyFirstChannelChanged(int value) override
    {
        legacyStartChannel.setSelectedId(value, dontSendNotification);
    }

    void legacyLastChannelChanged(int value) override
    {
        legacyEndChannel.setSelectedId(value, dontSendNotification);
    }

    void legacyPitchbendRangeChanged(int value) override
    {
        legacyPitchbendRange.setSelectedId(value + 1, dontSendNotification);
    }

    int getFirstChannel() const
    {
        return legacyStartChannel.getText().getIntValue();
    }

    int getLastChannel() const
    {
        return legacyEndChannel.getText().getIntValue();
    }

    bool areLegacyModeParametersValid() const
    {
        return getFirstChannel() <= getLastChannel();
    }

    void handleInvalidLegacyModeParameters() const
    {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
            "Invalid legacy mode channel layout",
            "Cannot set legacy mode start/end channel:\n"
            "The end channel must not be less than the start channel!",
            "Got it");
    }

    MPESettingsDataModel dataModel;

    ComboBox legacyStartChannel, legacyEndChannel, legacyPitchbendRange;

    Label legacyStartChannelLabel{ {}, "First channel" },
        legacyEndChannelLabel{ {}, "Last channel" },
        legacyPitchbendRangeLabel{ {}, "Pitchbend range (semitones)" };

    UndoManager* undoManager;
};
