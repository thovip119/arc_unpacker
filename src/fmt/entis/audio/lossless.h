#pragma once

#include "fmt/entis/common/decoder.h"
#include "fmt/entis/audio/common.h"

namespace au {
namespace fmt {
namespace entis {
namespace audio {

    class LosslessAudioDecoder final : public AudioDecoderImpl
    {
    public:
        LosslessAudioDecoder(const MioHeader &header);
        ~LosslessAudioDecoder();

        virtual bstr process_chunk(const MioChunk &chunk) override;

    private:
        struct Priv;
        std::unique_ptr<Priv> p;
    };

} } } }