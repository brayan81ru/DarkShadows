#include "DSTexture.h"
#include <fstream>
#include <algorithm>
#include <stdexcept>

// STB Image implementation
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "../third_party/stb/stb_image.h"

// STB Image Resize implementation
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../third_party/stb/stb_image_resize.h"

namespace DSEngine {
    // Compression implementation using stb_dxt
    bool DSTexture::CompressToDXT(Format dxtFormat, CompressionQuality quality) {
        if (m_mipmaps.empty() || (dxtFormat != Format::DXT1 && dxtFormat != Format::DXT5)) {
            return false;
        }

        // If already in target format, return success
        if (m_format == dxtFormat) {
            return true;
        }

        // Must be RGBA8 to compress to DXT
        if (m_format != Format::RGBA8) {
            // Convert to RGBA8 first
            //if (!ConvertFormat(Format::RGBA8)) {
            //    return false;
            //}
            return false;
        }

        std::vector<MipLevel> compressedMips;
        compressedMips.reserve(m_mipmaps.size());

        for (MipLevel &mip : m_mipmaps) {
            MipLevel newMip;
            newMip.width = mip.width;
            newMip.height = mip.height;

            // Calculate compressed size (4x4 blocks)
            uint32_t blocksWide = (mip.width + 3) / 4;
            uint32_t blocksHigh = (mip.height + 3) / 4;
            size_t compressedSize = blocksWide * blocksHigh * (dxtFormat == Format::DXT1 ? 8 : 16);
            newMip.data.resize(compressedSize);

            // Compress using appropriate method
            bool success = false;
            switch (dxtFormat) {
                case Format::DXT1:
                    success = CompressDXT1(mip, newMip, quality);
                    break;
                case Format::DXT5:
                    success = CompressDXT5(mip, newMip, quality);
                    break;
                default:
                    return false;
            }

            if (!success) return false;
            compressedMips.push_back(std::move(newMip));
        }

        m_mipmaps = std::move(compressedMips);
        m_format = dxtFormat;
        return true;
    }

    bool DSTexture::CompressDXT1(MipLevel& source, MipLevel& dest, CompressionQuality quality) {
        const int alpha = 1; // STB_DXT flag for DXT1 with alpha
        const int mode = (quality == CompressionQuality::FAST) ? STB_DXT_NORMAL : STB_DXT_HIGHQUAL;

        stb_compress_dxt_block(
            dest.data.data(),
            source.data.data(),
            alpha,
            mode
        );

        // Process all 4x4 blocks
        uint32_t blocksWide = (source.width + 3) / 4;
        uint32_t blocksHigh = (source.height + 3) / 4;

        for (uint32_t by = 0; by < blocksHigh; by++) {
            for (uint32_t bx = 0; bx < blocksWide; bx++) {
                uint8_t blockPixels[4*4*4];

                // Extract 4x4 RGBA block (with edge padding)
                for (int y = 0; y < 4; y++) {
                    for (int x = 0; x < 4; x++) {
                        int sx = bx*4 + x;
                        int sy = by*4 + y;

                        // Clamp to texture dimensions
                        sx = std::min(sx, (int)source.width-1);
                        sy = std::min(sy, (int)source.height-1);

                        const uint8_t* src = source.data.data() + (sy * source.width + sx) * 4;
                        uint8_t* dst = blockPixels + (y*4 + x) * 4;

                        dst[0] = src[0];
                        dst[1] = src[1];
                        dst[2] = src[2];
                        dst[3] = src[3];
                    }
                }

                // Compress the block
                uint8_t* output = dest.data.data() + (by * blocksWide + bx) * 8;
                stb_compress_dxt_block(output, blockPixels, alpha, mode);
            }
        }

        return true;
    }

