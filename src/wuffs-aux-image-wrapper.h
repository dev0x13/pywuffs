#pragma once

#include <pybind11/numpy.h>

#include <c/wuffs-v0.3.c>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

// This API wraps the wuffs_aux API for image decoding. The wrapper is needed
// since the wuffs_aux API uses the callback-based approach which doesn't play
// well with Python-C++ interop.
// Although the API is different from wuffs_aux, it's supposed to be as thin as
// possible including minor aspect like the default non-configured behavior.

namespace wuffs_aux_wrap {

enum class ImageDecoderFlags : uint64_t {
#define IDFE(flag)         \
  REPORT_METADATA_##flag = \
      wuffs_aux::DecodeImageArgFlags::REPORT_METADATA_##flag
  IDFE(BGCL),
  IDFE(CHRM),
  IDFE(EXIF),
  IDFE(GAMA),
  IDFE(ICCP),
  IDFE(KVP),
  IDFE(MTIM),
  IDFE(OFS2),
  IDFE(PHYD),
  IDFE(SRGB),
  IDFE(XMP)
#undef IDF_ENTRY
};

enum class ImageDecoderQuirks : uint32_t {
  IGNORE_CHECKSUM = WUFFS_BASE__QUIRK_IGNORE_CHECKSUM,
  GIF_DELAY_NUM_DECODED_FRAMES = WUFFS_GIF__QUIRK_DELAY_NUM_DECODED_FRAMES,
  GIF_FIRST_FRAME_LOCAL_PALETTE_MEANS_BLACK_BACKGROUND =
      WUFFS_GIF__QUIRK_FIRST_FRAME_LOCAL_PALETTE_MEANS_BLACK_BACKGROUND,
  GIF_QUIRK_HONOR_BACKGROUND_COLOR = WUFFS_GIF__QUIRK_HONOR_BACKGROUND_COLOR,
  GIF_IGNORE_TOO_MUCH_PIXEL_DATA = WUFFS_GIF__QUIRK_IGNORE_TOO_MUCH_PIXEL_DATA,
  GIF_IMAGE_BOUNDS_ARE_STRICT = WUFFS_GIF__QUIRK_IMAGE_BOUNDS_ARE_STRICT,
  GIF_REJECT_EMPTY_FRAME = WUFFS_GIF__QUIRK_REJECT_EMPTY_FRAME,
  GIF_REJECT_EMPTY_PALETTE = WUFFS_GIF__QUIRK_REJECT_EMPTY_PALETTE
};

enum class ImageDecoderType : uint32_t {
#define IDTE(dt) dt = WUFFS_BASE__FOURCC__##dt
  IDTE(BMP),
  IDTE(GIF),
  IDTE(NIE),
  IDTE(PNG),
  IDTE(TGA),
  IDTE(WBMP),
  IDTE(JPEG)
#undef IDTE
};

enum class PixelFormat : uint32_t {
#define PFE(pf) pf = WUFFS_BASE__PIXEL_FORMAT__##pf
  PFE(A),
  PFE(Y),
  PFE(Y_16LE),
  PFE(Y_16BE),
  PFE(YA_NONPREMUL),
  PFE(YA_PREMUL),
  PFE(YCBCR),
  PFE(YCBCRA_NONPREMUL),
  PFE(YCBCRK),
  PFE(YCOCG),
  PFE(YCOCGA_NONPREMUL),
  PFE(YCOCGK),
  PFE(INDEXED__BGRA_NONPREMUL),
  PFE(INDEXED__BGRA_PREMUL),
  PFE(INDEXED__BGRA_BINARY),
  PFE(BGR_565),
  PFE(BGR),
  PFE(BGRA_NONPREMUL),
  PFE(BGRA_NONPREMUL_4X16LE),
  PFE(BGRA_PREMUL),
  PFE(BGRA_PREMUL_4X16LE),
  PFE(BGRA_BINARY),
  PFE(BGRX),
  PFE(RGB),
  PFE(RGBA_NONPREMUL),
  PFE(RGBA_NONPREMUL_4X16LE),
  PFE(RGBA_PREMUL),
  PFE(RGBA_PREMUL_4X16LE),
  PFE(RGBA_BINARY),
  PFE(RGBX),
  PFE(CMY),
  PFE(CMYK)
#undef PFE
};

enum class PixelBlend : uint32_t {
  SRC = WUFFS_BASE__PIXEL_BLEND__SRC,
  SRC_OVER = WUFFS_BASE__PIXEL_BLEND__SRC_OVER
};

enum class PixelSubsampling : uint32_t {
  NONE = WUFFS_BASE__PIXEL_SUBSAMPLING__NONE,
#define PSE(ps) K##ps = WUFFS_BASE__PIXEL_SUBSAMPLING__##ps
  PSE(444),
  PSE(440),
  PSE(422),
  PSE(420),
  PSE(411),
  PSE(410)
#undef PFE
};

// This struct hosts wuffs_aux::DecodeImage arguments in more user- and
// Python- friendly fashion
struct ImageDecoderConfig {
  std::vector<ImageDecoderFlags> flags;
  PixelBlend pixel_blend = static_cast<PixelBlend>(
      wuffs_aux::DecodeImageArgPixelBlend::DefaultValue().repr);
  std::vector<ImageDecoderQuirks> quirks;
  uint32_t background_color =
      wuffs_aux::DecodeImageArgBackgroundColor::DefaultValue().repr;
  uint32_t max_incl_dimension =
      wuffs_aux::DecodeImageArgMaxInclDimension::DefaultValue().repr;
  uint64_t max_incl_metadata_length =
      wuffs_aux::DecodeImageArgMaxInclMetadataLength::DefaultValue().repr;
  std::vector<ImageDecoderType> enabled_decoders = {
      ImageDecoderType::BMP, ImageDecoderType::GIF, ImageDecoderType::NIE,
      ImageDecoderType::PNG, ImageDecoderType::TGA, ImageDecoderType::WBMP};
  uint32_t pixel_format = wuffs_base__make_pixel_format(
                              static_cast<uint32_t>(PixelFormat::BGRA_PREMUL))
                              .repr;
};

// This structs represents the wuffs_aux::DecodeImageCallbacks::HandleMetadata
// input
struct MetadataEntry {
  wuffs_base__more_information minfo{};
  pybind11::array_t<uint8_t> data;

