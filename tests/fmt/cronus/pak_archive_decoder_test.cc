#include "fmt/cronus/pak_archive_decoder.h"
#include "test_support/archive_support.h"
#include "test_support/catch.hh"
#include "test_support/file_support.h"

using namespace au;
using namespace au::fmt::cronus;

static void do_test(const std::string &input_path)
{
    std::vector<std::shared_ptr<File>> expected_files
    {
        tests::stub_file("123.txt", "1234567890"_b),
        tests::stub_file("abc.txt", "abcdefghijklmnopqrstuvwxyz"_b),
    };

    PakArchiveDecoder decoder;
    auto actual_files = tests::unpack_to_memory(input_path, decoder);
    tests::compare_files(expected_files, actual_files, true);
}

TEST_CASE("Cronus PAK unencrypted uncompressed archives", "[fmt]")
{
    do_test("tests/fmt/cronus/files/pak/unencrypted-uncompressed.pak");
}

TEST_CASE("Cronus PAK encrypted uncompressed archives", "[fmt]")
{
    do_test("tests/fmt/cronus/files/pak/encrypted-uncompressed.pak");
}

TEST_CASE("Cronus PAK encrypted compressed archives", "[fmt]")
{
    do_test("tests/fmt/cronus/files/pak/encrypted-compressed.pak");
}