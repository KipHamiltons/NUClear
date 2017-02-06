/*
 * Copyright (C) 2013-2016 Trent Houliston <trent@houliston.me>, Jake Woods <jake.f.woods@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef NUCLEAR_EXTENSION_NETWORK_WIRE_PROTOCOL_HPP
#define NUCLEAR_EXTENSION_NETWORK_WIRE_PROTOCOL_HPP

#include "nuclear"

namespace NUClear {
namespace extension {
    namespace network {

#pragma pack(push, 1)
        enum Type : uint8_t { ANNOUNCE = 1, LEAVE = 2, DATA = 3, ACK = 4, NACK = 5 };

        struct PacketHeader {
            PacketHeader() : type() {}

            uint8_t header[3] = {0xE2, 0x98, 0xA2};  // Radioactive symbol in UTF8
            uint8_t version   = 0x02;                // The NUClear networking version
            Type type;                               // The type of packet
        };

        struct AnnouncePacket : public PacketHeader {
            AnnouncePacket() : name(0) {}

            char name;  // A null terminated string name for this node (&name)
        };

        struct LeavePacket : public PacketHeader {
            LeavePacket() {}
        };

        struct DataPacket : public PacketHeader {
            DataPacket() : packet_id(0), packet_no(0), packet_count(1), reliable(false), hash(), data(0) {}

            uint16_t packet_id;            // A semiunique identifier for this packet group
            uint16_t packet_no;            // What packet number this is within the group
            uint16_t packet_count;         // How many packets there are in the group
            bool reliable;                 // If this packet is reliable and should be acked
            std::array<uint64_t, 2> hash;  // The 128 bit hash to identify the data type
            char data;                     // The data (&data)
        };

        struct ACKPacket : public PacketHeader {
            ACKPacket() : packet_id(0), packet_count(1), packets(0) {}

            uint16_t packet_id;     // The packet group identifier we are acknowledging
            uint16_t packet_count;  // How many packets there are in the group
            uint8_t packets;        // A bitset of which packets we have received (&packets)
        };
        
        struct NACKPacket : public ACKPacket {
        };

#pragma pack(pop)

        /// The largest packet we will transmit is based on an MTU of 1500 which should be safe for most uses
        static constexpr const size_t NUCLEAR_PACKET_MTU = 1500;

        // Our max amount of data is based on the MTU size
        // Subtract the IP and UDP headers, and our protocol header size
        static constexpr const size_t MAX_UDP_PAYLOAD_LENGTH =
            NUCLEAR_PACKET_MTU /*MTU*/ - 20 /*IP header*/ - 8 /*UDP header*/ - sizeof(network::DataPacket)
            + 1 /*Last char*/;


    }  // namespace network
}  // namespace extension
}  // namespace NUClear

#endif  // NUCLEAR_EXTENSION_NETWORK_WIRE_PROTOCOL_HPP
