//
// Copyright (c) 2013 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier: MIT
//


#pragma once

#include<TWELITE>
#include "Encoding/COBS.h"
#include "Encoding/SLIP.h"

/// \brief A template class enabling packet-based Serial communication.
///
/// Typically one of the typedefined versions are used, for example,
/// `COBSPacketSerial` or `SLIPPacketSerial`.
///
/// The template parameters allow the user to define their own packet encoder /
/// decoder, custom packet marker and receive buffer size.
///
/// \tparam EncoderType The static packet encoder class name.
/// \tparam PacketMarker The byte value used to mark the packet boundary.
/// \tparam BufferSize The number of bytes allocated for the receive buffer.
template<typename EncoderType, uint8_t PacketMarker = 0, size_t ReceiveBufferSize = 256>
class PacketSerial_
{
public:
    /// \brief A typedef describing the packet handler method.
    ///
    /// The packet handler method usually has the form:
    ///
    ///     void onPacketReceived(const uint8_t* buffer, size_t size);
    ///
    /// where buffer is a pointer to the incoming buffer array, and size is the
    /// number of bytes in the incoming buffer.
    typedef void (*PacketHandlerFunction)(const uint8_t* buffer, size_t size);

    /// \brief A typedef describing the packet handler method.
    ///
    /// The packet handler method usually has the form:
    ///
    ///     void onPacketReceived(void* sender, const uint8_t* buffer, size_t size);
    ///
    /// where sender is a pointer to the PacketSerial_ instance that recieved
    /// the buffer,  buffer is a pointer to the incoming buffer array, and size
    /// is the number of bytes in the incoming buffer.
    typedef void (*PacketHandlerFunctionWithSender)(const void* sender, const uint8_t* buffer, size_t size);

    /// \brief Construct a default PacketSerial_ device.
    PacketSerial_():
        _receiveBufferIndex(0),
        _onPacketFunction(nullptr),
        _onPacketFunctionWithSender(nullptr),
        _senderPtr(nullptr)
    {
    }

    /// \brief Destroy the PacketSerial_ device.
    ~PacketSerial_()
    {
    }

    /// \brief Begin a default serial connection with the given speed.
    ///
    /// The default Serial port `Serial` and default config `SERIAL_8N1` will be
    /// used. For example:
    ///
    ///     PacketSerial myPacketSerial;
    ///
    ///     void setup()
    ///     {
    ///         myPacketSerial1.begin(9600);
    ///     }
    ///
    /// This is a convenience method. For more complex Serial port
    /// configurations, use the `setStream()` function to set an arbitrary
    /// Arduino Stream.
    ///
    /// \param speed The serial data transmission speed in bits / second (baud).
    /// \sa https://www.arduino.cc/en/Serial/Begin
    void begin(unsigned long speed)
    {
#ifdef UART0
        Serial.begin(speed);
#else
        Serial1.begin(speed);
#endif
    }

