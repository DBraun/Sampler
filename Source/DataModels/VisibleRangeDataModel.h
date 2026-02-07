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

class VisibleRangeDataModel final : private ValueTree::Listener
{
public:
    class Listener
    {
    public:
        virtual ~Listener() noexcept = default;
        virtual void totalRangeChanged(Range<double>) {}
        virtual void visibleRangeChanged(Range<double>) {}
    };

    VisibleRangeDataModel()
        : VisibleRangeDataModel(ValueTree(IDs::VISIBLE_RANGE))
    {}

    explicit VisibleRangeDataModel(const ValueTree& vt)
        : valueTree(vt),
        totalRange(valueTree, IDs::totalRange, nullptr),
        visibleRange(valueTree, IDs::visibleRange, nullptr)
    {
        jassert(valueTree.hasType(IDs::VISIBLE_RANGE));
        valueTree.addListener(this);
    }

    VisibleRangeDataModel(const VisibleRangeDataModel& other)
        : VisibleRangeDataModel(other.valueTree)
    {}

    VisibleRangeDataModel& operator= (const VisibleRangeDataModel& other)
    {
        auto copy(other);
        swap(copy);
        return *this;
    }

    Range<double> getTotalRange() const
    {
        return totalRange;
    }

    void setTotalRange(Range<double> value, UndoManager* undoManager)
    {
        totalRange.setValue(value, undoManager);
        setVisibleRange(visibleRange, undoManager);
    }

    Range<double> getVisibleRange() const
    {
        return visibleRange;
    }

    void setVisibleRange(Range<double> value, UndoManager* undoManager)
    {
        visibleRange.setValue(totalRange.get().constrainRange(value), undoManager);
    }

    void addListener(Listener& listener)
    {
        listenerList.add(&listener);
    }

    void removeListener(Listener& listener)
    {
        listenerList.remove(&listener);
    }

    void swap(VisibleRangeDataModel& other) noexcept
    {
        using std::swap;
        swap(other.valueTree, valueTree);
    }

private:
    void valueTreePropertyChanged(ValueTree&, const Identifier& property) override
    {
        if (property == IDs::totalRange)
        {
            totalRange.forceUpdateOfCachedValue();
            listenerList.call([this](Listener& l) { l.totalRangeChanged(totalRange); });
        }
        else if (property == IDs::visibleRange)
        {
            visibleRange.forceUpdateOfCachedValue();
            listenerList.call([this](Listener& l) { l.visibleRangeChanged(visibleRange); });
        }
    }

    void valueTreeChildAdded(ValueTree&, ValueTree&)      override { jassertfalse; }
    void valueTreeChildRemoved(ValueTree&, ValueTree&, int) override { jassertfalse; }
    void valueTreeChildOrderChanged(ValueTree&, int, int)        override { jassertfalse; }
    void valueTreeParentChanged(ValueTree&)                  override { jassertfalse; }

    ValueTree valueTree;

    CachedValue<Range<double>> totalRange;
    CachedValue<Range<double>> visibleRange;

    ListenerList<Listener> listenerList;
};

namespace
{
    void initialiseComboBoxWithConsecutiveIntegers(juce::Component& owner,
        ComboBox& comboBox,
        Label& label,
        int firstValue,
        int numValues,
        int valueToSelect)
    {
        for (auto i = 0; i < numValues; ++i)
            comboBox.addItem(String(i + firstValue), i + 1);

        comboBox.setSelectedId(valueToSelect - firstValue + 1);

        label.attachToComponent(&comboBox, true);
        owner.addAndMakeVisible(comboBox);
    }

    constexpr int controlHeight = 24;
    constexpr int controlSeparation = 6;

} // namespace
