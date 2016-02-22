#include "io/base_stream.h"
#include <memory>
#include "err.h"

using namespace au;
using namespace au::io;

BaseStream::~BaseStream() {}

size_t BaseStream::left() const
{
    return size() - pos();
}

io::BaseStream &BaseStream::skip(const int offset)
{
    return seek(pos() + offset);
}

io::BaseStream &BaseStream::peek(
    const size_t offset, std::function<void()> func)
{
    const auto old_pos = pos();
    seek(offset);
    try
    {
        func();
        seek(old_pos);
        return *this;
    }
    catch (...)
    {
        seek(old_pos);
        throw;
    }
}
