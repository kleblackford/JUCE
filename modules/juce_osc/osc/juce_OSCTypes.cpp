/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 7 End-User License
   Agreement and JUCE Privacy Policy.

   End User License Agreement: www.juce.com/juce-7-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

const OSCType OSCTypes::int32   = 'i';
const OSCType OSCTypes::int64	= 'h';
const OSCType OSCTypes::float32 = 'f';
const OSCType OSCTypes::float64 = 'd';
const OSCType OSCTypes::string  = 's';
const OSCType OSCTypes::blob    = 'b';
const OSCType OSCTypes::colour  = 'r';

uint32 OSCColour::toInt32() const
{
    return ByteOrder::makeInt (alpha, blue, green, red);
}

OSCColour OSCColour::fromInt32 (uint32 c)
{
    return { (uint8) (c >> 24),
             (uint8) (c >> 16),
             (uint8) (c >> 8),
             (uint8) c };
}

} // namespace juce