    /// \brief The update function services the serial connection.
    ///
    /// This must be called often, ideally once per `loop()`, e.g.:
    ///
    ///     void loop()
    ///     {
    ///         // Other program code.
    ///
    ///         myPacketSerial1.update();
    ///     }
    ///
    void update()
    {
#ifdef UART0

        while (Serial.available() > 0)
        {
            uint8_t data = Serial.read();
#else
        while (Serial1.available() > 0)
        {
            uint8_t data = Serial1.read();
#endif
            if (data == PacketMarker)
            {
                if (_onPacketFunction || _onPacketFunctionWithSender)
                {
                    uint8_t _decodeBuffer[_receiveBufferIndex];

                    size_t numDecoded = EncoderType::decode(_receiveBuffer,
                                                            _receiveBufferIndex,
                                                            _decodeBuffer);

                    // clear the index here so that the callback function can call update() if needed and receive more data
                    _receiveBufferIndex = 0;
                    _recieveBufferOverflow = false;

                    if (_onPacketFunction)
                    {
                        _onPacketFunction(_decodeBuffer, numDecoded);
                    }
                    else if (_onPacketFunctionWithSender)
                    {
                        _onPacketFunctionWithSender(_senderPtr, _decodeBuffer, numDecoded);
                    }

                } else {
                    _receiveBufferIndex = 0;
                    _recieveBufferOverflow = false;
                }
            }
            else
            {
                if ((_receiveBufferIndex + 1) < ReceiveBufferSize)
                {
                    _receiveBuffer[_receiveBufferIndex++] = data;
                }
                else
                {
                    // The buffer will be in an overflowed state if we write
                    // so set a buffer overflowed flag.
                    _recieveBufferOverflow = true;
                }
            }
        }
    }

    /// \brief Set a packet of data.
    ///
    /// This function will encode and send an arbitrary packet of data. After
    /// sending, it will send the specified `PacketMarker` defined in the
    /// template parameters.
    ///
    ///     // Make an array.
    ///     uint8_t myPacket[2] = { 255, 10 };
    ///
    ///     // Send the array.
    ///     myPacketSerial1.send(myPacket, 2);
    ///
    /// \param buffer A pointer to a data buffer.
    /// \param size The number of bytes in the data buffer.
    void send(const uint8_t* buffer, size_t size) const
    {
        if(buffer == nullptr || size == 0) return;

        uint8_t _encodeBuffer[EncoderType::getEncodedBufferSize(size)];

        size_t numEncoded = EncoderType::encode(buffer,
                                                size,
                                                _encodeBuffer);

#ifdef UART0
        for(size_t i=0;i<numEncoded;i++){
            Serial.write(_encodeBuffer[i]);
        }
        Serial.write(PacketMarker);
#else
        for(size_t i=0;i<numEncoded;i++){
            Serial1.write(_encodeBuffer[i]);
        }
        Serial1.write(PacketMarker);
#endif
    }

    /// \brief Set the function that will receive decoded packets.
    ///
    /// This function will be called when data is read from the serial stream
    /// connection and a packet is decoded. The decoded packet will be passed
    /// to the packet handler. The packet handler must have the form:
    ///
    /// The packet handler method usually has the form:
    ///
    ///     void onPacketReceived(const uint8_t* buffer, size_t size);
    ///
    /// The packet handler would then be registered like this:
    ///
    ///     myPacketSerial1.setPacketHandler(&onPacketReceived);
    ///
    /// Setting a packet handler will remove all other packet handlers.
    ///
    /// \param onPacketFunction A pointer to the packet handler function.
    void setPacketHandler(PacketHandlerFunction onPacketFunction)
    {
        _onPacketFunction = onPacketFunction;
        _onPacketFunctionWithSender = nullptr;
        _senderPtr = nullptr;
    }

    /// \brief Set the function that will receive decoded packets.
    ///
    /// This function will be called when data is read from the serial stream
    /// connection and a packet is decoded. The decoded packet will be passed
    /// to the packet handler. The packet handler must have the form:
    ///
    /// The packet handler method usually has the form:
    ///
    ///     void onPacketReceived(const void* sender, const uint8_t* buffer, size_t size);
    ///
    /// To determine the sender, compare the pointer to the known possible
    /// PacketSerial senders.
    ///
    ///     void onPacketReceived(void* sender, const uint8_t* buffer, size_t size)
    ///     {
    ///         if (sender == &myPacketSerial)
    ///         {
    ///             // Do something with the packet from myPacketSerial1.
    ///         }
    ///         else if (sender == &myOtherPacketSerial)
    ///         {
    ///             // Do something with the packet from myOtherPacketSerial1.
    ///         }
    ///     }
    ///
    /// The packet handler would then be registered like this:
    ///
    ///     myPacketSerial1.setPacketHandler(&onPacketReceived);
    ///
    /// You can also register an arbitrary void* pointer to be passed to your packet handler method.
    /// This is most useful when PacketSerial is used inside a class, to pass a pointer to
    /// the containing class:
    ///
    ///     class EchoClass {
    ///       public:
    ///         void begin(unsigned long speed) {
    ///           myPacketSerial1.setPacketHandler(&onPacketReceived, this);
    ///           myPacketSerial1.begin(speed);
    ///         }
    ///     
    ///         // C-style callbacks can't use non-static methods,
    ///         // so we use a static method that receives "this" as the sender argument:
    ///         // https://wiki.c2.com/?VirtualStaticIdiom
    ///         static void onPacketReceived(const void* sender, const uint8_t* buffer, size_t size) {
    ///           ((EchoClass*)sender)->onPacketReceived(buffer, size);
    ///         }
    ///     
    ///         void onPacketReceived(const uint8_t* buffer, size_t size) {
    ///             // we can now use myPacketSerial as needed here
    ///         }
    ///     
    ///         PacketSerial myPacketSerial;
    ///     };
    ///
    /// Setting a packet handler will remove all other packet handlers.
    ///
    /// \param onPacketFunctionWithSender A pointer to the packet handler function.
    /// \param senderPtr Optional pointer to a void* pointer, default argument will pass a pointer to the sending PacketSerial instance to the callback
    void setPacketHandler(PacketHandlerFunctionWithSender onPacketFunctionWithSender, void * senderPtr = nullptr)
    {
        _onPacketFunction = nullptr;
        _onPacketFunctionWithSender = onPacketFunctionWithSender;
        _senderPtr = senderPtr;
        // for backwards compatibility, the default _senderPtr is "this", but you can't use "this" as a default argument
        if(!senderPtr) _senderPtr = this;
    }

    /// \brief Check to see if the receive buffer overflowed.
    ///
    /// This must be called often, directly after the `update()` function.
    ///
    ///     void loop()
    ///     {
    ///         // Other program code.
    ///         myPacketSerial1.update();
    ///
    ///         // Check for a receive buffer overflow.
    ///         if (myPacketSerial1.overflow())
    ///         {
    ///             // Send an alert via a pin (e.g. make an overflow LED) or return a
    ///             // user-defined packet to the sender.
    ///             //
    ///             // Ultimately you may need to just increase your recieve buffer via the
    ///             // template parameters.
    ///         }
    ///     }
    ///
    /// The state is reset every time a new packet marker is received NOT when 
    /// overflow() method is called.
    ///
    /// \returns true if the receive buffer overflowed.
    bool overflow() const
    {
        return _recieveBufferOverflow;
    }

private:
    PacketSerial_(const PacketSerial_&);
    PacketSerial_& operator = (const PacketSerial_&);

    bool _recieveBufferOverflow = false;

    uint8_t _receiveBuffer[ReceiveBufferSize];
    size_t _receiveBufferIndex = 0;

    PacketHandlerFunction _onPacketFunction = nullptr;
    PacketHandlerFunctionWithSender _onPacketFunctionWithSender = nullptr;
    void* _senderPtr = nullptr;
};


/// \brief A typedef for the default COBS PacketSerial class.
typedef PacketSerial_<COBS> PacketSerial;

/// \brief A typedef for a PacketSerial type with COBS encoding.
typedef PacketSerial_<COBS> COBSPacketSerial;

/// \brief A typedef for a PacketSerial type with SLIP encoding.
typedef PacketSerial_<SLIP, SLIP::END> SLIPPacketSerial;
