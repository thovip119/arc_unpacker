#include "fmt/yuka_script/ykg_image_decoder.h"
#include "algo/range.h"
#include "err.h"
#include "fmt/png/png_image_decoder.h"

using namespace au;
using namespace au::fmt::yuka_script;

namespace
{
    struct Header final
    {
        bool encrypted;
        size_t data_offset;
        size_t data_size;
        size_t regions_offset;
        size_t regions_size;
    };

    struct Region final
    {
        size_t width;
        size_t height;
        size_t x;
        size_t y;
    };
}

static const bstr magic = "YKG000"_b;

static std::unique_ptr<Header> read_header(io::Stream &input)
{
    auto header = std::make_unique<Header>();
    header->encrypted = input.read_u16_le() > 0;

    size_t header_size = input.read_u32_le();
    if (header_size != 64)
        throw err::NotSupportedError("Unexpected header size");
    input.skip(28);

    header->data_offset = input.read_u32_le();
    header->data_size = input.read_u32_le();
    input.skip(8);

    header->regions_offset = input.read_u32_le();
    header->regions_size = input.read_u32_le();
    return header;
}

static std::vector<std::unique_ptr<Region>> read_regions(
    io::Stream &input, Header &header)
{
    std::vector<std::unique_ptr<Region>> regions;

    input.seek(header.regions_offset);
    size_t region_count = header.regions_size / 64;
    for (auto i : algo::range(region_count))
    {
        auto region = std::make_unique<Region>();
        region->x = input.read_u32_le();
        region->y = input.read_u32_le();
        region->width = input.read_u32_le();
        region->height = input.read_u32_le();
        input.skip(48);
        regions.push_back(std::move(region));
    }
    return regions;
}

static res::Image decode_png(io::File &input_file, Header &header)
{
    input_file.stream.seek(header.data_offset);
    bstr data = input_file.stream.read(header.data_size);
    if (data.empty())
        throw std::logic_error("File doesn't contain any data");
    if (data.substr(1, 3) != "GNP"_b)
    {
        throw err::NotSupportedError(
            "Decoding non-PNG based YKG images is not supported");
    }
    data[1] = 'P';
    data[2] = 'N';
    data[3] = 'G';

    io::File png_file(input_file.path, data);
    const fmt::png::PngImageDecoder png_image_decoder;
    return png_image_decoder.decode(png_file);
}

bool YkgImageDecoder::is_recognized_impl(io::File &input_file) const
{
    return input_file.stream.read(magic.size()) == magic;
}

res::Image YkgImageDecoder::decode_impl(io::File &input_file) const
{
    input_file.stream.skip(magic.size());

    auto header = read_header(input_file.stream);
    if (header->encrypted)
    {
        throw err::NotSupportedError(
            "Decoding encrypted YKG images is not supported");
    }

    read_regions(input_file.stream, *header);
    return decode_png(input_file, *header);
}

static auto dummy = fmt::register_fmt<YkgImageDecoder>("yuka-script/ykg");
