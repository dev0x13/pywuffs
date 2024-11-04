import os
import json
import pytest

from pywuffs import *
from pywuffs.aux import *

JSON_PATH = os.path.join(os.path.dirname(os.path.realpath(__file__)), "json/")

# Positive test cases


def assert_decoded(result, encoded=None, file=None):
    assert encoded or file
    assert len(result.error_message) == 0
    assert result.cursor_position != 0
    if file:
        with open(file, "rb") as f:
            encoded = f.read()
    if encoded:
        assert result.parsed == json.loads(encoded.decode("utf-8"))


@pytest.mark.parametrize("file_path", [
    (JSON_PATH + "/simple.json"),
    (JSON_PATH + "/valid1.json"),
])
def test_decode_default_config(file_path):
    config = JsonDecoderConfig()
    decoder = JsonDecoder(config)
    decoding_result_from_file = decoder.decode(file_path)
    assert_decoded(decoding_result_from_file, file=file_path)
    with open(file_path, "rb") as f:
        data = f.read()
    decoding_result_from_bytes = decoder.decode(data)
    assert_decoded(decoding_result_from_bytes, encoded=data)
    assert decoding_result_from_bytes.error_message == decoding_result_from_file.error_message
    assert decoding_result_from_bytes.parsed == decoding_result_from_file.parsed
    assert decoding_result_from_bytes.cursor_position == decoding_result_from_file.cursor_position


def test_decode_json_quirks():
    config = JsonDecoderConfig()
    config.quirks = {JsonDecoderQuirks.ALLOW_COMMENT_BLOCK: 1,
                     JsonDecoderQuirks.ALLOW_EXTRA_COMMA: 1}
    decoder = JsonDecoder(config)
    data = b"{\"test\": \"value\", \"test1\": 123,}"
    decoding_result = decoder.decode(data)
    assert_decoded(decoding_result, encoded=data[:-2] + b"}")


def test_decode_json_pointer():
    data = {"key1": 1, "key2": [2, 3], "key3": "value"}
    config = JsonDecoderConfig()
    config.json_pointer = "/key2"
    decoder = JsonDecoder(config)
    decoding_result = decoder.decode(bytes(json.dumps(data), "utf-8"))
    assert_decoded(decoding_result, encoded=bytes(
        json.dumps(data["key2"]), "utf-8"))

# Negative test cases


def assert_not_decoded(result, expected_error_message=None):
    assert len(result.error_message) != 0
    if expected_error_message:
        assert result.error_message == expected_error_message
    assert result.parsed is None


def test_decode_non_existent_file():
    decoder = JsonDecoder(JsonDecoderConfig())
    decoding_result = decoder.decode("random123")
    assert_not_decoded(decoding_result, JsonDecoderError.FailedToOpenFile)


@pytest.mark.parametrize("param", [
    (b"+(=)", JsonDecoderError.BadDepth),
    (b"test", JsonDecoderError.BadDepth),
    (b"{\"val\":" + b"1"*130 + b"}", JsonDecoderError.UnsupportedNumberLength),
    (b"{\"val\": 1, \"val\": 2}", JsonDecoderError.DuplicateMapKey + "val"),
])
def test_decode_invalid_bytes(param):
    decoder = JsonDecoder(JsonDecoderConfig())
    decoding_result = decoder.decode(param[0])
    assert_not_decoded(decoding_result, param[1])


@pytest.mark.parametrize("param", [
    (JSON_PATH + "/invalid1.json", JsonDecoderError.BadDepth),
    (JSON_PATH + "/invalid2.json", JsonDecoderError.BadInput),
    (JSON_PATH + "/invalid3.json", JsonDecoderError.BadInput),
    (JSON_PATH + "/non-string-map-key.json", JsonDecoderError.BadInput)
])
def test_decode_invalid_file(param):
    decoder = JsonDecoder(JsonDecoderConfig())
    decoding_result = decoder.decode(param[0])
    assert_not_decoded(decoding_result, param[1])


def test_decode_invalid_json_pointer():
    data = {"key1": 1, "key2": [2, 3], "key3": "value"}
    config = JsonDecoderConfig()
    config.json_pointer = "/random"
    decoder = JsonDecoder(config)
    decoding_result = decoder.decode(bytes(json.dumps(data), "utf-8"))
    assert_not_decoded(decoding_result, JsonDecoderError.BadDepth)
