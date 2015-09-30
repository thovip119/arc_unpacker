#include "fmt/png/png_image_decoder.h"
#include <png.h>
#include "err.h"
#include "util/range.h"

using namespace au;
using namespace au::fmt::png;

static const bstr magic = "\x89PNG"_b;

static void png_read_data(
    png_structp png_ptr, png_bytep data, png_size_t size)
{
    io::IO *io = reinterpret_cast<io::IO*>(png_get_io_ptr(png_ptr));
    io->read(data, size);
}

bool PngImageDecoder::is_recognized_internal(File &file) const
{
    return file.io.read(magic.size()) == magic;
}

pix::Grid PngImageDecoder::decode_internal(File &file) const
{
    png_structp png_ptr = png_create_read_struct(
        PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr)
        throw std::logic_error("Failed to create PNG read structure");

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        throw std::logic_error("Failed to create PNG info structure");

    png_set_read_fn(png_ptr, &file.io, &png_read_data);
    png_read_png(
        png_ptr,
        info_ptr,
        PNG_TRANSFORM_GRAY_TO_RGB
            | PNG_TRANSFORM_STRIP_16
            | PNG_TRANSFORM_PACKING
            | PNG_TRANSFORM_BGR
            | PNG_TRANSFORM_EXPAND,
        nullptr);

    int color_type;
    int bits_per_channel;
    png_uint_32 width, height;
    png_get_IHDR(
        png_ptr, info_ptr,
        &width, &height,
        &bits_per_channel, &color_type,
        nullptr, nullptr, nullptr);
    if (bits_per_channel != 8)
        throw err::UnsupportedBitDepthError(bits_per_channel);

    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

    pix::Format format;
    if (color_type == PNG_COLOR_TYPE_RGB)
        format = pix::Format::BGR888;
    else if (color_type == PNG_COLOR_TYPE_RGBA)
        format = pix::Format::BGRA8888;
    else if (color_type == PNG_COLOR_TYPE_GRAY)
        format = pix::Format::Gray8;
    else
        throw err::NotSupportedError("Bad pixel format");

    bstr data;
    data.reserve(width * height * format_to_bpp(format));
    for (auto y : util::range(height))
        data += bstr(row_pointers[y], width * format_to_bpp(format));
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

    return pix::Grid(width, height, data, format);
}

static auto dummy = fmt::Registry::add<PngImageDecoder>("png/png");