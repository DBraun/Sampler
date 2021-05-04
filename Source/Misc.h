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

#include "DemoUtilities.h"

#include <array>
#include <atomic>
#include <memory>
#include <vector>
#include <tuple>
#include <iomanip>
#include <sstream>
#include <functional>
#include <mutex>

namespace IDs
{

#define DECLARE_ID(name) const juce::Identifier name (#name);

    DECLARE_ID(DATA_MODEL)
    DECLARE_ID(samplerVoice)
    DECLARE_ID(sampleReader)
    DECLARE_ID(centreFrequencyHz)
    DECLARE_ID(loopMode)
    DECLARE_ID(loopPointsSeconds)

    DECLARE_ID(ampEnvAttack)
    DECLARE_ID(ampEnvDecay)
    DECLARE_ID(ampEnvSustain)
    DECLARE_ID(ampEnvRelease)
    DECLARE_ID(ampEnvModAmt)

    DECLARE_ID(filterCutoff)

    DECLARE_ID(filterEnvAttack)
    DECLARE_ID(filterEnvDecay)
    DECLARE_ID(filterEnvSustain)
    DECLARE_ID(filterEnvRelease)
    DECLARE_ID(filterEnvModAmt)

    DECLARE_ID(MPE_SETTINGS)
    DECLARE_ID(synthVoices)
    DECLARE_ID(voiceStealingEnabled)
    DECLARE_ID(legacyModeEnabled)
    DECLARE_ID(mpeZoneLayout)
    DECLARE_ID(legacyFirstChannel)
    DECLARE_ID(legacyLastChannel)
    DECLARE_ID(legacyPitchbendRange)

    DECLARE_ID(VISIBLE_RANGE)
    DECLARE_ID(totalRange)
    DECLARE_ID(visibleRange)

#undef DECLARE_ID

} // namespace IDs

enum class LoopMode
{
    none,
    forward,
    pingpong
};

// We want to send type-erased commands to the audio thread, but we also
// want those commands to contain move-only resources, so that we can
// construct resources on the gui thread, and then transfer ownership
// cheaply to the audio thread. We can't do this with std::function
// because it enforces that functions are copy-constructible.
// Therefore, we use a very simple templated type-eraser here.
template <typename Proc>
struct Command
{
    virtual ~Command() noexcept = default;
    virtual void run(Proc& proc) = 0;
};

template <typename Proc, typename Func>
class TemplateCommand : public Command<Proc>,
    private Func
{
public:
    template <typename FuncPrime>
    explicit TemplateCommand(FuncPrime&& funcPrime)
        : Func(std::forward<FuncPrime>(funcPrime))
    {}

    void run(Proc& proc) override { (*this) (proc); }
};

class AudioFormatReaderFactory
{
public:
    virtual ~AudioFormatReaderFactory() noexcept = default;
    virtual std::unique_ptr<AudioFormatReader> make(AudioFormatManager&) const = 0;
    virtual std::unique_ptr<AudioFormatReaderFactory> clone() const = 0;
};

template <typename Contents>
class ReferenceCountingAdapter : public ReferenceCountedObject
{
public:
    template <typename... Args>
    explicit ReferenceCountingAdapter(Args&&... args)
        : contents(std::forward<Args>(args)...)
    {}

    const Contents& get() const
    {
        return contents;
    }

    Contents& get()
    {
        return contents;
    }

private:
    Contents contents;
};

namespace juce {

    template <typename Contents, typename... Args>
    std::unique_ptr<ReferenceCountingAdapter<Contents>>
        make_reference_counted(Args&&... args)
    {
        auto adapter = new ReferenceCountingAdapter<Contents>(std::forward<Args>(args)...);
        return std::unique_ptr<ReferenceCountingAdapter<Contents>>(adapter);
    }

    template<>
    struct VariantConverter<LoopMode>
    {
        static LoopMode fromVar(const var& v)
        {
            return static_cast<LoopMode> (int(v));
        }

        static var toVar(LoopMode loopMode)
        {
            return static_cast<int> (loopMode);
        }
    };

    template <typename Wrapped>
    struct GenericVariantConverter
    {
        static Wrapped fromVar(const var& v)
        {
            auto cast = dynamic_cast<ReferenceCountingAdapter<Wrapped>*> (v.getObject());
            jassert(cast != nullptr);
            return cast->get();
        }

        static var toVar(Wrapped range)
        {
            return { make_reference_counted<Wrapped>(std::move(range)).release() };
        }
    };

    template <typename Numeric>
    struct VariantConverter<Range<Numeric>> : GenericVariantConverter<Range<Numeric>> {};

    template<>
    struct VariantConverter<MPEZoneLayout> : GenericVariantConverter<MPEZoneLayout> {};

    template<>
    struct VariantConverter<std::shared_ptr<AudioFormatReaderFactory>>
        : GenericVariantConverter<std::shared_ptr<AudioFormatReaderFactory>>
    {};
} // namespace juce