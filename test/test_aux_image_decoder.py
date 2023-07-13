import os
from struct import unpack
import pytest
import numpy as np

from pywuffs.aux import *
from pywuffs import *

IMAGES_PATH = os.path.join(os.path.dirname(os.path.realpath(__file__)), "images/")
TEST_IMAGES = [
    (ImageDecoderType.PNG, IMAGES_PATH + "/lena.png"),
    (ImageDecoderType.BMP, IMAGES_PATH + "/lena.bmp"),
    (ImageDecoderType.TGA, IMAGES_PATH + "/lena.tga"),
    (ImageDecoderType.NIE, IMAGES_PATH + "/hippopotamus.nie"),
    (ImageDecoderType.GIF, IMAGES_PATH + "/lena.gif"),
    (ImageDecoderType.WBMP, IMAGES_PATH + "/lena.wbmp")
]


# Positive test cases

def assert_decoded(result, expected_metadata_len=0):
    assert len(result.error_message) == 0
    if expected_metadata_len is not None:
        assert len(result.reported_metadata) == expected_metadata_len
    assert result.pixcfg.is_valid()
    assert result.pixcfg.width() != 0
    assert result.pixcfg.height() != 0
    assert result.pixbuf.dtype == np.uint8
    assert result.pixbuf.shape[0] == result.pixcfg.height()
    assert result.pixbuf.shape[1] == result.pixcfg.width()
    assert result.pixbuf.size == result.pixcfg.pixbuf_len() != 0


@pytest.mark.parametrize("param", [
    ([ImageDecoderFlags.REPORT_METADATA_EXIF], 1),
    ([ImageDecoderFlags.REPORT_METADATA_CHRM], 1),
    ([ImageDecoderFlags.REPORT_METADATA_GAMA], 1),
    ([ImageDecoderFlags.REPORT_METADATA_EXIF, ImageDecoderFlags.REPORT_METADATA_CHRM,
      ImageDecoderFlags.REPORT_METADATA_GAMA], 3),
])
def test_decode_image_with_metadata(param):
    config = ImageDecoderConfig()
    config.flags = param[0]
    decoder = ImageDecoder(config)
    decoding_result = decoder.decode(IMAGES_PATH + "/lena_exif.png")
    assert_decoded(decoding_result, param[1])


@pytest.mark.parametrize("param", TEST_IMAGES)
def test_decode_default_config(param):
    config = ImageDecoderConfig()
    decoder = ImageDecoder(config)
    decoding_result_from_file = decoder.decode(param[1])
    assert_decoded(decoding_result_from_file)
    with open(param[1], "rb") as f:
        data = f.read()
    decoding_result_from_bytes = decoder.decode(data)
    assert_decoded(decoding_result_from_bytes)
    assert decoding_result_from_bytes.error_message == decoding_result_from_file.error_message
    assert decoding_result_from_bytes.reported_metadata == decoding_result_from_file.reported_metadata
    assert np.array_equal(decoding_result_from_bytes.pixbuf, decoding_result_from_file.pixbuf)
    assert decoding_result_from_bytes.pixcfg.is_valid() == decoding_result_from_file.pixcfg.is_valid()
    assert decoding_result_from_bytes.pixcfg.width() == decoding_result_from_file.pixcfg.width()
    assert decoding_result_from_bytes.pixcfg.height() == decoding_result_from_file.pixcfg.height()
    assert decoding_result_from_bytes.pixcfg.pixel_format() == decoding_result_from_file.pixcfg.pixel_format()
    assert decoding_result_from_bytes.pixcfg.pixel_subsampling() == decoding_result_from_file.pixcfg.pixel_subsampling()
    assert decoding_result_from_bytes.pixcfg.pixbuf_len() == decoding_result_from_file.pixcfg.pixbuf_len()


@pytest.mark.parametrize("param", TEST_IMAGES)
def test_decode_specific_decoder(param):
    config = ImageDecoderConfig()
    config.enabled_decoders = [param[0]]
    decoder = ImageDecoder(config)
    decoding_result = decoder.decode(param[1])
    assert_decoded(decoding_result)