  MetadataEntry(const wuffs_base__more_information& minfo,
                pybind11::array_t<uint8_t>&& data)
      : minfo(minfo), data(std::move(data)) {}

  MetadataEntry() : minfo(wuffs_base__empty_more_information()) {}

  MetadataEntry(MetadataEntry&& other) noexcept {
    minfo = other.minfo;
    std::swap(data, other.data);
  }

  MetadataEntry& operator=(MetadataEntry&& other) noexcept {
    if (this != &other) {
      minfo = other.minfo;
      std::swap(data, other.data);
    }
    return *this;
  }

  MetadataEntry(const wuffs_aux_wrap::MetadataEntry& other) = delete;
  MetadataEntry& operator=(const wuffs_aux_wrap::MetadataEntry& other) = delete;
};

struct ImageDecodingResult {
  wuffs_base__pixel_config pixcfg = wuffs_base__null_pixel_config();
  pybind11::array_t<uint8_t> pixbuf;
  std::vector<MetadataEntry> reported_metadata;
  std::string error_message;

  ImageDecodingResult() = default;

  ImageDecodingResult(ImageDecodingResult&& other) noexcept {
    std::swap(pixcfg, other.pixcfg);
    std::swap(pixbuf, other.pixbuf);
    std::swap(reported_metadata, other.reported_metadata);
    std::swap(error_message, other.error_message);
  }

  ImageDecodingResult& operator=(ImageDecodingResult&& other) noexcept {
    if (this != &other) {
      std::swap(pixcfg, other.pixcfg);
      std::swap(pixbuf, other.pixbuf);
      std::swap(reported_metadata, other.reported_metadata);
      std::swap(error_message, other.error_message);
    }
    return *this;
  }

