#define WUFFS_IMPLEMENTATION

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <c/wuffs-v0.3.c>

#include "wuffs-aux-image-wrapper.h"

namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(std::vector<uint8_t>);

PYBIND11_MODULE(pywuffs, m) {
  m.doc() = "Python bindings for Wuffs the Library.";

  // This prevents input/output buffers from copying
  py::bind_vector<std::vector<uint8_t>>(
      m, "VectorUint8", py::buffer_protocol(),
      "Auxiliary class for zero-copy data transfers between Python and C++. "
      "Exposes append, clear, extend, insert, pop, and some iterator methods. "
      "Implements buffer protocol, so it can be used with memoryview.");
  py::bind_vector<std::vector<wuffs_aux_wrap::MetadataEntry>>(
      m, "VectorMetadataEntry",
      "Auxiliary list-like class holding instances of MetadataEntry for "
      "zero-copy data transfers between Python and C++. "
      "Exposes append, clear, extend, insert, pop, and some iterator "
      "methods.");

  py::module aux_m = m.def_submodule("aux", "Simplified \"auxiliary\" API.");

  py::class_<wuffs_base__more_information>(
      m, "wuffs_base__more_information",
      "Holds additional fields. The flavor field follows the base38 namespace "
      "convention. The other fields' semantics depends on the flavor.")
      .def_readonly("flavor", &wuffs_base__more_information::flavor)
      .def_readonly("w", &wuffs_base__more_information::w)
      .def_readonly("x", &wuffs_base__more_information::x)
      .def_readonly("y", &wuffs_base__more_information::y)
      .def_readonly("z", &wuffs_base__more_information::z)
      .def("io_redirect__fourcc",
           &wuffs_base__more_information::io_redirect__fourcc)
      .def("io_seek__position",
           &wuffs_base__more_information::io_seek__position)
      .def("metadata__fourcc", &wuffs_base__more_information::metadata__fourcc)
      .def("metadata_parsed__chrm",
           &wuffs_base__more_information::metadata_parsed__chrm)
      .def("metadata_parsed__gama",
           &wuffs_base__more_information::metadata_parsed__gama)
      .def("metadata_parsed__srgb",
           &wuffs_base__more_information::metadata_parsed__srgb);

  py::class_<wuffs_aux_wrap::MetadataEntry>(aux_m, "MetadataEntry",
                                            "Holds parsed metadata piece.")
      .def_readonly("minfo", &wuffs_aux_wrap::MetadataEntry::minfo,
                    "wuffs_base__more_information: Info on parsed metadata.")
      .def_readonly("data", &wuffs_aux_wrap::MetadataEntry::data,
                    "VectorUint8: Parsed metadata.");

  py::enum_<wuffs_aux_wrap::ImageDecoderFlags>(
      aux_m, "ImageDecoderFlags",
      "Flags to defining image decoder behavior (e.g. metadata reporting).")
      .value("REPORT_METADATA_BGCL",
             wuffs_aux_wrap::ImageDecoderFlags::REPORT_METADATA_BGCL,
             "Background Color.")
      .value("REPORT_METADATA_CHRM",
             wuffs_aux_wrap::ImageDecoderFlags::REPORT_METADATA_CHRM,
             "Primary Chromaticities and White Point.")
      .value("REPORT_METADATA_EXIF",
             wuffs_aux_wrap::ImageDecoderFlags::REPORT_METADATA_EXIF,
             "Exchangeable Image File Format.")
      .value("REPORT_METADATA_GAMA",
             wuffs_aux_wrap::ImageDecoderFlags::REPORT_METADATA_GAMA,
             "Gamma Correction.")
      .value("REPORT_METADATA_ICCP",
             wuffs_aux_wrap::ImageDecoderFlags::REPORT_METADATA_ICCP,
             "International Color Consortium Profile.")
      .value("REPORT_METADATA_KVP",
             wuffs_aux_wrap::ImageDecoderFlags::REPORT_METADATA_KVP,
             "Key-Value Pair. For PNG files, this includes iTXt, tEXt and zTXt "
             "chunks. In the HandleMetadata callback, the raw argument "
             "contains UTF-8 strings.")
      .value("REPORT_METADATA_MTIM",
             wuffs_aux_wrap::ImageDecoderFlags::REPORT_METADATA_MTIM,
             "Modification Time.")
      .value("REPORT_METADATA_OFS2",
             wuffs_aux_wrap::ImageDecoderFlags::REPORT_METADATA_OFS2,
             "Offset (2-Dimensional).")
      .value("REPORT_METADATA_PHYD",
             wuffs_aux_wrap::ImageDecoderFlags::REPORT_METADATA_PHYD,
             "Physical Dimensions.")
      .value("REPORT_METADATA_SRGB",
             wuffs_aux_wrap::ImageDecoderFlags::REPORT_METADATA_SRGB,
             "Standard Red Green Blue (Rendering Intent).")
      .value("REPORT_METADATA_XMP",
             wuffs_aux_wrap::ImageDecoderFlags::REPORT_METADATA_XMP,
             "Extensible Metadata Platform.");

  py::enum_<wuffs_aux_wrap::ImageDecoderQuirks>(
      aux_m, "ImageDecoderQuirks",
      "See https://github.com/google/wuffs/blob/main/doc/note/quirks.md.")
      .value("IGNORE_CHECKSUM",
             wuffs_aux_wrap::ImageDecoderQuirks::IGNORE_CHECKSUM,
             "Favor faster decodes over rejecting invalid checksums.")
      .value("GIF_DELAY_NUM_DECODED_FRAMES",
             wuffs_aux_wrap::ImageDecoderQuirks::GIF_DELAY_NUM_DECODED_FRAMES)
      .value("GIF_FIRST_FRAME_LOCAL_PALETTE_MEANS_BLACK_BACKGROUND",
             wuffs_aux_wrap::ImageDecoderQuirks::
                 GIF_FIRST_FRAME_LOCAL_PALETTE_MEANS_BLACK_BACKGROUND)
      .value(
          "GIF_QUIRK_HONOR_BACKGROUND_COLOR",
          wuffs_aux_wrap::ImageDecoderQuirks::GIF_QUIRK_HONOR_BACKGROUND_COLOR)
      .value("GIF_IGNORE_TOO_MUCH_PIXEL_DATA",
             wuffs_aux_wrap::ImageDecoderQuirks::GIF_IGNORE_TOO_MUCH_PIXEL_DATA)
      .value("GIF_IMAGE_BOUNDS_ARE_STRICT",
             wuffs_aux_wrap::ImageDecoderQuirks::GIF_IMAGE_BOUNDS_ARE_STRICT)
      .value("GIF_REJECT_EMPTY_FRAME",
             wuffs_aux_wrap::ImageDecoderQuirks::GIF_REJECT_EMPTY_FRAME)
      .value("GIF_REJECT_EMPTY_PALETTE",
             wuffs_aux_wrap::ImageDecoderQuirks::GIF_REJECT_EMPTY_PALETTE);

  py::enum_<wuffs_aux_wrap::ImageDecoderType>(aux_m, "ImageDecoderType")
      .value("BMP", wuffs_aux_wrap::ImageDecoderType::BMP)
      .value("GIF", wuffs_aux_wrap::ImageDecoderType::GIF)
      .value("NIE", wuffs_aux_wrap::ImageDecoderType::NIE)
      .value("PNG", wuffs_aux_wrap::ImageDecoderType::PNG)
      .value("TGA", wuffs_aux_wrap::ImageDecoderType::TGA)
      .value("WBMP", wuffs_aux_wrap::ImageDecoderType::WBMP)
      .value("JPEG", wuffs_aux_wrap::ImageDecoderType::JPEG);

  // clang-format off
#define PYPF(pf) .value(#pf, wuffs_aux_wrap::PixelFormat::pf)
  py::enum_<wuffs_aux_wrap::PixelFormat>(
      aux_m, "PixelFormat", "Common 8-bit-depth pixel formats. This list is not "
      "exhaustive; not all valid wuffs_base__pixel_format values are present.")
      PYPF(A)
      PYPF(Y)
      PYPF(Y_16LE)
      PYPF(Y_16BE)
      PYPF(YA_NONPREMUL)
      PYPF(YA_PREMUL)
      PYPF(YCBCR)
      PYPF(YCBCRA_NONPREMUL)
      PYPF(YCBCRK)
      PYPF(YCOCG)
      PYPF(YCOCGA_NONPREMUL)
      PYPF(YCOCGK)
      PYPF(INDEXED__BGRA_NONPREMUL)
      PYPF(INDEXED__BGRA_PREMUL)
      PYPF(INDEXED__BGRA_BINARY)
      PYPF(BGR_565)
      PYPF(BGR)
      PYPF(BGRA_NONPREMUL)
      PYPF(BGRA_NONPREMUL_4X16LE)
      PYPF(BGRA_PREMUL)
      PYPF(BGRA_PREMUL_4X16LE)
      PYPF(BGRA_BINARY)
      PYPF(BGRX)
      PYPF(RGB)
      PYPF(RGBA_NONPREMUL)
      PYPF(RGBA_NONPREMUL_4X16LE)
      PYPF(RGBA_PREMUL)
      PYPF(RGBA_PREMUL_4X16LE)
      PYPF(RGBA_BINARY)
      PYPF(RGBX)
      PYPF(CMY)
      PYPF(CMYK);
#undef PYPF
  // clang-format on

  py::enum_<wuffs_aux_wrap::PixelBlend>(
      aux_m, "PixelBlend",
      "Encodes how to blend source and destination pixels, accounting for "
      "transparency. It encompasses the Porter-Duff compositing operators as "
      "well as the other blending modes defined by PDF.")
      .value("SRC", wuffs_aux_wrap::PixelBlend::SRC)
      .value("SRC_OVER", wuffs_aux_wrap::PixelBlend::SRC_OVER);

  py::class_<wuffs_aux_wrap::ImageDecoderConfig>(aux_m, "ImageDecoderConfig",
                                                 "Image decoder configuration.")
      .def(py::init<>())
      .def_readwrite("flags", &wuffs_aux_wrap::ImageDecoderConfig::flags,
                     "list: list of ImageDecoderFlags, empty by default.")
      .def_readwrite("pixel_blend",
                     &wuffs_aux_wrap::ImageDecoderConfig::pixel_blend,
                     "PixelBlend: pixel blend mode, default is PixelBlend.SRC.")
      .def_readwrite(
          "quirks", &wuffs_aux_wrap::ImageDecoderConfig::quirks,
          "list: list of ImageDecoderQuirks, empty by default (PNG "
          "decoder will always have ImageDecoderQuirks.IGNORE_CHECKSUM quirk "
          "enabled implicitly).")
      .def_readwrite(
          "background_color",
          &wuffs_aux_wrap::ImageDecoderConfig::background_color,
          "int: The background_color is used to fill the pixel buffer after "
          "callbacks.AllocPixbuf returns, if it is valid in the "
          "wuffs_base__color_u32_argb_premul__is_valid sense. The default "
          "value, 0x0000_0001, is not valid since its Blue channel value "
          "(0x01) is greater than its Alpha channel value (0x00). A valid "
          "background_color will typically be overwritten when pixel_blend is "
          "PixelBlend.SRC, but might still be visible on partial "
          "(not total) success or when pixel_blend is PixelBlend.SRC_OVER and "
          "the decoded image is not fully opaque.")
      .def_readwrite(
          "max_incl_dimension",
          &wuffs_aux_wrap::ImageDecoderConfig::max_incl_dimension,
          "int: Decoding fails (with DecodeImage_MaxInclDimensionExceeded) if "
          "the image's width or height is greater than max_incl_dimension.")
      .def_readwrite(
          "max_incl_metadata_length",
          &wuffs_aux_wrap::ImageDecoderConfig::max_incl_metadata_length,
          "int: Decoding fails (with DecodeImage_MaxInclDimensionExceeded) if "
          "any opted-in (via flags bits) metadata is longer than "
          "max_incl_metadata_length.")
      .def_readwrite("enabled_decoders",
                     &wuffs_aux_wrap::ImageDecoderConfig::enabled_decoders,
                     "list: list of ImageDecoderType.")
      .def_readwrite(
          "output_pixel_format",
          &wuffs_aux_wrap::ImageDecoderConfig::pixel_format,
          "PixelFormat: Destination pixel format, default is "
          "PixelFormat.BGRA_PREMUL which is 4 bytes per pixel (8 "
          "bits per channel × 4 channels). Currently supported formats are:"
          "- PixelFormat.BGR_565\n"
          "- PixelFormat.BGR\n"
          "- PixelFormat.BGRA_NONPREMUL\n"
          "- PixelFormat.BGRA_NONPREMUL_4X16LE\n"
          "- PixelFormat.BGRA_PREMUL\n"
          "- PixelFormat.RGBA_NONPREMUL\n"
          "- PixelFormat.RGBA_PREMUL");

  py::class_<wuffs_aux_wrap::ImageDecodingResult>(
      aux_m, "ImageDecodingResult",
      "Image decoding result. The fields depend on whether decoding "
      "succeeded:\n"
      " - On total success, the error_message is empty and decoded_data is not "
      "empty.\n"
      " - On partial success (e.g. the input file was truncated but we are "
      "still able to decode some of the pixels), error_message is non-empty "
      "but decoded_data is not empty. It is up to the caller whether to accept "
      "or reject partial success.\n"
      " - On failure, the error_message is non-empty and decoded_data is "
      "empty.")
      .def_readonly("decoded_data",
                    &wuffs_aux_wrap::ImageDecodingResult::decoded_data,
                    "VectorUint8: a byte buffer containing decoded data.")
      .def_readonly("reported_metadata",
                    &wuffs_aux_wrap::ImageDecodingResult::reported_metadata,
                    "VectorMetadataEntry: a list-like object containing reported data "
                    "(only filled is any metadata was decoded and the "
                    "corresponding ImageDecoderFlag flag was set).")
      .def_readonly("error_message",
                    &wuffs_aux_wrap::ImageDecodingResult::error_message,
                    "str: error message, empty on success, one of "
                    "ImageDecoderError* on error.");

  aux_m.attr("ImageDecoderErrorMaxInclDimensionExceeded") =
      wuffs_aux::DecodeImage_MaxInclDimensionExceeded;
  aux_m.attr("ImageDecoderErrorMaxInclMetadataLengthExceeded") =
      wuffs_aux::DecodeImage_MaxInclMetadataLengthExceeded;
  aux_m.attr("ImageDecoderErrorMaxInclMetadataLengthExceeded") =
      wuffs_aux::DecodeImage_OutOfMemory;
  aux_m.attr("ImageDecoderErrorUnexpectedEndOfFile") =
      wuffs_aux::DecodeImage_UnexpectedEndOfFile;
  aux_m.attr("ImageDecoderErrorUnsupportedImageFormat") =
      wuffs_aux::DecodeImage_UnsupportedImageFormat;
  aux_m.attr("ImageDecoderErrorUnsupportedMetadata") =
      wuffs_aux::DecodeImage_UnsupportedMetadata;
  aux_m.attr("ImageDecoderErrorUnsupportedPixelBlend") =
      wuffs_aux::DecodeImage_UnsupportedPixelBlend;
  aux_m.attr("ImageDecoderErrorUnsupportedPixelConfiguration") =
      wuffs_aux::DecodeImage_UnsupportedPixelConfiguration;
  aux_m.attr("ImageDecoderErrorUnsupportedPixelFormat") =
      wuffs_aux::DecodeImage_UnsupportedPixelFormat;

  py::class_<wuffs_aux_wrap::ImageDecoder>(aux_m, "ImageDecoder",
                                           "Image decoder class.")
      .def(py::init<const wuffs_aux_wrap::ImageDecoderConfig&>(),
           "Sole constructor.\n\n"
           "Args:"
           "\n config (ImageDecoderConfig): image decoder config.")
      .def(
          "decode",
          [](wuffs_aux_wrap::ImageDecoder& image_decoder,
             const py::bytes& data) -> wuffs_aux_wrap::ImageDecodingResult {
            py::buffer_info data_view(py::buffer(data).request());
            return image_decoder.Decode(
                reinterpret_cast<uint8_t*>(data_view.ptr), data_view.size);
          },
          "Decodes image using given byte buffer.\n\n"
          "Args:"
          "\n data (bytes): a byte buffer holding encoded image."
          "\nReturns:"
          "\n ImageDecodingResult: image decoding result.")
      .def(
          "decode",
          [](wuffs_aux_wrap::ImageDecoder& image_decoder,
             const std::string& path_to_file)
              -> wuffs_aux_wrap::ImageDecodingResult {
            return image_decoder.Decode(path_to_file);
          },
          "Decodes image using given file path.\n\n"
          "Args:"
          "\n path_to_file (str): path to an image file."
          "\nReturns:"
          "\n ImageDecodingResult: image decoding result.");
}