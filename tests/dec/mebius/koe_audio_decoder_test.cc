// Copyright (C) 2016 by rr-
//
// This file is part of arc_unpacker.
//
// arc_unpacker is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.
//
// arc_unpacker is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with arc_unpacker. If not, see <http://www.gnu.org/licenses/>.

#include "dec/mebius/koe_audio_decoder.h"
#include "algo/binary.h"
#include "enc/microsoft/wav_audio_encoder.h"
#include "test_support/audio_support.h"
#include "test_support/catch.h"
#include "test_support/decoder_support.h"
#include "test_support/file_support.h"

using namespace au;
using namespace au::dec::mebius;

static std::unique_ptr<io::File> encode(
    const res::Audio &input_audio, const bstr &key, const std::string &ext)
{
    res::Audio audio_copy(input_audio);
    audio_copy.samples = algo::unxor(audio_copy.samples, key);
    Logger dummy_logger;
    dummy_logger.mute();
    enc::microsoft::WavAudioEncoder encoder;
    auto output_file = encoder.encode(dummy_logger, audio_copy, "test");
    output_file->path.change_extension(ext);
    return output_file;
}

static void do_test(const std::string &ext, const bstr &key)
{
    KoeAudioDecoder decoder;
    decoder.plugin_manager.set("snow");
    const auto expected_audio = tests::get_test_audio();
    const auto input_file = encode(expected_audio, key, ext);
    const auto actual_audio = tests::decode(decoder, *input_file);
    tests::compare_audio(actual_audio, expected_audio);
}

