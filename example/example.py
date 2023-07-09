from pywuffs.aux import (
    ImageDecoder,
    ImageDecoderConfig,
    ImageDecoderType,
    ImageDecoderFlags,
    PixelFormat
)

config = ImageDecoderConfig()
config.enabled_decoders = [ImageDecoderType.PNG]
config.flags = [ImageDecoderFlags.REPORT_METADATA_EXIF]
config.pixel_format = PixelFormat.BGR

decoder = ImageDecoder(config)

decoding_result = decoder.decode("lena.png")

print(bytes(memoryview(decoding_result.decoded_data)))