    bool DSTexture::CompressDXT5(MipLevel& source, MipLevel& dest, CompressionQuality quality) {
        const int mode = (quality == CompressionQuality::FAST) ? STB_DXT_NORMAL : STB_DXT_HIGHQUAL;

        // Process all 4x4 blocks
        uint32_t blocksWide = (source.width + 3) / 4;
        uint32_t blocksHigh = (source.height + 3) / 4;

        for (uint32_t by = 0; by < blocksHigh; by++) {
            for (uint32_t bx = 0; bx < blocksWide; bx++) {
                uint8_t blockPixels[4*4*4];

                // Extract 4x4 RGBA block (with edge padding)
                for (int y = 0; y < 4; y++) {
                    for (int x = 0; x < 4; x++) {
                        int sx = bx*4 + x;
                        int sy = by*4 + y;

                        // Clamp to texture dimensions
                        sx = std::min(sx, (int)source.width-1);
                        sy = std::min(sy, (int)source.height-1);

                        const uint8_t* src = source.data.data() + (sy * source.width + sx) * 4;
                        uint8_t* dst = blockPixels + (y*4 + x) * 4;

                        dst[0] = src[0];
                        dst[1] = src[1];
                        dst[2] = src[2];
                        dst[3] = src[3];
                    }
                }

                // Compress the block (DXT5 is two DXT blocks: alpha + color)
                uint8_t* output = dest.data.data() + (by * blocksWide + bx) * 16;
                stb_compress_dxt_block(output, blockPixels, 0, mode); // Alpha block
                stb_compress_dxt_block(output+8, blockPixels, 0, mode); // Color block
            }
        }

        return true;
    }

    bool DSTexture::IsDXTCompressed() const {
        return m_format == Format::DXT1 || m_format == Format::DXT5;
    }

    // Update IsFormatCompressed helper
    bool DSTexture::IsFormatCompressed(Format format) {
        return format == Format::DXT1 || format == Format::DXT5;
    }

    DSTexture::DSTexture()
        : m_format(Format::UNKNOWN), m_flags(0) {}

    DSTexture::~DSTexture() {
        // Automatic cleanup via vector destructors
    }

    // Static creator methods
    DSTexture* DSTexture::CreateFromFile(const std::string& path, bool flipVertically) {
        DSTexture* texture = new DSTexture();
        if (!texture->LoadFromFile(path)) {
            delete texture;
            return nullptr;
        }
        return texture;
    }

    DSTexture* DSTexture::CreateEmpty(uint32_t width, uint32_t height, Format format) {
        DSTexture* texture = new DSTexture();
        texture->m_format = format;

        MipLevel baseLevel;
        baseLevel.width = width;
        baseLevel.height = height;
        baseLevel.data.resize(width * height * GetPixelSize(format));

        texture->m_mipmaps.push_back(std::move(baseLevel));
        return texture;
    }

    DSTexture* DSTexture::CreateFromMemory(const uint8_t* data, uint32_t width, uint32_t height, Format format) {
        DSTexture* texture = new DSTexture();
        texture->m_format = format;

        MipLevel baseLevel;
        baseLevel.width = width;
        baseLevel.height = height;
        const size_t dataSize = width * height * GetPixelSize(format);
        baseLevel.data.assign(data, data + dataSize);

        texture->m_mipmaps.push_back(std::move(baseLevel));
        return texture;
    }

    // File operations
    bool DSTexture::SaveToFile(const std::string& path) const {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        // Prepare header
        DSTHeader header;
        header.width = GetWidth();
        header.height = GetHeight();
        header.format = static_cast<uint16_t>(m_format);
        header.mipLevels = GetMipLevels();

        // Add compression flag if needed
        if (IsDXTCompressed()) {
            header.flags |= 0x1; // Set compression flag
        }

        // Write header
        file.write(reinterpret_cast<const char*>(&header), sizeof(DSTHeader));

        // Calculate data offset (header + mip info array)
        uint32_t dataOffset = sizeof(DSTHeader) + (header.mipLevels * sizeof(DSTMipInfo));

        // Write mipmap info and accumulate data offset
        for (const auto& mip : m_mipmaps) {
            DSTMipInfo info;
            info.width = mip.width;
            info.height = mip.height;
            info.dataSize = static_cast<uint32_t>(mip.data.size());
            info.dataOffset = dataOffset;

            file.write(reinterpret_cast<const char*>(&info), sizeof(DSTMipInfo));
            dataOffset += info.dataSize;
        }

        // Write pixel data
        for (const auto& mip : m_mipmaps) {
            file.write(reinterpret_cast<const char*>(mip.data.data()), mip.data.size());
        }

        return true;
    }

