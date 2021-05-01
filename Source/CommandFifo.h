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

template <typename Proc>
class CommandFifo final
{
public:
    explicit CommandFifo(int size)
        : buffer((size_t)size),
        abstractFifo(size)
    {}

    CommandFifo()
        : CommandFifo(1024)
    {}

    template <typename Item>
    void push(Item&& item) noexcept
    {
        auto command = makeCommand(std::forward<Item>(item));

        abstractFifo.write(1).forEach([&](int index)
            {
                buffer[size_t(index)] = std::move(command);
            });
    }

    void call(Proc& proc) noexcept
    {
        abstractFifo.read(abstractFifo.getNumReady()).forEach([&](int index)
            {
                buffer[size_t(index)]->run(proc);
            });
    }

private:
    template <typename Func>
    static std::unique_ptr<Command<Proc>> makeCommand(Func&& func)
    {
        using Decayed = typename std::decay<Func>::type;
        return std::make_unique<TemplateCommand<Proc, Decayed>>(std::forward<Func>(func));
    }

    std::vector<std::unique_ptr<Command<Proc>>> buffer;
    AbstractFifo abstractFifo;
};