  ImageDecodingResult(ImageDecodingResult& other) = delete;
  ImageDecodingResult& operator=(ImageDecodingResult& other) = delete;
};

struct ImageDecoderError {
  static const std::string MaxInclDimensionExceeded;
  static const std::string MaxInclMetadataLengthExceeded;
  static const std::string OutOfMemory;
  static const std::string UnexpectedEndOfFile;
  static const std::string UnsupportedImageFormat;
  static const std::string UnsupportedMetadata;
  static const std::string UnsupportedPixelBlend;
  static const std::string UnsupportedPixelConfiguration;
  static const std::string UnsupportedPixelFormat;
  static const std::string FailedToOpenFile;
};

const std::string ImageDecoderError::MaxInclDimensionExceeded =
    wuffs_aux::DecodeImage_MaxInclDimensionExceeded;
const std::string ImageDecoderError::MaxInclMetadataLengthExceeded =
    wuffs_aux::DecodeImage_MaxInclMetadataLengthExceeded;
const std::string ImageDecoderError::OutOfMemory =
    wuffs_aux::DecodeImage_OutOfMemory;
const std::string ImageDecoderError::UnexpectedEndOfFile =
    wuffs_aux::DecodeImage_UnexpectedEndOfFile;
const std::string ImageDecoderError::UnsupportedImageFormat =
    wuffs_aux::DecodeImage_UnsupportedImageFormat;
const std::string ImageDecoderError::UnsupportedMetadata =
    wuffs_aux::DecodeImage_UnsupportedMetadata;
const std::string ImageDecoderError::UnsupportedPixelBlend =
    wuffs_aux::DecodeImage_UnsupportedPixelBlend;
const std::string ImageDecoderError::UnsupportedPixelConfiguration =
    wuffs_aux::DecodeImage_UnsupportedPixelConfiguration;
const std::string ImageDecoderError::UnsupportedPixelFormat =
    wuffs_aux::DecodeImage_UnsupportedPixelFormat;
const std::string ImageDecoderError::FailedToOpenFile =
    "wuffs_aux_wrap::ImageDecoder::Decode: failed to open file";

    class ImageDecoder : public wuffs_aux::DecodeImageCallbacks {
 public:
  explicit ImageDecoder(const ImageDecoderConfig& config)
      : quirks_vector_({config.quirks.begin(), config.quirks.end()}),
        enabled_decoders_(
            {config.enabled_decoders.begin(), config.enabled_decoders.end()}),
        pixel_format_(wuffs_base__make_pixel_format(config.pixel_format)),
        quirks_(wuffs_aux::DecodeImageArgQuirks(
            reinterpret_cast<uint32_t*>(quirks_vector_.data()),
            quirks_vector_.size())),
        flags_(GetFlagsBitmask(config.flags)),
        pixel_blend_(wuffs_aux::DecodeImageArgPixelBlend(
            static_cast<uint32_t>(config.pixel_blend))),
        background_color_(
            wuffs_aux::DecodeImageArgBackgroundColor(config.background_color)),
        max_incl_dimension_(wuffs_aux::DecodeImageArgMaxInclDimension(
            config.max_incl_dimension)),
        max_incl_metadata_length_(
            wuffs_aux::DecodeImageArgMaxInclMetadataLength(
                config.max_incl_metadata_length)) {}

  /* DecodeImageCallbacks methods implementation */

  wuffs_base__image_decoder::unique_ptr SelectDecoder(
      uint32_t fourcc, wuffs_base__slice_u8 prefix_data,
      bool prefix_closed) override {
    if (enabled_decoders_.count(static_cast<ImageDecoderType>(fourcc)) == 0) {
      return {nullptr, &free};
    }
    return wuffs_aux::DecodeImageCallbacks::SelectDecoder(fourcc, prefix_data,
                                                          prefix_closed);
  }

  std::string HandleMetadata(const wuffs_base__more_information& minfo,
                             wuffs_base__slice_u8 raw) override {
    decoding_result_.reported_metadata.emplace_back(
        minfo, pybind11::array(pybind11::dtype("uint8"), {raw.len}, raw.ptr));
    return "";
  }

  wuffs_base__pixel_format SelectPixfmt(
      const wuffs_base__image_config&) override {
    return pixel_format_;
  }