    bool DSTexture::LoadFromFile(const std::string& path) {
        // Try loading as DST first
        std::ifstream file(path, std::ios::binary);
        if (file.is_open()) {
            DSTHeader header;
            file.read(reinterpret_cast<char*>(&header), sizeof(DSTHeader));

            // Verify magic number
            if (header.magic[0] != 'D' || header.magic[1] != 'S' || header.magic[2] != 'T') {
                file.close();
                return LoadFromSTB(path, true);
            }

            m_format = static_cast<Format>(header.format);
            m_flags = header.flags;

            // Read mipmap info
            std::vector<DSTMipInfo> mipInfos(header.mipLevels);
            file.read(reinterpret_cast<char*>(mipInfos.data()), header.mipLevels * sizeof(DSTMipInfo));

            // Read mipmap data
            m_mipmaps.resize(header.mipLevels);
            for (uint32_t i = 0; i < header.mipLevels; ++i) {
                MipLevel& mip = m_mipmaps[i];
                mip.width = mipInfos[i].width;
                mip.height = mipInfos[i].height;
                mip.data.resize(mipInfos[i].dataSize);

                file.seekg(mipInfos[i].dataOffset);
                file.read(reinterpret_cast<char*>(mip.data.data()), mip.data.size());
            }

            return true;
        }

        // Fall back to STB loader
        return LoadFromSTB(path, true);
    }

    // Mipmap operations
    bool DSTexture::GenerateMipmaps() {
        if (m_mipmaps.empty()) return false;

        // Clear existing mipmaps (except base level)
        if (m_mipmaps.size() > 1) {
            m_mipmaps.erase(m_mipmaps.begin() + 1, m_mipmaps.end());
        }

        const MipLevel& base = m_mipmaps[0];
        uint32_t width = base.width;
        uint32_t height = base.height;

        while (width > 1 || height > 1) {
            width = std::max(1u, width / 2);
            height = std::max(1u, height / 2);

            MipLevel newLevel;
            newLevel.width = width;
            newLevel.height = height;
            newLevel.data.resize(width * height * GetPixelSize(m_format));

            const MipLevel& prevLevel = m_mipmaps.back();
            stbir_resize_uint8(
                prevLevel.data.data(), prevLevel.width, prevLevel.height, 0,
                newLevel.data.data(), width, height, 0,
                GetChannelCount(m_format)
            );

            m_mipmaps.push_back(std::move(newLevel));
        }

        return true;
    }

    bool DSTexture::SetMipLevel(uint32_t level, const void* data, size_t size) {
        if (!ValidateMipLevel(level) || !data) return false;

        MipLevel& mip = m_mipmaps[level];
        if (size != mip.width * mip.height * GetPixelSize(m_format)) {
            return false;
        }

        std::memcpy(mip.data.data(), data, size);
        return true;
    }

    bool DSTexture::RemoveMipmaps() {
        if (m_mipmaps.size() <= 1) return false;
        m_mipmaps.resize(1);
        return true;
    }

    // Accessors
    uint32_t DSTexture::GetWidth(uint32_t mipLevel) const {
        return ValidateMipLevel(mipLevel) ? m_mipmaps[mipLevel].width : 0;
    }

    uint32_t DSTexture::GetHeight(uint32_t mipLevel) const {
        return ValidateMipLevel(mipLevel) ? m_mipmaps[mipLevel].height : 0;
    }

    const uint8_t* DSTexture::GetPixels(uint32_t mipLevel) const {
        return ValidateMipLevel(mipLevel) ? m_mipmaps[mipLevel].data.data() : nullptr;
    }

