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

class MPENewSettingsComponent final : public juce::Component,
    private MPESettingsDataModel::Listener
{
public:
    MPENewSettingsComponent(const MPESettingsDataModel& model,
        UndoManager& um)
        : dataModel(model),
        undoManager(&um)
    {
        dataModel.addListener(*this);

        addAndMakeVisible(isLowerZoneButton);
        isLowerZoneButton.setToggleState(true, NotificationType::dontSendNotification);

        initialiseComboBoxWithConsecutiveIntegers(*this, memberChannels, memberChannelsLabel, 0, 16, 15);
        initialiseComboBoxWithConsecutiveIntegers(*this, masterPitchbendRange, masterPitchbendRangeLabel, 0, 96, 2);
        initialiseComboBoxWithConsecutiveIntegers(*this, notePitchbendRange, notePitchbendRangeLabel, 0, 96, 48);

        for (auto& button : { &setZoneButton, &clearAllZonesButton })
            addAndMakeVisible(button);

        setZoneButton.onClick = [this]
        {
            auto isLowerZone = isLowerZoneButton.getToggleState();
            auto numMemberChannels = memberChannels.getText().getIntValue();
            auto perNotePb = notePitchbendRange.getText().getIntValue();
            auto masterPb = masterPitchbendRange.getText().getIntValue();

            if (isLowerZone)
                zoneLayout.setLowerZone(numMemberChannels, perNotePb, masterPb);
            else
                zoneLayout.setUpperZone(numMemberChannels, perNotePb, masterPb);

            undoManager->beginNewTransaction();
            dataModel.setMPEZoneLayout(zoneLayout, undoManager);
        };

        clearAllZonesButton.onClick = [this]
        {
            zoneLayout.clearAllZones();
            undoManager->beginNewTransaction();
            dataModel.setMPEZoneLayout(zoneLayout, undoManager);
        };
    }

    int getMinHeight() const
    {
        return (controlHeight * 6) + (controlSeparation * 6);
    }

private:
    void resized() override
    {
        juce::Rectangle<int> r(proportionOfWidth(0.65f), 0, proportionOfWidth(0.25f), getHeight());

        isLowerZoneButton.setBounds(r.removeFromTop(controlHeight));
        r.removeFromTop(controlSeparation);

        for (auto& comboBox : { &memberChannels, &masterPitchbendRange, &notePitchbendRange })
        {
            comboBox->setBounds(r.removeFromTop(controlHeight));
            r.removeFromTop(controlSeparation);
        }

        r.removeFromTop(controlSeparation);

        auto buttonLeft = proportionOfWidth(0.5f);

        setZoneButton.setBounds(r.removeFromTop(controlHeight).withLeft(buttonLeft));
        r.removeFromTop(controlSeparation);
        clearAllZonesButton.setBounds(r.removeFromTop(controlHeight).withLeft(buttonLeft));
    }

    void mpeZoneLayoutChanged(const MPEZoneLayout& value) override
    {
        zoneLayout = value;
    }

    MPESettingsDataModel dataModel;
    MPEZoneLayout zoneLayout;

    ComboBox memberChannels, masterPitchbendRange, notePitchbendRange;

    ToggleButton isLowerZoneButton{ "Lower zone" };

    Label memberChannelsLabel{ {}, "Nr. of member channels" },
        masterPitchbendRangeLabel{ {}, "Master pitchbend range (semitones)" },
        notePitchbendRangeLabel{ {}, "Note pitchbend range (semitones)" };

    TextButton setZoneButton{ "Set zone" },
        clearAllZonesButton{ "Clear all zones" };

    UndoManager* undoManager;
};
