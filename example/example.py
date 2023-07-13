from pywuffs import ImageDecoderType, PixelFormat
from pywuffs.aux import (
    ImageDecoder,
    ImageDecoderConfig,
    ImageDecoderFlags
)

config = ImageDecoderConfig()
config.enabled_decoders = [ImageDecoderType.PNG]
config.flags = [ImageDecoderFlags.REPORT_METADATA_EXIF]
config.pixel_format = PixelFormat.BGR

decoder = ImageDecoder(config)

decoding_result = decoder.decode("lena.png")

print(decoding_result.pixbuf.shape)
print(decoding_result.pixbuf)