    size_t DSTexture::GetPixelDataSize(uint32_t mipLevel) const {
        return ValidateMipLevel(mipLevel) ? m_mipmaps[mipLevel].data.size() : 0;
    }

    // Static format helpers
    uint32_t DSTexture::GetChannelCount(Format format) {
        switch (format) {
            case Format::RGB8: return 3;
            case Format::RGBA8: return 4;
            default: return 0;
        }
    }

    size_t DSTexture::GetPixelSize(Format format) {
        switch (format) {
            case Format::RGB8: return 3;
            case Format::RGBA8: return 4;
            default: return 0;
        }
    }

    // Private methods
    bool DSTexture::ValidateMipLevel(uint32_t level) const {
        return level < m_mipmaps.size();
    }

    bool DSTexture::LoadFromSTB(const std::string& path, bool flipVertically) {
        stbi_set_flip_vertically_on_load(flipVertically);

        int width, height, channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        if (!data) return false;

        // Determine format
        switch (channels) {
            case 3: m_format = Format::RGB8; break;
            case 4: m_format = Format::RGBA8; break;
            default:
                stbi_image_free(data);
                return false;
        }

        // Create base mip level
        MipLevel baseLevel;
        baseLevel.width = width;
        baseLevel.height = height;
        const size_t dataSize = width * height * channels;
        baseLevel.data.assign(data, data + dataSize);
        m_mipmaps.push_back(std::move(baseLevel));

        stbi_image_free(data);
        return true;
    }

    // DXT Decompression Implementation
    void DSTexture::DecompressDXT1Block(const uint8_t* block, uint8_t* output, uint32_t outputStride) {
        // Extract color endpoints
        uint16_t color0 = block[0] | (block[1] << 8);
        uint16_t color1 = block[2] | (block[3] << 8);

        // Convert endpoints to RGB
        uint32_t colors[4];
        colors[0] = ConvertR5G6B5ToR8G8B8(color0);
        colors[1] = ConvertR5G6B5ToR8G8B8(color1);

        // Calculate intermediate colors
        if (color0 > color1) {
            colors[2] = ((2 * colors[0]) + colors[1]) / 3;
            colors[3] = ((2 * colors[1]) + colors[0]) / 3;
        } else {
            colors[2] = (colors[0] + colors[1]) / 2;
            colors[3] = 0x00000000; // Transparent black
        }

        // Decode each pixel in the 4x4 block
        for (int y = 0; y < 4; y++) {
            uint8_t rowBits = block[4 + y];
            for (int x = 0; x < 4; x++) {
                uint8_t idx = (rowBits >> (2 * x)) & 0x03;
                uint8_t* pixel = output + y * outputStride + x * 4;

                *reinterpret_cast<uint32_t*>(pixel) = colors[idx];
                if (idx == 3 && color0 <= color1) {
                    pixel[3] = 0; // Fully transparent
                } else {
                    pixel[3] = 255; // Fully opaque
                }
            }
        }
    }

