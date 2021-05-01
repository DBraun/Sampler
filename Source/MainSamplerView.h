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

#include "PlaybackPositionOverlay.h"
#include "WaveformEditor.h"
#include "MPESamplerVoiceDataModel.h"


class MainSamplerView : public Component,
    private DataModel::Listener,
    private MPESamplerVoiceDataModel::Listener,
    private ChangeListener
{
public:
    MainSamplerView(const DataModel& model,
        PlaybackPositionOverlay::Provider provider,
        UndoManager& um)
        : dataModel(model),
        samplerVoiceDataModel(dataModel.samplerVoiceSettings()),
        waveformEditor(dataModel, move(provider), um),
        undoManager(um)
    {
        dataModel.addListener(*this);

        addAndMakeVisible(waveformEditor);
        addAndMakeVisible(loadNewSampleButton);
        addAndMakeVisible(undoButton);
        addAndMakeVisible(redoButton);

        ampEnvAttackSlider.setSliderStyle(Slider::LinearVertical);
        ampEnvAttackSlider.setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
        ampEnvAttackSlider.setBounds(40, 120, 40, 136);
        ampEnvAttackSlider.setRange(0.0f, 500.0f);
        ampEnvAttackSlider.setValue(0.0f);
        addAndMakeVisible(ampEnvAttackSlider);

        ampEnvDecaySlider.setSliderStyle(Slider::LinearVertical);
        ampEnvDecaySlider.setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
        ampEnvDecaySlider.setBounds(88, 120, 40, 136);
        ampEnvDecaySlider.setRange(0.0f, 1000.0f);
        ampEnvDecaySlider.setValue(0.0f);
        addAndMakeVisible(ampEnvDecaySlider);

        ampEnvSustainSlider.setSliderStyle(Slider::LinearVertical);
        ampEnvSustainSlider.setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
        ampEnvSustainSlider.setBounds(136, 120, 40, 136);
        ampEnvSustainSlider.setRange(0.0f, 1.0f);
        ampEnvSustainSlider.setValue(1.0f);
        addAndMakeVisible(ampEnvSustainSlider);

        ampEnvReleaseSlider.setSliderStyle(Slider::LinearVertical);
        ampEnvReleaseSlider.setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
        ampEnvReleaseSlider.setBounds(184, 120, 40, 136);
        ampEnvReleaseSlider.setRange(0.0f, 5000.0f);
        ampEnvReleaseSlider.setValue(1000.0f);
        addAndMakeVisible(ampEnvReleaseSlider);

        filterEnvAttackSlider.setSliderStyle(Slider::LinearVertical);
        filterEnvAttackSlider.setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
        filterEnvAttackSlider.setBounds(184+40, 120, 40, 136);
        filterEnvAttackSlider.setRange(0.0f, 500.0f);
        filterEnvAttackSlider.setValue(0.0f);
        addAndMakeVisible(filterEnvAttackSlider);

        filterEnvDecaySlider.setSliderStyle(Slider::LinearVertical);
        filterEnvDecaySlider.setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
        filterEnvDecaySlider.setBounds(184+88, 120, 40, 136);
        filterEnvDecaySlider.setRange(0.0f, 1000.0f);
        filterEnvDecaySlider.setValue(0.0f);
        addAndMakeVisible(filterEnvDecaySlider);

        filterEnvSustainSlider.setSliderStyle(Slider::LinearVertical);
        filterEnvSustainSlider.setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
        filterEnvSustainSlider.setBounds(184+136, 120, 40, 136);
        filterEnvSustainSlider.setRange(0.0f, 1.0f);
        filterEnvSustainSlider.setValue(1.0f);
        addAndMakeVisible(filterEnvSustainSlider);

        filterEnvReleaseSlider.setSliderStyle(Slider::LinearVertical);
        filterEnvReleaseSlider.setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
        filterEnvReleaseSlider.setBounds(184+184, 120, 40, 136);
        filterEnvReleaseSlider.setRange(.0f, 5000.0f);
        filterEnvReleaseSlider.setValue(1000.0f);
        addAndMakeVisible(filterEnvReleaseSlider);

        filterEnvModAmtSlider.setSliderStyle(Slider::LinearVertical);
        filterEnvModAmtSlider.setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
        filterEnvModAmtSlider.setBounds(184 + 184+48, 120, 40, 136);
        filterEnvModAmtSlider.setRange(-20000., 20000.0f);
        filterEnvModAmtSlider.setValue(0.0f);
        addAndMakeVisible(filterEnvModAmtSlider);

        ampEnvAttackSlider.onValueChange = [this]
        {
            undoManager.beginNewTransaction();
            samplerVoiceDataModel.setAmpEnvAttack(ampEnvAttackSlider.getValue()*.001,
                ampEnvAttackSlider.isMouseButtonDown() ? nullptr : &undoManager);
        };

        ampEnvDecaySlider.onValueChange = [this]
        {
            undoManager.beginNewTransaction();
            samplerVoiceDataModel.setAmpEnvDecay(ampEnvDecaySlider.getValue() * .001,
                ampEnvDecaySlider.isMouseButtonDown() ? nullptr : &undoManager);
        };

        ampEnvSustainSlider.onValueChange = [this]
        {
            undoManager.beginNewTransaction();
            samplerVoiceDataModel.setAmpEnvSustain(ampEnvSustainSlider.getValue(),
                ampEnvSustainSlider.isMouseButtonDown() ? nullptr : &undoManager);
        };

        ampEnvReleaseSlider.onValueChange = [this]
        {
            undoManager.beginNewTransaction();
            samplerVoiceDataModel.setAmpEnvRelease(ampEnvReleaseSlider.getValue() * .001,
                ampEnvReleaseSlider.isMouseButtonDown() ? nullptr : &undoManager);
        };

        filterEnvAttackSlider.onValueChange = [this]
        {
            undoManager.beginNewTransaction();
            samplerVoiceDataModel.setFilterEnvAttack(filterEnvAttackSlider.getValue() * .001,
                filterEnvAttackSlider.isMouseButtonDown() ? nullptr : &undoManager);
        };

        filterEnvDecaySlider.onValueChange = [this]
        {
            undoManager.beginNewTransaction();
            samplerVoiceDataModel.setFilterEnvDecay(filterEnvDecaySlider.getValue() * .001,
                filterEnvDecaySlider.isMouseButtonDown() ? nullptr : &undoManager);
        };

        filterEnvSustainSlider.onValueChange = [this]
        {
            undoManager.beginNewTransaction();
            samplerVoiceDataModel.setFilterEnvSustain(filterEnvSustainSlider.getValue(),
                filterEnvSustainSlider.isMouseButtonDown() ? nullptr : &undoManager);
        };

        filterEnvReleaseSlider.onValueChange = [this]
        {
            undoManager.beginNewTransaction();
            samplerVoiceDataModel.setFilterEnvRelease(filterEnvReleaseSlider.getValue() * .001,
                filterEnvReleaseSlider.isMouseButtonDown() ? nullptr : &undoManager);
        };

        filterEnvModAmtSlider.onValueChange = [this]
        {
            undoManager.beginNewTransaction();
            samplerVoiceDataModel.setFilterEnvModAmt(filterEnvModAmtSlider.getValue(),
                filterEnvModAmtSlider.isMouseButtonDown() ? nullptr : &undoManager);
        };

        auto setReader = [this](const FileChooser& fc)
        {
            const auto result = fc.getResult();

            if (result != File())
            {
                undoManager.beginNewTransaction();
                auto readerFactory = new FileAudioFormatReaderFactory(result);
                dataModel.setSampleReader(std::unique_ptr<AudioFormatReaderFactory>(readerFactory),
                    &undoManager);
            }
        };

        loadNewSampleButton.onClick = [this, setReader]
        {
            fileChooser.launchAsync(FileBrowserComponent::FileChooserFlags::openMode |
                FileBrowserComponent::FileChooserFlags::canSelectFiles,
                setReader);
        };

        addAndMakeVisible(centreFrequency);
        centreFrequency.onValueChange = [this]
        {
            undoManager.beginNewTransaction();
            dataModel.setCentreFrequencyHz(centreFrequency.getValue(),
                centreFrequency.isMouseButtonDown() ? nullptr : &undoManager);
        };

        centreFrequency.setRange(20, 20000, 1);
        centreFrequency.setSliderStyle(Slider::SliderStyle::IncDecButtons);
        centreFrequency.setIncDecButtonsMode(Slider::IncDecButtonMode::incDecButtonsDraggable_Vertical);

        auto radioGroupId = 1;

        for (auto buttonPtr : { &loopKindNone, &loopKindForward, &loopKindPingpong })
        {
            addAndMakeVisible(buttonPtr);
            buttonPtr->setRadioGroupId(radioGroupId, dontSendNotification);
            buttonPtr->setClickingTogglesState(true);
        }

        loopKindNone.onClick = [this]
        {
            if (loopKindNone.getToggleState())
            {
                undoManager.beginNewTransaction();
                dataModel.setLoopMode(LoopMode::none, &undoManager);
            }
        };

        loopKindForward.onClick = [this]
        {
            if (loopKindForward.getToggleState())
            {
                undoManager.beginNewTransaction();
                dataModel.setLoopMode(LoopMode::forward, &undoManager);
            }
        };

        loopKindPingpong.onClick = [this]
        {
            if (loopKindPingpong.getToggleState())
            {
                undoManager.beginNewTransaction();
                dataModel.setLoopMode(LoopMode::pingpong, &undoManager);
            }
        };

        undoButton.onClick = [this] { undoManager.undo(); };
        redoButton.onClick = [this] { undoManager.redo(); };

        addAndMakeVisible(centreFrequencyLabel);
        addAndMakeVisible(loopKindLabel);

        changeListenerCallback(&undoManager);
        undoManager.addChangeListener(this);
    }

    ~MainSamplerView() override
    {
        undoManager.removeChangeListener(this);
    }

private:
    void changeListenerCallback(ChangeBroadcaster* source) override
    {
        if (source == &undoManager)
        {
            undoButton.setEnabled(undoManager.canUndo());
            redoButton.setEnabled(undoManager.canRedo());
        }
    }

    void resized() override
    {
        auto bounds = getLocalBounds();

        auto topBar = bounds.removeFromTop(50);
        auto padding = 4;
        loadNewSampleButton.setBounds(topBar.removeFromRight(100).reduced(padding));
        redoButton.setBounds(topBar.removeFromRight(100).reduced(padding));
        undoButton.setBounds(topBar.removeFromRight(100).reduced(padding));
        centreFrequencyLabel.setBounds(topBar.removeFromLeft(100).reduced(padding));
        centreFrequency.setBounds(topBar.removeFromLeft(100).reduced(padding));

        auto bottomBar = bounds.removeFromBottom(50);
        loopKindLabel.setBounds(bottomBar.removeFromLeft(100).reduced(padding));
        loopKindNone.setBounds(bottomBar.removeFromLeft(80).reduced(padding));
        loopKindForward.setBounds(bottomBar.removeFromLeft(80).reduced(padding));
        loopKindPingpong.setBounds(bottomBar.removeFromLeft(80).reduced(padding));

        waveformEditor.setBounds(bounds);
    }

    void loopModeChanged(LoopMode value) override
    {
        switch (value)
        {
        case LoopMode::none:
            loopKindNone.setToggleState(true, dontSendNotification);
            break;
        case LoopMode::forward:
            loopKindForward.setToggleState(true, dontSendNotification);
            break;
        case LoopMode::pingpong:
            loopKindPingpong.setToggleState(true, dontSendNotification);
            break;

        default:
            break;
        }
    }

    void centreFrequencyHzChanged(double value) override
    {
        centreFrequency.setValue(value, dontSendNotification);
    }

    DataModel dataModel;
    MPESamplerVoiceDataModel samplerVoiceDataModel;
    WaveformEditor waveformEditor;
    TextButton loadNewSampleButton{ "Load New Sample" };
    TextButton undoButton{ "Undo" };
    TextButton redoButton{ "Redo" };
    Slider centreFrequency;

    TextButton loopKindNone{ "None" },
        loopKindForward{ "Forward" },
        loopKindPingpong{ "Ping Pong" };

    Label centreFrequencyLabel{ {}, "Sample Centre Freq / Hz" },
        loopKindLabel{ {}, "Looping Mode" };


    FileChooser fileChooser{ "Select a file to load...", File(),
                              dataModel.getAudioFormatManager().getWildcardForAllFormats() };

    Slider ampEnvAttackSlider;
    Slider ampEnvDecaySlider;
    Slider ampEnvSustainSlider;
    Slider ampEnvReleaseSlider;

    Slider filterEnvAttackSlider;
    Slider filterEnvDecaySlider;
    Slider filterEnvSustainSlider;
    Slider filterEnvReleaseSlider;
    Slider filterEnvModAmtSlider;

    UndoManager& undoManager;
};