@pytest.mark.parametrize("pixel_format", [
    PixelFormat.BGR_565,
    PixelFormat.BGR,
    PixelFormat.BGRA_NONPREMUL,
    PixelFormat.BGRA_NONPREMUL_4X16LE,
    PixelFormat.BGRA_PREMUL,
    PixelFormat.RGBA_NONPREMUL,
    PixelFormat.RGBA_PREMUL
])
@pytest.mark.parametrize("pixel_blend", [
    PixelBlend.SRC,
    PixelBlend.SRC_OVER
])
@pytest.mark.parametrize("test_image", TEST_IMAGES)
def test_decode_pixel_format_and_blend(pixel_format, pixel_blend, test_image):
    config = ImageDecoderConfig()
    config.pixel_format = pixel_format
    config.pixel_blend = pixel_blend
    decoder = ImageDecoder(config)
    decoding_result = decoder.decode(test_image[1])
    assert_decoded(decoding_result)
    assert decoding_result.pixcfg.pixel_format() == pixel_format


@pytest.mark.parametrize("background_color", [0, 1, 0x7F7F_0000])
@pytest.mark.parametrize("test_image", TEST_IMAGES)
def test_decode_background_color(background_color, test_image):
    config = ImageDecoderConfig()
    config.background_color = background_color
    decoder = ImageDecoder(config)
    decoding_result = decoder.decode(test_image[1])
    assert_decoded(decoding_result)


@pytest.mark.parametrize("test_image", TEST_IMAGES)
@pytest.mark.parametrize("quirk", [
    ImageDecoderQuirks.IGNORE_CHECKSUM,
    ImageDecoderQuirks.GIF_DELAY_NUM_DECODED_FRAMES,
    ImageDecoderQuirks.GIF_FIRST_FRAME_LOCAL_PALETTE_MEANS_BLACK_BACKGROUND,
    ImageDecoderQuirks.GIF_QUIRK_HONOR_BACKGROUND_COLOR,
    ImageDecoderQuirks.GIF_IGNORE_TOO_MUCH_PIXEL_DATA,
    ImageDecoderQuirks.GIF_IMAGE_BOUNDS_ARE_STRICT,
    ImageDecoderQuirks.GIF_REJECT_EMPTY_FRAME,
    ImageDecoderQuirks.GIF_REJECT_EMPTY_PALETTE
])
def test_decode_image_quirks(test_image, quirk):
    config = ImageDecoderConfig()
    config.quirks = [quirk]
    decoder = ImageDecoder(config)
    decoding_result = decoder.decode(test_image[1])
    assert_decoded(decoding_result)


@pytest.mark.parametrize("test_image", TEST_IMAGES)
@pytest.mark.parametrize("flag", [
    ImageDecoderFlags.REPORT_METADATA_BGCL,
    ImageDecoderFlags.REPORT_METADATA_CHRM,
    ImageDecoderFlags.REPORT_METADATA_EXIF,
    ImageDecoderFlags.REPORT_METADATA_GAMA,
    ImageDecoderFlags.REPORT_METADATA_ICCP,
    ImageDecoderFlags.REPORT_METADATA_MTIM,
    ImageDecoderFlags.REPORT_METADATA_OFS2,
    ImageDecoderFlags.REPORT_METADATA_PHYD,
    ImageDecoderFlags.REPORT_METADATA_SRGB,
    ImageDecoderFlags.REPORT_METADATA_XMP
])
def test_decode_image_flags(test_image, flag):
    config = ImageDecoderConfig()
    config.flags = [flag]
    decoder = ImageDecoder(config)
    decoding_result = decoder.decode(test_image[1])
    assert_decoded(decoding_result, None)


def test_decode_image_exif_metadata():
    config = ImageDecoderConfig()
    config.flags = [ImageDecoderFlags.REPORT_METADATA_EXIF]
    decoder = ImageDecoder(config)
    decoding_result = decoder.decode(IMAGES_PATH + "/lena_exif.png")
    assert_decoded(decoding_result, 1)
    assert decoding_result.pixbuf.shape == (32, 32, 4)
    meta_minfo = decoding_result.reported_metadata[0].minfo
    meta_bytes = decoding_result.reported_metadata[0].data.tobytes()
    assert meta_minfo.metadata__fourcc() == 1163413830  # EXIF
    assert meta_bytes[:2] == b"II"  # little endian
    exif_orientation = 0
    cursor = 0

    def get_uint16():
        return unpack("@H", meta_bytes[cursor:cursor + 2])[0]

    while cursor < len(meta_bytes):
        if get_uint16() == 0x0112:  # orientation flag
            cursor += 8
            exif_orientation = get_uint16()
        cursor += 2
    assert exif_orientation == 3


