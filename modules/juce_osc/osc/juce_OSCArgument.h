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

//==============================================================================
/**
    An OSC argument.

    An OSC argument is a value of one of the following types: int32, float32, string,
    or blob (raw binary data).

    OSCMessage objects are essentially arrays of OSCArgument objects.

    @tags{OSC}
*/
class JUCE_API  OSCArgument
{
public:
    /** Constructs an OSCArgument with type int32 and a given value. */
    OSCArgument (int32 value);

	/** Constructs an OSCArgument with type int64 and a given value. */
	OSCArgument(int64 value);

    /** Constructs an OSCArgument with type float32 and a given value. */
    OSCArgument (float value);

	/** Constructs an OSCArgument with type float32 and a given value. */
	OSCArgument(double value);

    /** Constructs an OSCArgument with type string and a given value */
    OSCArgument (const String& value);

    /** Constructs an OSCArgument with type blob and copies dataSize bytes
        from the memory pointed to by data into the blob.

        The data owned by the blob will be released when the OSCArgument object
        gets destructed.
    */
    OSCArgument (MemoryBlock blob);

    /** Constructs an OSCArgument with type colour and a given colour value */
    OSCArgument (OSCColour colour);

    /** Returns the type of the OSCArgument as an OSCType.
        OSCType is a char type, and its value will be the OSC type tag of the type.
    */
    OSCType getType() const noexcept        { return type; }

    /** Returns whether the type of the OSCArgument is int32. */
    bool isInt32() const noexcept           { return type == OSCTypes::int32; }

	/** Returns whether the type of the OSCArgument is int32. */
	bool isInt64() const noexcept			{ return type == OSCTypes::int64; }

    /** Returns whether the type of the OSCArgument is float. */
    bool isFloat32() const noexcept         { return type == OSCTypes::float32; }

	/** Returns whether the type of the OSCArgument is double. */
	bool isFloat64() const noexcept			{ return type == OSCTypes::float64; }

    /** Returns whether the type of the OSCArgument is string. */
    bool isString() const noexcept          { return type == OSCTypes::string; }

    /** Returns whether the type of the OSCArgument is blob. */
    bool isBlob() const noexcept            { return type == OSCTypes::blob; }

    /** Returns whether the type of the OSCArgument is colour. */
    bool isColour() const noexcept          { return type == OSCTypes::colour; }

    /** Returns the value of the OSCArgument as an int32.
        If the type of the OSCArgument is not int32, the behaviour is undefined.
    */
    int32 getInt32() const noexcept;

	/** Returns the value of the OSCArgument as an int32.
		If the type of the OSCArgument is not int32, the behaviour is undefined.
	*/
	int64 getInt64() const noexcept;

    /** Returns the value of the OSCArgument as a float32.
        If the type of the OSCArgument is not float32, the behaviour is undefined.
    */
    float getFloat32() const noexcept;

	/** Returns the value of the OSCArgument as a float64.
		If the type of the OSCArgument is not float64, the behaviour is undefined.
	*/
	double getFloat64() const noexcept;

    /** Returns the value of the OSCArgument as a string.
        If the type of the OSCArgument is not string, the behaviour is undefined.
    */
    String getString() const noexcept;

    /** Returns the binary data contained in the blob and owned by the OSCArgument,
        as a reference to a JUCE MemoryBlock object.

        If the type of the OSCArgument is not blob, the behaviour is undefined.
    */
    const MemoryBlock& getBlob() const noexcept;

    /** Returns the value of the OSCArgument as an OSCColour.
        If the type of the OSCArgument is not a colour, the behaviour is undefined.
    */
    OSCColour getColour() const noexcept;

private:
    //==============================================================================
    OSCType type;

    union
    {
        int32 intValue;
        float floatValue;
    };

	union
	{
		int64 longValue;
		double doubleValue;
	};

    String stringValue;
    MemoryBlock blob;
};

} // namespace juce
