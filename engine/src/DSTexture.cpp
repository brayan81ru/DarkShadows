#include "DSTexture.h"
#include <fstream>
#include <algorithm>
#include <cstring>

// STB implementations
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "../third_party/stb/stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../third_party/stb/stb_image_resize.h"
namespace DSEngine {
    // =====================
    // Construction/Destruction
    // =====================

    DSTexture::DSTexture() : m_format(Format::UNKNOWN), m_flags(0) {}

    DSTexture::~DSTexture() = default;

    // =====================
    // Static Creation Methods
    // =====================

    std::unique_ptr<DSTexture> DSTexture::CreateFromFile(const std::string& path, bool flipVertically) {
        auto texture = std::make_unique<DSTexture>();
        if (!texture->LoadFromFile(path)) {
            return nullptr;
        }
        return texture;
    }

    std::unique_ptr<DSTexture> DSTexture::CreateEmpty(uint32_t width, uint32_t height, Format format) {
        auto texture = std::make_unique<DSTexture>();
        texture->m_format = format;

        MipLevel baseLevel;
        baseLevel.width = width;
        baseLevel.height = height;
        baseLevel.data.resize(width * height * GetPixelSize(format));

        texture->m_mipmaps.push_back(std::move(baseLevel));
        return texture;
    }

    std::unique_ptr<DSTexture> DSTexture::CreateFromMemory(const uint8_t* data, uint32_t width, uint32_t height, Format format) {
        auto texture = std::make_unique<DSTexture>();
        texture->m_format = format;

        MipLevel baseLevel;
        baseLevel.width = width;
        baseLevel.height = height;
        const size_t dataSize = width * height * GetPixelSize(format);
        baseLevel.data.assign(data, data + dataSize);

        texture->m_mipmaps.push_back(std::move(baseLevel));
        return texture;
    }

    // =====================
    // File Operations
    // =====================

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
        header.flags = m_flags;

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

        return file.good();
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

