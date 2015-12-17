#include "fmt/real_live/g00_image_decoder.h"
#include "test_support/catch.h"
#include "test_support/decoder_support.h"
#include "test_support/file_support.h"
#include "test_support/image_support.h"

using namespace au;
using namespace au::fmt::real_live;

static const std::string dir = "tests/fmt/real_live/files/g00/";

static void do_test(
    const std::string &input_path, const std::string &expected_path)
{
    const G00ImageDecoder decoder;
    const auto input_file = tests::file_from_path(dir + input_path);
    const auto expected_file = tests::file_from_path(dir + expected_path);
    const auto actual_image = tests::decode(decoder, *input_file);
    tests::compare_images(*expected_file, actual_image);
}

TEST_CASE("RealLive G00 images", "[fmt]")
{
    SECTION("Version 0")
    {
        do_test("ayu_02.g00", "ayu_02-out.png");
    }

    SECTION("Version 1")
    {
        do_test("ayu_05.g00", "ayu_05-out.png");
    }

    SECTION("Version 2")
    {
        do_test("AYU_03.g00", "AYU_03-out.png");
    }
}