    void DSTexture::DecompressDXT5Block(const uint8_t* block, uint8_t* output, uint32_t outputStride) {
        // Decompress alpha channel (first 8 bytes)
        uint8_t alpha0 = block[0];
        uint8_t alpha1 = block[1];
        const uint8_t* alphaBits = block + 2;
        uint8_t alphaValues[8];

        alphaValues[0] = alpha0;
        alphaValues[1] = alpha1;

        if (alpha0 > alpha1) {
            alphaValues[2] = (6 * alpha0 + 1 * alpha1) / 7;
            alphaValues[3] = (5 * alpha0 + 2 * alpha1) / 7;
            alphaValues[4] = (4 * alpha0 + 3 * alpha1) / 7;
            alphaValues[5] = (3 * alpha0 + 4 * alpha1) / 7;
            alphaValues[6] = (2 * alpha0 + 5 * alpha1) / 7;
            alphaValues[7] = (1 * alpha0 + 6 * alpha1) / 7;
        } else {
            alphaValues[2] = (4 * alpha0 + 1 * alpha1) / 5;
            alphaValues[3] = (3 * alpha0 + 2 * alpha1) / 5;
            alphaValues[4] = (2 * alpha0 + 3 * alpha1) / 5;
            alphaValues[5] = (1 * alpha0 + 4 * alpha1) / 5;
            alphaValues[6] = 0x00;
            alphaValues[7] = 0xFF;
        }

        // Decompress color channel (last 8 bytes, same as DXT1)
        uint8_t colorBlock[8];
        memcpy(colorBlock, block + 8, 8);

        // Temporary storage for color
        uint8_t tempColor[4*4*4];
        DecompressDXT1Block(colorBlock, tempColor, 16);

        // Combine alpha and color channels
        for (int y = 0; y < 4; y++) {
            // Get alpha bits for this row
            uint32_t alphaRow;
            if (y < 2) {
                alphaRow = alphaBits[0] | (alphaBits[1] << 8) | (alphaBits[2] << 16);
                if (y == 1) alphaBits += 3;
            } else {
                alphaRow = alphaBits[0] | (alphaBits[1] << 8) | (alphaBits[2] << 16);
                alphaBits += 3;
            }

            for (int x = 0; x < 4; x++) {
                uint8_t alphaIdx = (alphaRow >> (3 * x)) & 0x07;
                uint8_t alpha = alphaValues[alphaIdx];

                uint8_t* srcPixel = tempColor + (y * 4 + x) * 4;
                uint8_t* dstPixel = output + y * outputStride + x * 4;

                dstPixel[0] = srcPixel[0];
                dstPixel[1] = srcPixel[1];
                dstPixel[2] = srcPixel[2];
                dstPixel[3] = alpha;
            }
        }
    }

    bool DSTexture::Decompress() {

        if (!IsFormatCompressed(m_format) || m_mipmaps.empty()) {
            return false;
        }

        // Determine target format based on compression type
        Format targetFormat = (m_format == Format::DXT1) ? Format::RGB8 : Format::RGBA8;

        // Perform the decompression
        if (!DecompressDXT()) {
            return false;
        }

        m_format = targetFormat;
        return true;
    }

    bool DSTexture::DecompressDXT() {
        std::vector<MipLevel> decompressedMips;
        decompressedMips.reserve(m_mipmaps.size());

        for (const auto& mip : m_mipmaps) {
            MipLevel newMip;
            newMip.width = mip.width;
            newMip.height = mip.height;

            // Calculate decompressed size
            size_t pixelSize = (m_format == Format::DXT1) ? 3 : 4;
            size_t decompressedSize = mip.width * mip.height * pixelSize;
            newMip.data.resize(decompressedSize);

            uint32_t blocksWide = (mip.width + 3) / 4;
            uint32_t blocksHigh = (mip.height + 3) / 4;
            uint32_t blockSize = (m_format == Format::DXT1) ? 8 : 16;

            for (uint32_t by = 0; by < blocksHigh; by++) {
                for (uint32_t bx = 0; bx < blocksWide; bx++) {
                    const uint8_t* block = mip.data.data() + (by * blocksWide + bx) * blockSize;
                    uint8_t* output = newMip.data.data() + (by * 4 * mip.width + bx * 4) * pixelSize;

                    if (m_format == Format::DXT1) {
                        DecompressDXT1Block(block, output, mip.width * pixelSize);
                    } else {
                        DecompressDXT5Block(block, output, mip.width * pixelSize);
                    }
                }
            }

            decompressedMips.push_back(std::move(newMip));
        }

        m_mipmaps = std::move(decompressedMips);
        return true;
    }

    uint32_t DSTexture::ConvertR5G6B5ToR8G8B8(uint16_t color) {
        uint8_t r = (color >> 11) & 0x1F;
        uint8_t g = (color >> 5) & 0x3F;
        uint8_t b = color & 0x1F;

        // Scale up to 8 bits
        r = (r << 3) | (r >> 2);
        g = (g << 2) | (g >> 4);
        b = (b << 3) | (b >> 2);

        return (r << 16) | (g << 8) | b;
    }

}