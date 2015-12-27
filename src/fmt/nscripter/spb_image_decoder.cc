#include "fmt/nscripter/spb_image_decoder.h"
#include "algo/range.h"
#include "io/bit_reader.h"

using namespace au;
using namespace au::fmt::nscripter;

static res::Image decode_image(
    size_t width, size_t height, io::IBitReader &bit_reader)
{
    res::Image output(width, height);
    for (auto &c : output)
        c.a = 0xFF;

    bstr channel_data(width * height);

    for (int rgb = 2; rgb >= 0; rgb--)
    {
        u8 *channel_ptr = channel_data.get<u8>();
        const u8 *channel_end = channel_ptr + channel_data.size();

        u8 ch = bit_reader.get(8);
        if (channel_ptr >= channel_end) break;
        *channel_ptr++ = ch;

        while (channel_ptr < channel_end)
        {
            size_t t = bit_reader.get(3);
            if (t == 0)
            {
                if (channel_ptr >= channel_end)
                    break;
                *channel_ptr++ = ch;
                if (channel_ptr >= channel_end)
                    break;
                *channel_ptr++ = ch;
                if (channel_ptr >= channel_end)
                    break;
                *channel_ptr++ = ch;
                if (channel_ptr >= channel_end)
                    break;
                *channel_ptr++ = ch;
                continue;
            }

            size_t mask = t == 7 ? bit_reader.get(1) + 1 : t + 2;

            for (auto i : algo::range(4))
            {
                if (mask == 8)
                {
                    ch = bit_reader.get(8);
                }
                else
                {
                    t = bit_reader.get(mask);
                    if (t & 1)
                        ch += (t >> 1) + 1;
                    else
                        ch -= (t >> 1);
                }
                if (channel_ptr >= channel_end)
                    break;
                *channel_ptr++ = ch;
            }
        }

        const u8 *p = channel_data.get<const u8>();
        for (auto y : algo::range(0, height))
        {
            if (y & 1)
            {
                for (auto x : algo::range(width))
                    output.at(width - 1 - x, y)[2 - rgb] = *p++;
            }
            else
            {
                for (auto x : algo::range(width))
                    output.at(x, y)[2 - rgb] = *p++;
            }
        }
    }

    return output;
}

bool SpbImageDecoder::is_recognized_impl(io::File &input_file) const
{
    if (!input_file.path.has_extension("bmp"))
        return false;
    const auto width = input_file.stream.read_u16_be();
    const auto height = input_file.stream.read_u16_be();
    if (height == 0 || width == 0)
        return false;
    if (width > 5000 || height > 5000)
        return false;
    return true;
}

res::Image SpbImageDecoder::decode_impl(
    const Logger &logger, io::File &input_file) const
{
    const auto width = input_file.stream.read_u16_be();
    const auto height = input_file.stream.read_u16_be();
    io::MsbBitReader bit_reader(input_file.stream.read_to_eof());
    return decode_image(width, height, bit_reader);
}

static auto dummy = fmt::register_fmt<SpbImageDecoder>("nscripter/spb");
