#include "io/bit_reader.h"
#include "algo/range.h"
#include "io/memory_stream.h"
#include "test_support/catch.hh"

using namespace au;

static bstr from_bits(std::initializer_list<u8> s)
{
    bstr x;
    for (const u8 &c : s)
        x += c;
    return x;
}

TEST_CASE("BitReader", "[io]")
{
    SECTION("Reading missing bits throws exceptions")
    {
        io::BitReader reader(""_b);
        try
        {
            reader.get(1);
            REQUIRE(0);
        }
        catch (...)
        {
        }
    }

    SECTION("Reading single bits")
    {
        io::BitReader reader("\x8F"_b); // 10001111
        REQUIRE(reader.get(1));
        REQUIRE(!reader.get(1));
        REQUIRE(!reader.get(1));
        REQUIRE(!reader.get(1));
        REQUIRE(reader.get(1));
        REQUIRE(reader.get(1));
        REQUIRE(reader.get(1));
        REQUIRE(reader.get(1));
    }

    SECTION("Reading multiple bits")
    {
        io::BitReader reader(from_bits({0b10001111}));
        REQUIRE(reader.get(7) == 0b1000111);
        REQUIRE(reader.get(1));
    }

    SECTION("Reading multiple bytes")
    {
        SECTION("Smaller test")
        {
            io::BitReader reader("\x8F\x8F"_b); // 10001111
            REQUIRE(reader.get(7) == (0x8F >> 1));
            REQUIRE(reader.get(1));

            REQUIRE(reader.get(1));
            REQUIRE(!reader.get(1));
            REQUIRE(reader.get(4) == 3);
            REQUIRE(reader.get(2) == 3);
            try
            {
                reader.get(1);
                REQUIRE(0);
            }
            catch (...)
            {
            }
        }

        SECTION("Bigger test")
        {
            io::BitReader reader(from_bits(
                {0b10101010, 0b11110000, 0b00110011}));
            REQUIRE(reader.get(1) == 1);
            REQUIRE(reader.get(23) == 0b01010101111000000110011);
        }

        SECTION("Max bit reader capacity test (unaligned)")
        {
            io::BitReader reader(from_bits(
                {0b11001100, 0b10101010, 0b11110000, 0b00110011}));
            REQUIRE(reader.get(32) == 0b11001100'10101010'11110000'00110011);
        }

        SECTION("Max bit reader capacity test (aligned)")
        {
            io::BitReader reader(from_bits(
                {0b11001100, 0b10101010, 0b11110000, 0b00110011, 0b01010101}));
            reader.get(1);
            REQUIRE(reader.get(32) == 0b1001100'10101010'11110000'00110011'0);
        }
    }

    SECTION("Checking for EO")
    {
        io::BitReader reader("\x00\x00"_b);
        reader.get(7);
        REQUIRE(!reader.eof());
        reader.get(7);
        REQUIRE(!reader.eof());
        reader.get(1);
        REQUIRE(!reader.eof());
        reader.get(1);
        REQUIRE(reader.eof());
    }

    SECTION("Checking size")
    {
        io::BitReader reader1("\x00\x00"_b);
        REQUIRE(reader1.size() == 16);
        io::BitReader reader2("\x00"_b);
        REQUIRE(reader2.size() == 8);
        io::BitReader reader3(""_b);
        REQUIRE(reader3.size() == 0);
    }

    SECTION("Seeking")
    {
        SECTION("Integer aligned")
        {
            io::BitReader reader(from_bits({
                0b00000000, 0b00000000, 0b00000000, 0b00000000,
                0b11111111, 0b11111111, 0b11111111, 0b11111111,
                0b11001100, 0b10101010, 0b11110000, 0b00110011}));
            reader.seek(0);
            REQUIRE(reader.get(32) == 0b00000000000000000000000000000000);
            reader.seek(32);
            REQUIRE(reader.get(32) == 0b11111111111111111111111111111111);
            reader.seek(64);
            REQUIRE(reader.get(32) == 0b11001100101010101111000000110011);
        }

        SECTION("Byte aligned")
        {
            io::BitReader reader(from_bits(
                {0b11001100, 0b10101010, 0b11110000, 0b00110011}));
            reader.seek(0);  REQUIRE(reader.get(8) == 0b11001100);
            reader.seek(8);  REQUIRE(reader.get(8) == 0b10101010);
            reader.seek(16); REQUIRE(reader.get(8) == 0b11110000);
            reader.seek(24); REQUIRE(reader.get(8) == 0b00110011);
        }

        SECTION("Unaligned")
        {
            io::BitReader reader(from_bits(
                {0b11001100, 0b10101010, 0b11110000, 0b00110011}));
            reader.seek(0);  REQUIRE(reader.get(8) == 0b11001100);
            reader.seek(1);  REQUIRE(reader.get(8) == 0b10011001);
            reader.seek(2);  REQUIRE(reader.get(8) == 0b00110010);
            reader.seek(3);  REQUIRE(reader.get(8) == 0b01100101);
            reader.seek(4);  REQUIRE(reader.get(8) == 0b11001010);
            reader.seek(5);  REQUIRE(reader.get(8) == 0b10010101);
            reader.seek(6);  REQUIRE(reader.get(8) == 0b00101010);
            reader.seek(7);  REQUIRE(reader.get(8) == 0b01010101);
            reader.seek(8);  REQUIRE(reader.get(8) == 0b10101010);
            reader.seek(9);  REQUIRE(reader.get(8) == 0b01010101);
            reader.seek(10); REQUIRE(reader.get(8) == 0b10101011);
            reader.seek(11); REQUIRE(reader.get(8) == 0b01010111);
            reader.seek(12); REQUIRE(reader.get(8) == 0b10101111);
            reader.seek(13); REQUIRE(reader.get(8) == 0b01011110);
            reader.seek(14); REQUIRE(reader.get(8) == 0b10111100);
            reader.seek(15); REQUIRE(reader.get(8) == 0b01111000);
            reader.seek(16); REQUIRE(reader.get(8) == 0b11110000);
            reader.seek(17); REQUIRE(reader.get(8) == 0b11100000);
            reader.seek(18); REQUIRE(reader.get(8) == 0b11000000);
            reader.seek(19); REQUIRE(reader.get(8) == 0b10000001);
            reader.seek(20); REQUIRE(reader.get(8) == 0b00000011);
            reader.seek(21); REQUIRE(reader.get(8) == 0b00000110);
            reader.seek(22); REQUIRE(reader.get(8) == 0b00001100);
            reader.seek(23); REQUIRE(reader.get(8) == 0b00011001);
            reader.seek(24); REQUIRE(reader.get(8) == 0b00110011);
        }

        SECTION("Unaligned (automatic)")
        {
            io::BitReader reader(from_bits(
                {0b11001100, 0b10101010, 0b11110000, 0b00110011}));
            for (auto i : algo::range(32))
            {
                reader.seek(i);
                INFO("Position: " << reader.tell());
                auto mask = (1ull << (32 - i)) - 1;
                auto expected = 0b11001100101010101111000000110011ull & mask;
                REQUIRE(reader.get(32 - i) == expected);
            }
        }

        SECTION("Seeking beyond EOF throws errors")
        {
            io::BitReader reader(from_bits(
                {0b11001100, 0b10101010, 0b11110000, 0b00110011}));
            for (auto i : algo::range(32))
            {
                reader.seek(31);
                REQUIRE_THROWS(reader.skip(2 + i));
                REQUIRE(reader.tell() == 31);
            }
            for (auto i : algo::range(32))
                REQUIRE_THROWS(reader.seek(33 + i));
        }
    }

    SECTION("Skipping")
    {
        io::BitReader reader(from_bits(
            {0b11001100, 0b10101010, 0b11110000, 0b00110011}));
        reader.seek(0);
        REQUIRE(reader.get(8) == 0b11001100);
        reader.skip(-7);
        REQUIRE(reader.get(8) == 0b10011001);
    }

    SECTION("Stream-based buffering")
    {
        SECTION("Aligned")
        {
            io::MemoryStream stream("\xFF\x01"_b);
            io::BitReader reader(stream);
            REQUIRE(reader.get(8) == 0xFF);
            REQUIRE(stream.read_u8() == 1);
        }

        SECTION("Unaligned")
        {
            io::MemoryStream stream("\xFF\x80\x02"_b);
            io::BitReader reader(stream);
            REQUIRE(reader.get(8) == 0xFF);
            REQUIRE(reader.get(1) == 0x01);
            REQUIRE(stream.read_u8() == 2);
        }

        SECTION("Interleaving")
        {
            io::MemoryStream stream("\xFF\xC0\x02\x01\xFF"_b);
            io::BitReader reader(stream);
            REQUIRE(reader.get(8) == 0xFF);
            REQUIRE(reader.get(1) == 0x01);
            REQUIRE(stream.read_u8() == 2);
            REQUIRE(reader.get(7) == 0x40);
            REQUIRE(stream.read_u8() == 1);
            REQUIRE(reader.get(8) == 0xFF);
        }

        SECTION("Interleaving with seeking")
        {
            io::MemoryStream stream("\xFF\xC0\x02\x01\xFF"_b);
            io::BitReader reader(stream);
            REQUIRE(reader.get(8) == 0xFF);
            REQUIRE(reader.get(1) == 0x01);
            REQUIRE(stream.read_u8() == 2);
            reader.seek(stream.tell() << 3);
            REQUIRE(reader.get(8) == 1);
            REQUIRE(stream.read_u8() == 0xFF);
        }
    }

    SECTION("Reading beyond EOF retracts to prior offset")
    {
        SECTION("Byte-aligned without byte retrieval")
        {
            io::BitReader reader("\x00"_b);
            reader.get(7);
            reader.get(1);
            REQUIRE(reader.eof());
            REQUIRE_THROWS(reader.get(1));
            REQUIRE(reader.eof());
            REQUIRE(reader.tell() == 8);
        }

        SECTION("Byte-aligned with byte retrieval")
        {
            io::BitReader reader("\x00\xFF"_b);
            reader.get(7);
            reader.get(1);
            REQUIRE_THROWS(reader.get(16));
            REQUIRE(!reader.eof());
            REQUIRE(reader.tell() == 8);
            REQUIRE(reader.get(8) == 0xFF);
        }

        SECTION("Byte-unaligned without byte retrieval")
        {
            io::BitReader reader("\x01"_b);
            reader.get(7);
            REQUIRE_THROWS(reader.get(2));
            REQUIRE(!reader.eof());
            REQUIRE(reader.tell() == 7);
            REQUIRE(reader.get(1));
        }

        SECTION("Byte-unaligned with byte retrieval")
        {
            io::BitReader reader("\x01\x00"_b);
            reader.get(7);
            REQUIRE_THROWS(reader.get(10));
            REQUIRE(!reader.eof());
            REQUIRE(reader.tell() == 7);
            REQUIRE(reader.get(1));
        }
    }
}
