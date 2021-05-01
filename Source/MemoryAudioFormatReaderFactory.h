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
#include "FileAudioFormatReaderFactory.h"

class MemoryAudioFormatReaderFactory : public AudioFormatReaderFactory
{
public:
    MemoryAudioFormatReaderFactory(const void* sampleDataIn, size_t dataSizeIn)
        : sampleData(sampleDataIn),
        dataSize(dataSizeIn)
    {}

    std::unique_ptr<AudioFormatReader> make(AudioFormatManager& manager) const override
    {
        return makeAudioFormatReader(manager, sampleData, dataSize);
    }

    std::unique_ptr<AudioFormatReaderFactory> clone() const override
    {
        return std::unique_ptr<AudioFormatReaderFactory>(new MemoryAudioFormatReaderFactory(*this));
    }

private:
    const void* sampleData;
    size_t dataSize;
};