# Negative test cases

def assert_not_decoded(result, expected_error_message=None, expected_metadata_length=0):
    assert len(result.error_message) != 0
    if expected_error_message:
        assert result.error_message == expected_error_message
    assert not result.pixcfg.is_valid()
    assert result.pixcfg.width() == 0
    assert result.pixcfg.height() == 0
    assert result.pixbuf.dtype == np.uint8
    assert result.pixbuf.size == result.pixcfg.pixbuf_len() == 0
    assert len(result.reported_metadata) == expected_metadata_length


def test_decode_image_invalid_kvp_chunk():
    config = ImageDecoderConfig()
    config.flags = [ImageDecoderFlags.REPORT_METADATA_KVP]
    decoder = ImageDecoder(config)
    decoding_result = decoder.decode(IMAGES_PATH + "/lena.png")
    assert_not_decoded(decoding_result, "png: bad text chunk (not Latin-1)", 1)


def test_decode_non_existent_file():
    decoder = ImageDecoder(ImageDecoderConfig())
    decoding_result = decoder.decode("random123")
    assert_not_decoded(decoding_result, ImageDecoderError.FailedToOpenFile)


@pytest.mark.parametrize("param", [b"", b"123"])
def test_decode_invalid_bytes(param):
    decoder = ImageDecoder(ImageDecoderConfig())
    decoding_result = decoder.decode(param)
    assert_not_decoded(decoding_result, ImageDecoderError.UnsupportedImageFormat)


@pytest.mark.parametrize("param", TEST_IMAGES)
def test_decode_image_formats_truncated(param):
    config = ImageDecoderConfig()
    config.enabled_decoders = [param[0]]
    decoder = ImageDecoder(config)
    with open(param[1], "rb") as f:
        data = f.read()
    data = data[:len(data) - 64]
    decoding_result = decoder.decode(data)
    assert decoding_result.error_message.endswith("truncated input")
    assert decoding_result.pixcfg.is_valid()
    assert decoding_result.pixcfg.width() != 0
    assert decoding_result.pixcfg.height() != 0
    assert decoding_result.pixbuf.dtype == np.uint8
    assert decoding_result.pixbuf.size == decoding_result.pixcfg.pixbuf_len() != 0
    assert len(decoding_result.reported_metadata) == 0


@pytest.mark.parametrize("param", TEST_IMAGES)
def test_decode_image_unsupported_image_format(param):
    config = ImageDecoderConfig()
    enabled_decoders = config.enabled_decoders
    enabled_decoders.remove(param[0])
    config.enabled_decoders = enabled_decoders
    decoder = ImageDecoder(config)
    decoding_result = decoder.decode(param[1])
    assert_not_decoded(decoding_result, ImageDecoderError.UnsupportedImageFormat)


@pytest.mark.parametrize("test_image", TEST_IMAGES)
def test_decode_image_unsupported_pixel_format(test_image):
    config = ImageDecoderConfig()
    config.pixel_format = PixelFormat.RGB
    decoder = ImageDecoder(config)
    decoding_result = decoder.decode(test_image[1])
    assert_not_decoded(decoding_result, ImageDecoderError.UnsupportedPixelFormat)


@pytest.mark.parametrize("test_image", TEST_IMAGES)
def test_decode_image_max_incl_dimension(test_image):
    config = ImageDecoderConfig()
    config.max_incl_dimension = 8
    decoder = ImageDecoder(config)
    decoding_result = decoder.decode(test_image[1])
    assert_not_decoded(decoding_result, ImageDecoderError.MaxInclDimensionExceeded)


def test_decode_image_max_incl_metadata_length():
    config = ImageDecoderConfig()
    config.max_incl_metadata_length = 8
    config.flags = [ImageDecoderFlags.REPORT_METADATA_EXIF]
    decoder = ImageDecoder(config)
    decoding_result = decoder.decode(IMAGES_PATH + "/lena_exif.png")
    assert_not_decoded(decoding_result, ImageDecoderError.MaxInclMetadataLengthExceeded)