TEST_CASE("Studio Mebius BGM/KOE/MSE audio", "[dec]")
{
    SECTION("BGM")
    {
        do_test(
            "bgm",
            "\x16\x83\x0A\x4D\x6E\x39\xBF\xD8\x9C\x2B\x9E\x9F\xAE\x13\x8C\x63"
            "\xBE\x53\x95\x2E\x61\xB3\xFC\x26\x1C\xA5\xBF\x99\x69\x29\x3C\x99"
            "\xD7\x1E\x8B\xFD\xBD\x98\xC9\x12\x0E\x93\x5F\x59\x4E\x89\x7B\x26"
            "\xA7\x53\x50\xF1\xB6\x52\x5A\xA6\x6D\xCD\x20\xD9\xC3\x82\xCB\x21"
            "\xFD\x4D\x8B\xFA\x49\xEA\xC3\x7C\x81\x42\xEE\x38\xC3\xAB\xE0\x1A"
            "\xBD\x9F\xB4\x98\x4F\x59\x60\x8D\xEE\x41\x92\x87\xEB\x30\x2A\x66"
            "\xF4\x69\xA2\xA4\x0F\x53\xB6\x04\x4E\x4A\xB8\x9E\x8B\x23\xE0\xF8"
            "\xE6\xA2\x1F\xA4\x46\x9B\x34\x09\x33\xE3\x0B\x66\xB7\xCC\x1F\xA9"
            "\x1F\xEE\xF6\x1D\x42\x55\xE6\x19\x44\x61\xBA\xAE\x57\xFC\x6D\x08"
            "\xFE\x6B\x84\x5C\x69\x50\xD0\xCC\xC3\xBC\x92\x7C\x33\x59\x4D\x2D"
            "\x50\x00\x47\xCE\x4C\xDB\x7A\xB0\x25\x61\x07\x55\x8A\xAD\x50\x0B"
            "\xD3\x2D\x6C\xC9\x39\x94\x82\x0F\x9B\xF9\x45\x95\x1C\xBA\xA5\xB9"
            "\xD2\x60\xE3\xE3\xC7\x34\xAA\x43\x27\xC7\xC2\x3D\xBD\x8A\xA6\x4B"
            "\xA9\x3F\xEF\xBB\x6B\xE4\x6B\x89\x2A\xE9\xD1\xC0\xE5\x3A\xED\x1A"
            "\x61\xF9\xB3\xCC\x03\x0F\x82\xCD\x74\x36\x2A\xD8\x3E\x4E\xE0\x17"
            "\x37\x1B\x41\xC2\xE8\xA7\x81\x7C\xD3\x02\xFD\x51\xB4\x02\x43\x9E"
            ""_b);
    }

    SECTION("KOE")
    {
        do_test(
            "koe",
            "\xCE\xC5\x94\xE8\xD5\x7F\xEB\xF4\x96\xCA\xAA\x80\xAC\x45\x60\x58"
            "\x71\x50\xDD\x72\x20\x39\x08\x73\xFE\x46\x07\xC5\x78\x77\xC0\x23"
            "\x49\x9F\xFC\xD1\x9A\x0F\x99\x7F\x3E\x7B\xAE\xF4\x66\xEE\x14\x94"
            "\x75\xD0\x0E\xD8\x64\x60\xB4\x3B\x40\x33\xC3\x4E\x40\x0E\xE4\x6C"
            "\x8D\x26\xBA\xB0\x17\xA5\x40\xB7\x27\x80\x79\x58\x92\xF8\x79\x3E"
            "\x2A\xDA\xC8\x29\xD3\x43\x66\xC0\xE5\x16\xAB\x25\x35\x68\x60\xC1"
            "\x77\x6E\x2B\x0E\x50\x58\xDC\xAE\xC5\x97\xE9\x27\xE1\xF3\x03\xA2"
            "\x43\x77\x13\xF0\xEC\x8C\x40\xB4\x7F\x62\x8B\x84\x40\x68\xAF\xD2"
            "\x10\xF2\xFE\x79\x3D\x63\x3D\xB4\x43\x65\xB8\x5F\x77\x13\x32\x56"
            "\xA4\x93\xC9\x3D\x9F\x89\xFE\x0B\xD0\x6C\x81\x2D\x3F\x94\xDD\x16"
            "\x1A\x12\x3A\x83\xC7\x26\xC3\xE0\xFE\xF1\xEC\x82\x6C\xAF\xA0\x30"
            "\xEB\xFD\x1A\xA1\xD0\xA9\xEC\x7A\x52\x6D\x83\xE4\x84\x97\x8F\x44"
            "\x89\x0E\xB7\xC1\x4F\xA1\x89\x8C\x09\xA6\xE5\x98\x4C\xC3\x7A\xCA"
            "\xE6\x6D\x06\xB7\x5B\x82\x6C\x02\x2E\x03\x57\xF3\xD6\x3D\x79\x5B"
            "\x87\x0E\xA2\x4E\xA6\xFE\xB8\x56\xA6\x55\xD3\x2B\x17\x6F\x7F\x84"
            "\x16\xF7\xE6\x99\x8A\x4E\x73\xDE\x45\x2E\x1A\xA6\xEF\x78\x67\x1A"
            ""_b);
    }

    SECTION("MSE")
    {
        do_test(
            "mse",
            "\x40\xBA\x96\x7E\x07\xE1\x92\x95\x7E\x95\x17\x47\x3D\x1C\x08\x94"
            "\x02\xA5\x39\x7D\x95\xCB\xD8\x57\x09\x52\x67\xFD\x86\x57\xFD\x81"
            "\x04\xB9\x70\x54\x14\xC7\x8E\xA5\xA0\x11\xF5\xE2\xC5\x6E\xDB\x01"
            "\xA8\x8C\xA9\x25\xEB\x98\xD6\xBA\xAD\xD9\x62\x00\xAE\x50\xCA\x3E"
            "\x04\xAA\xF7\x98\xF9\x2C\xAE\xA4\x11\xCE\xF8\xCC\xAD\xB8\x07\xA5"
            "\xE8\xDF\x28\x2A\xA1\xE4\x81\x1F\x35\x7B\x4C\x7F\xFA\x04\x75\x31"
            "\x77\x0D\xD1\x79\xD3\x68\x2C\xDB\x16\x27\xBB\xD5\x2A\xFB\x2C\xBC"
            "\xB1\x70\xE2\x1C\xA8\xF6\x1E\x53\xDA\xA0\x89\xED\xB9\x25\x0A\x55"
            "\x08\x01\x37\xE7\x6B\xB4\xDB\x18\xE2\x13\x6B\x8E\x25\x98\x40\x05"
            "\xE7\x32\x1F\x4B\xA9\x7C\xC8\x24\x51\x54\x16\xFD\x6F\xC8\x67\x2B"
            "\xD2\xCD\x78\x18\xC2\xB0\xB6\xAA\x25\xB2\x4E\xCD\x3A\xD7\x0D\x43"
            "\x64\xBD\x35\x52\xFC\x07\x70\x67\xBE\x48\xFB\xA9\xD2\x67\xC3\xB8"
            "\x6A\xDC\x76\x04\x0E\xDD\xD3\xEB\x7A\x49\x39\xAC\xBD\xE5\x31\xBB"
            "\x71\xCC\x91\x8A\xB1\x09\x57\xF3\x39\xD2\x5E\xAB\x4F\x5F\x24\x86"
            "\xD5\x3D\xA8\xE7\x36\x23\x21\x32\x76\x3C\x98\x0A\x34\x51\x1E\xB8"
            "\x51\x40\x34\x93\x0B\x5C\x94\x24\x50\x6A\x72\x85\x04\xF1\xE5\x20"
            ""_b);
    }
}
