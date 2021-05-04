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

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;


class MainSamplerView : public Component,
    private DataModel::Listener,
    private ChangeListener,
    public ValueTree::Listener
{
public:
    MainSamplerView(const DataModel& model,
        PlaybackPositionOverlay::Provider provider,
        UndoManager& um,
        AudioProcessorValueTreeState& vts)
        : dataModel(model),
        waveformEditor(dataModel, move(provider), um),
        undoManager(um),
        valueTreeState(vts)
    {
        valueTreeState.state.addListener(this);

        dataModel.addListener(*this);

        int WIDTH = 48;

        addAndMakeVisible(waveformEditor);
        addAndMakeVisible(loadNewSampleButton);
        addAndMakeVisible(undoButton);
        addAndMakeVisible(redoButton);

        ampEnvAttackSlider.setSliderStyle(Slider::LinearVertical);
        ampEnvAttackSlider.setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
        ampEnvAttackSlider.setBounds(0*WIDTH, 120, 40, 136);
        ampEnvAttackSlider.setRange(0.0f, 500.0f);
        ampEnvAttackSlider.setValue(0.0f);
        addAndMakeVisible(ampEnvAttackSlider);

        ampEnvDecaySlider.setSliderStyle(Slider::LinearVertical);
        ampEnvDecaySlider.setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
        ampEnvDecaySlider.setBounds(1 * WIDTH, 120, 40, 136);
        ampEnvDecaySlider.setRange(0.0f, 1000.0f);
        ampEnvDecaySlider.setValue(0.0f);
        addAndMakeVisible(ampEnvDecaySlider);

        ampEnvSustainSlider.setSliderStyle(Slider::LinearVertical);
        ampEnvSustainSlider.setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
        ampEnvSustainSlider.setBounds(2 * WIDTH, 120, 40, 136);
        ampEnvSustainSlider.setRange(0.0f, 1.0f);
        ampEnvSustainSlider.setValue(1.0f);
        addAndMakeVisible(ampEnvSustainSlider);

        ampEnvReleaseSlider.setSliderStyle(Slider::LinearVertical);
        ampEnvReleaseSlider.setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
        ampEnvReleaseSlider.setBounds(3 * WIDTH, 120, 40, 136);
        ampEnvReleaseSlider.setRange(0.0f, 5000.0f);
        ampEnvReleaseSlider.setValue(1000.0f);
        addAndMakeVisible(ampEnvReleaseSlider);

        ampEnvModAmtSlider.setSliderStyle(Slider::LinearVertical);
        ampEnvModAmtSlider.setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
        ampEnvModAmtSlider.setBounds(4 * WIDTH, 120, 40, 136);
        ampEnvModAmtSlider.setRange(-20000., 20000.0f);
        ampEnvModAmtSlider.setValue(0.0f);
        addAndMakeVisible(ampEnvModAmtSlider);



        filterEnvAttackSlider.setSliderStyle(Slider::LinearVertical);
        filterEnvAttackSlider.setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
        filterEnvAttackSlider.setBounds(5 * WIDTH, 120, 40, 136);
        filterEnvAttackSlider.setRange(0.0f, 500.0f);
        filterEnvAttackSlider.setValue(0.0f);
        addAndMakeVisible(filterEnvAttackSlider);

        filterEnvDecaySlider.setSliderStyle(Slider::LinearVertical);
        filterEnvDecaySlider.setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
        filterEnvDecaySlider.setBounds(6 * WIDTH, 120, 40, 136);
        filterEnvDecaySlider.setRange(0.0f, 1000.0f);
        filterEnvDecaySlider.setValue(0.0f);
        addAndMakeVisible(filterEnvDecaySlider);

        filterEnvSustainSlider.setSliderStyle(Slider::LinearVertical);
        filterEnvSustainSlider.setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
        filterEnvSustainSlider.setBounds(7 * WIDTH, 120, 40, 136);
        filterEnvSustainSlider.setRange(0.0f, 1.0f);
        filterEnvSustainSlider.setValue(1.0f);
        addAndMakeVisible(filterEnvSustainSlider);

        filterEnvReleaseSlider.setSliderStyle(Slider::LinearVertical);
        filterEnvReleaseSlider.setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
        filterEnvReleaseSlider.setBounds(8 * WIDTH, 120, 40, 136);
        filterEnvReleaseSlider.setRange(.0f, 5000.0f);
        filterEnvReleaseSlider.setValue(1000.0f);
        addAndMakeVisible(filterEnvReleaseSlider);

        filterEnvModAmtSlider.setSliderStyle(Slider::LinearVertical);
        filterEnvModAmtSlider.setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
        filterEnvModAmtSlider.setBounds(9 * WIDTH, 120, 40, 136);
        filterEnvModAmtSlider.setRange(-20000., 20000.0f);
        filterEnvModAmtSlider.setValue(0.0f);
        addAndMakeVisible(filterEnvModAmtSlider);

        filterCutoffSlider.setSliderStyle(Slider::LinearVertical);
        filterCutoffSlider.setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
        filterCutoffSlider.setBounds(10 * WIDTH, 120, 40, 136);
        filterCutoffSlider.setRange(-20000., 20000.0f);
        filterCutoffSlider.setValue(0.0f);
        addAndMakeVisible(filterCutoffSlider);

        filterEnvAttackAttachment.reset(new SliderAttachment(valueTreeState, "filterEnvAttack", filterEnvAttackSlider));
        filterEnvDecayAttachment.reset(new SliderAttachment(valueTreeState, "filterEnvDecay", filterEnvDecaySlider));
        filterEnvSustainAttachment.reset(new SliderAttachment(valueTreeState, "filterEnvSustain", filterEnvSustainSlider));
        filterEnvReleaseAttachment.reset(new SliderAttachment(valueTreeState, "filterEnvRelease", filterEnvReleaseSlider));
        filterEnvModAmtAttachment.reset(new SliderAttachment(valueTreeState, "filterEnvModAmt", filterEnvModAmtSlider));
        filterCutoffAttachment.reset(new SliderAttachment(valueTreeState, "filterCutoff", filterCutoffSlider));

        ampEnvAttackAttachment.reset(new SliderAttachment(valueTreeState, "ampEnvAttack", ampEnvAttackSlider));
        ampEnvDecayAttachment.reset(new SliderAttachment(valueTreeState, "ampEnvDecay", ampEnvDecaySlider));
        ampEnvSustainAttachment.reset(new SliderAttachment(valueTreeState, "ampEnvSustain", ampEnvSustainSlider));
        ampEnvReleaseAttachment.reset(new SliderAttachment(valueTreeState, "ampEnvRelease", ampEnvReleaseSlider));
        ampEnvModAmtAttachment.reset(new SliderAttachment(valueTreeState, "ampEnvModAmt", ampEnvModAmtSlider));

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

        centreFrequency.setRange(2, 20000, 1);
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

    AudioProcessorValueTreeState& valueTreeState;  // from the SamplerAudioProcessor

    DataModel dataModel;
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
    Slider ampEnvModAmtSlider;

    Slider filterCutoffSlider;

    Slider filterEnvAttackSlider;
    Slider filterEnvDecaySlider;
    Slider filterEnvSustainSlider;
    Slider filterEnvReleaseSlider;
    Slider filterEnvModAmtSlider;

    std::unique_ptr<SliderAttachment> ampEnvAttackAttachment;
    std::unique_ptr<SliderAttachment> ampEnvDecayAttachment;
    std::unique_ptr<SliderAttachment> ampEnvSustainAttachment;
    std::unique_ptr<SliderAttachment> ampEnvReleaseAttachment;
    std::unique_ptr<SliderAttachment> ampEnvModAmtAttachment;

    std::unique_ptr<SliderAttachment> filterEnvAttackAttachment;
    std::unique_ptr<SliderAttachment> filterEnvDecayAttachment;
    std::unique_ptr<SliderAttachment> filterEnvSustainAttachment;
    std::unique_ptr<SliderAttachment> filterEnvReleaseAttachment;
    std::unique_ptr<SliderAttachment> filterEnvModAmtAttachment;
    std::unique_ptr<SliderAttachment> filterCutoffAttachment;

    UndoManager& undoManager;
};