            return file.good();
        }

        // Fall back to STB loader
        return LoadFromSTB(path, true);
    }

    bool DSTexture::LoadFromSTB(const std::string& path, bool flipVertically) {
        stbi_set_flip_vertically_on_load(flipVertically);

        int width, height, channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        if (!data) return false;

        // Determine format
        switch (channels) {
            case 1: m_format = Format::RGB8; break; // Convert to RGB8 for consistency
            case 2: m_format = Format::RGB8; break; // Convert to RGB8
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

        // Convert to target format if needed
        if ((channels == 3 && m_format == Format::RGB8) ||
            (channels == 4 && m_format == Format::RGBA8)) {
            // Direct copy
            const size_t dataSize = width * height * channels;
            baseLevel.data.assign(data, data + dataSize);
            } else {
                // Convert channels
                const size_t targetPixelSize = GetPixelSize(m_format);
                baseLevel.data.resize(width * height * targetPixelSize);

                if (channels == 1 && m_format == Format::RGB8) {
                    // Grayscale to RGB
                    for (int i = 0; i < width * height; i++) {
                        baseLevel.data[i*3] = data[i];
                        baseLevel.data[i*3+1] = data[i];
                        baseLevel.data[i*3+2] = data[i];
                    }
                } else if (channels == 2 && m_format == Format::RGB8) {
                    // GA to RGB (drop alpha)
                    for (int i = 0; i < width * height; i++) {
                        baseLevel.data[i*3] = data[i*2];
                        baseLevel.data[i*3+1] = data[i*2];
                        baseLevel.data[i*3+2] = data[i*2];
                    }
                }
            }

        m_mipmaps.push_back(std::move(baseLevel));
        stbi_image_free(data);
        return true;
    }


    // =============================================
    // Private Helper Methods Implementation
    // =============================================

    bool DSTexture::ValidateMipLevel(uint32_t level) const {
        return level < m_mipmaps.size();
    }

    bool DSTexture::ConvertFormat(Format newFormat) {
        if (m_mipmaps.empty()) return false;

        // No conversion needed if formats match
        if (m_format == newFormat) return true;

        // Handle compressed-to-compressed conversion
        if (IsFormatCompressed(m_format) && IsFormatCompressed(newFormat)) {
            // Decompress first, then compress to new format
            if (!Decompress()) return false;
            return Compress(newFormat, CompressionQuality::NORMAL);
        }

        // Handle decompression
        if (IsFormatCompressed(m_format)) {
            return Decompress() && ConvertFormat(newFormat);
        }

        // Handle compression
        if (IsFormatCompressed(newFormat)) {
            return Compress(newFormat, CompressionQuality::NORMAL);
        }

        // Handle uncompressed-to-uncompressed conversion
        std::vector<MipLevel> convertedMips;
        convertedMips.reserve(m_mipmaps.size());

        for (const auto& mip : m_mipmaps) {
            MipLevel newMip;
            newMip.width = mip.width;
            newMip.height = mip.height;

            // Calculate new data size
            size_t newPixelSize = GetPixelSize(newFormat);
            size_t newDataSize = mip.width * mip.height * newPixelSize;
            newMip.data.resize(newDataSize);

            // Perform conversion
            bool success = false;
            switch (m_format) {
                case Format::RGB8:
                    success = ConvertFromRGB8(mip, newMip, newFormat);
                    break;
                case Format::RGBA8:
                    success = ConvertFromRGBA8(mip, newMip, newFormat);
                    break;
                default:
                    return false;
            }

            if (!success) return false;
            convertedMips.push_back(std::move(newMip));
        }

        m_mipmaps = std::move(convertedMips);
        m_format = newFormat;
        return true;
    }

    bool DSTexture::ConvertFromRGB8(const MipLevel& source, MipLevel& dest, Format newFormat) {
        const uint8_t* src = source.data.data();
        uint8_t* dst = dest.data.data();
        const size_t srcPixelSize = 3; // RGB8
        const size_t dstPixelSize = GetPixelSize(newFormat);
        const size_t pixelCount = source.width * source.height;

        switch (newFormat) {
            case Format::RGBA8: {
                // RGB8 -> RGBA8 (add alpha channel)
                for (size_t i = 0; i < pixelCount; i++) {
                    dst[0] = src[0]; // R
                    dst[1] = src[1]; // G
                    dst[2] = src[2]; // B
                    dst[3] = 255;    // A (fully opaque)
                    src += srcPixelSize;
                    dst += dstPixelSize;
                }
                return true;
            }
            default:
                return false;
        }
    }

    bool DSTexture::ConvertFromRGBA8(const MipLevel& source, MipLevel& dest, Format newFormat) {
        const uint8_t* src = source.data.data();
        uint8_t* dst = dest.data.data();
        const size_t srcPixelSize = 4; // RGBA8
        const size_t dstPixelSize = GetPixelSize(newFormat);
        const size_t pixelCount = source.width * source.height;

        switch (newFormat) {
            case Format::RGB8: {
                // RGBA8 -> RGB8 (drop alpha channel)
                for (size_t i = 0; i < pixelCount; i++) {
                    dst[0] = src[0]; // R
                    dst[1] = src[1]; // G
                    dst[2] = src[2]; // B
                    src += srcPixelSize;
                    dst += dstPixelSize;
                }
                return true;
            }
            default:
                return false;
        }
    }

    // =============================================
    // Compression/Decompression Implementation
    // =============================================

    // Compression implementation using stb_dxt
    bool DSTexture::Compress(Format dxtFormat, CompressionQuality quality) {
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
            if (!ConvertFormat(Format::RGBA8)) {
                return false;
            }
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

    bool DSTexture::Decompress() {
        if (!IsCompressed() || m_mipmaps.empty()) {
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

    void DSTexture::DecompressDXT1Block(const uint8_t* block, uint8_t* output, uint32_t outputStride) {
        // Extract color endpoints (stored as R5G6B5)
        uint16_t color0 = block[0] | (block[1] << 8);
        uint16_t color1 = block[2] | (block[3] << 8);

        // Convert to R8G8B8
        uint32_t colors[4];
        colors[0] = ConvertR5G6B5ToR8G8B8(color0);
        colors[1] = ConvertR5G6B5ToR8G8B8(color1);

        // Calculate intermediate colors
        if (color0 > color1) {
            colors[2] = ((2 * colors[0]) + colors[1]) / 3;
            colors[3] = ((2 * colors[1]) + colors[0]) / 3;
        } else {
            colors[2] = (colors[0] + colors[1]) / 2;
            colors[3] = 0; // Transparent black
        }

        // Decode 4x4 block
        for (int y = 0; y < 4; y++) {
            uint8_t rowBits = block[4 + y];
            for (int x = 0; x < 4; x++) {
                uint8_t idx = (rowBits >> (2 * x)) & 0x03;
                uint8_t* pixel = output + y * outputStride + x * 3;

                pixel[0] = (colors[idx] >> 16) & 0xFF; // R
                pixel[1] = (colors[idx] >> 8) & 0xFF;  // G
                pixel[2] = colors[idx] & 0xFF;         // B
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
        uint8_t tempColor[4*4*3];
        DecompressDXT1Block(colorBlock, tempColor, 12);

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

                uint8_t* srcPixel = tempColor + (y * 4 + x) * 3;
                uint8_t* dstPixel = output + y * outputStride + x * 4;

                dstPixel[0] = srcPixel[0]; // R
                dstPixel[1] = srcPixel[1]; // G
                dstPixel[2] = srcPixel[2]; // B
                dstPixel[3] = alpha;       // A
            }
        }
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

    // =============================================
    // Mipmap Operations
    // =============================================

    bool DSTexture::GenerateMipmaps(CompressionQuality quality) {
        if (m_mipmaps.empty()) return false;

        // If compressed, we need to decompress first
        bool wasCompressed = IsCompressed();
        Format originalFormat = m_format;

        if (wasCompressed && !Decompress()) {
            return false;
        }

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

        // Recompress if we were compressed before
        if (wasCompressed) {
            return Compress(originalFormat, quality);
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

    // =============================================
    // Accessors and Utility Methods
    // =============================================

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
            case Format::DXT1: return 3; // Decompresses to RGB
            case Format::DXT5: return 4; // Decompresses to RGBA
            default: return 0;
        }
    }

    size_t DSTexture::GetPixelSize(Format format) {
        switch (format) {
            case Format::RGB8: return 3;
            case Format::RGBA8: return 4;
            case Format::DXT1: return 3; // Decompressed size
            case Format::DXT5: return 4; // Decompressed size
            default: return 0;
        }
    }

    size_t DSTexture::GetBlockSize(Format format) {
        switch (format) {
            case Format::DXT1: return 8;  // 8 bytes per 4x4 block
            case Format::DXT5: return 16; // 16 bytes per 4x4 block
            default: return GetPixelSize(format); // Uncompressed
        }
    }

    bool DSTexture::IsFormatCompressed(Format format) {
        return format == Format::DXT1 || format == Format::DXT5;
    }

    uint32_t DSTexture::CalculateMipSize(uint32_t width, uint32_t height, Format format) {
        if (IsFormatCompressed(format)) {
            // For compressed formats, size is based on 4x4 blocks
            uint32_t blockWidth = (width + 3) / 4;
            uint32_t blockHeight = (height + 3) / 4;
            return blockWidth * blockHeight * GetBlockSize(format);
        } else {
            // For uncompressed formats
            return width * height * GetPixelSize(format);
        }
    }

    bool DSTexture::IsCompressed() const {
        return IsFormatCompressed(m_format);
    }
}