  // This implementation is essentially the same as the default one except that
  // it uses the "decoding_result_" field for allocating output buffer
  AllocPixbufResult AllocPixbuf(const wuffs_base__image_config& image_config,
                                bool allow_uninitialized_memory) override {
    uint32_t w = image_config.pixcfg.width();
    uint32_t h = image_config.pixcfg.height();
    if ((w == 0) || (h == 0)) {
      return {""};
    }
    uint64_t len = image_config.pixcfg.pixbuf_len();
    if (len == 0 || SIZE_MAX < len) {
      return {wuffs_aux::DecodeImage_UnsupportedPixelConfiguration};
    }
    decoding_result_.pixbuf.resize({len});
    if (!allow_uninitialized_memory) {
      std::memset(decoding_result_.pixbuf.mutable_data(), 0,
                  decoding_result_.pixbuf.size());
    }
    wuffs_base__pixel_buffer pixbuf;
    wuffs_base__status status = pixbuf.set_from_slice(
        &image_config.pixcfg,
        wuffs_base__make_slice_u8(decoding_result_.pixbuf.mutable_data(),
                                  decoding_result_.pixbuf.size()));
    if (!status.is_ok()) {
      decoding_result_.pixbuf = {};
      return {status.message()};
    }
    return {wuffs_aux::MemOwner(nullptr, &free), pixbuf};
  }

  /* End of DecodeImageCallbacks methods implementation */

  ImageDecodingResult Decode(const uint8_t* data, size_t size) {
    wuffs_aux::sync_io::MemoryInput input(data, size);
    return DecodeInternal(input);
  }

  ImageDecodingResult Decode(const std::string& path_to_file) {
    FILE* f = fopen(path_to_file.c_str(), "rb");
    if (!f) {
      ImageDecodingResult result;
      result.error_message = ImageDecoderError::FailedToOpenFile;
      return result;
    }
    wuffs_aux::sync_io::FileInput input(f);
    ImageDecodingResult result = DecodeInternal(input);
    fclose(f);
    return result;
  }

 private:
  static uint64_t GetFlagsBitmask(const std::vector<ImageDecoderFlags>& flags) {
    uint64_t bitmask = 0;
    for (const auto f : flags) {
      bitmask |= static_cast<uint64_t>(f);
    }
    return bitmask;
  }

  ImageDecodingResult DecodeInternal(wuffs_aux::sync_io::Input& input) {
    wuffs_aux::DecodeImageResult decode_image_result = wuffs_aux::DecodeImage(
        *this, input, quirks_, flags_, pixel_blend_, background_color_,
        max_incl_dimension_, max_incl_metadata_length_);
    decoding_result_.error_message =
        std::move(decode_image_result.error_message);
    if (!decode_image_result.pixbuf.pixcfg.is_valid()) {
      decoding_result_.pixbuf = {};
      decoding_result_.pixcfg = wuffs_base__null_pixel_config();
    } else {
      decoding_result_.pixcfg = decode_image_result.pixbuf.pixcfg;
      decoding_result_.pixbuf =
          decoding_result_.pixbuf.reshape(std::vector<size_t>{
              decoding_result_.pixcfg.height(), decoding_result_.pixcfg.width(),
              decoding_result_.pixcfg.pixbuf_len() /
                  (decoding_result_.pixcfg.width() *
                   decoding_result_.pixcfg.height())});
    }
    return std::move(decoding_result_);
  }

 private:
  ImageDecodingResult decoding_result_;
  std::vector<ImageDecoderQuirks> quirks_vector_;
  std::unordered_set<ImageDecoderType> enabled_decoders_;
  wuffs_base__pixel_format pixel_format_;
  wuffs_aux::DecodeImageArgQuirks quirks_;
  wuffs_aux::DecodeImageArgFlags flags_;
  wuffs_aux::DecodeImageArgPixelBlend pixel_blend_;
  wuffs_aux::DecodeImageArgBackgroundColor background_color_;
  wuffs_aux::DecodeImageArgMaxInclDimension max_incl_dimension_;
  wuffs_aux::DecodeImageArgMaxInclMetadataLength max_incl_metadata_length_;
};

}  // namespace wuffs_aux_wrap
