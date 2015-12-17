#include "fmt/nscripter/sar_archive_decoder.h"
#include "test_support/catch.h"
#include "test_support/decoder_support.h"
#include "test_support/file_support.h"

using namespace au;
using namespace au::fmt::nscripter;

static const std::string dir = "tests/fmt/nscripter/files/sar/";

static void do_test(const std::string &input_path)
{
    const std::vector<std::shared_ptr<io::File>> expected_files
    {
        tests::stub_file("abc.txt", "123"_b),
        tests::stub_file("dir/another.txt", "AAAAAAAAAAAAAAAA"_b),
    };
    const SarArchiveDecoder decoder;
    const auto input_file = tests::file_from_path(dir + input_path);
    const auto actual_files = tests::unpack(decoder, *input_file);
    tests::compare_files(expected_files, actual_files, true);
}

TEST_CASE("NScripter SAR archives", "[fmt]")
{
    do_test("test.sar");
}
