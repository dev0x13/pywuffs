#pragma once

#include <pybind11/numpy.h>
#include <pybind11/pytypes.h>

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <wuffs-unsupported-snapshot.c>

#include "wuffs-aux-utils.h"

// This API wraps the wuffs_aux API for JSON decoding. The wrapper is needed
// since the wuffs_aux API uses the callback-based approach which doesn't play
// well with Python-C++ interop.
// Although the API is different from wuffs_aux, it's supposed to be as thin as
// possible including minor aspect like the default non-configured behavior.

namespace wuffs_aux_wrap {

enum class JsonDecoderQuirks : uint32_t {
#define JDQE(quirk) quirk = WUFFS_JSON__QUIRK_##quirk
  JDQE(ALLOW_ASCII_CONTROL_CODES),
  JDQE(ALLOW_BACKSLASH_A),
  JDQE(ALLOW_BACKSLASH_CAPITAL_U),
  JDQE(ALLOW_BACKSLASH_E),
  JDQE(ALLOW_BACKSLASH_NEW_LINE),
  JDQE(ALLOW_BACKSLASH_QUESTION_MARK),
  JDQE(ALLOW_BACKSLASH_SINGLE_QUOTE),
  JDQE(ALLOW_BACKSLASH_V),
  JDQE(ALLOW_BACKSLASH_X_AS_CODE_POINTS),
  JDQE(ALLOW_BACKSLASH_ZERO),
  JDQE(ALLOW_COMMENT_BLOCK),
  JDQE(ALLOW_COMMENT_LINE),
  JDQE(ALLOW_EXTRA_COMMA),
  JDQE(ALLOW_INF_NAN_NUMBERS),
  JDQE(ALLOW_LEADING_ASCII_RECORD_SEPARATOR),
  JDQE(ALLOW_LEADING_UNICODE_BYTE_ORDER_MARK),
  JDQE(ALLOW_TRAILING_FILLER),
  JDQE(EXPECT_TRAILING_NEW_LINE_OR_EOF),
  JDQE(JSON_POINTER_ALLOW_TILDE_N_TILDE_R_TILDE_T),
  JDQE(REPLACE_INVALID_UNICODE)
#undef JDQE
};

// This struct hosts wuffs_aux::DecodeJson arguments in more user- and
// Python- friendly fashion
struct JsonDecoderConfig {
  std::map<JsonDecoderQuirks, uint64_t> quirks;
  std::string json_pointer;
};

struct JsonDecodingResult {
  pybind11::object parsed;
  std::string error_message;
  uint64_t cursor_position = 0;

  JsonDecodingResult() = default;

  JsonDecodingResult(JsonDecodingResult&& other) noexcept {
    std::swap(parsed, other.parsed);
    std::swap(error_message, other.error_message);
    std::swap(cursor_position, other.cursor_position);
  }

  JsonDecodingResult& operator=(JsonDecodingResult&& other) noexcept {
    if (this != &other) {
      std::swap(parsed, other.parsed);
      std::swap(error_message, other.error_message);
      std::swap(cursor_position, other.cursor_position);
    }
    return *this;
  }

  JsonDecodingResult(JsonDecodingResult& other) = delete;
  JsonDecodingResult& operator=(JsonDecodingResult& other) = delete;
};

struct JsonDecoderError {
  static const std::string BadJsonPointer;
  static const std::string NoMatch;
  static const std::string DuplicateMapKey;
  static const std::string NonStringMapKey;
  static const std::string NonContainerStackEntry;
  static const std::string BadDepth;
  static const std::string FailedToOpenFile;
  static const std::string BadC0ControlCode;
  static const std::string BadUtf8;
  static const std::string BadBackslashEscape;
  static const std::string BadInput;
  static const std::string BadNewLineInAString;
  static const std::string BadQuirkCombination;
  static const std::string UnsupportedNumberLength;
  static const std::string UnsupportedRecursionDepth;
};

const std::string JsonDecoderError::BadJsonPointer =
    wuffs_aux::DecodeJson_BadJsonPointer;
const std::string JsonDecoderError::NoMatch = wuffs_aux::DecodeJson_NoMatch;
const std::string JsonDecoderError::DuplicateMapKey =
    "wuffs_aux_wrap::JsonDecoder::Decode: duplicate map key: key=";
const std::string JsonDecoderError::NonStringMapKey =
    "wuffs_aux_wrap::JsonDecoder::Decode: non-string map key";
const std::string JsonDecoderError::NonContainerStackEntry =
    "wuffs_aux_wrap::JsonDecoder::Decode: non-container stack entry";
const std::string JsonDecoderError::BadDepth =
    "wuffs_aux_wrap::JsonDecoder::Decode: bad depth";
const std::string JsonDecoderError::FailedToOpenFile =
    "wuffs_aux_wrap::JsonDecoder::Decode: failed to open file";
// + 1 is for stripping leading '#'
const std::string JsonDecoderError::BadC0ControlCode =
    wuffs_json__error__bad_c0_control_code + 1;
const std::string JsonDecoderError::BadUtf8 = wuffs_json__error__bad_utf_8 + 1;
const std::string JsonDecoderError::BadBackslashEscape =
    wuffs_json__error__bad_backslash_escape + 1;
const std::string JsonDecoderError::BadInput = wuffs_json__error__bad_input + 1;
const std::string JsonDecoderError::BadNewLineInAString =
    wuffs_json__error__bad_new_line_in_a_string + 1;
const std::string JsonDecoderError::BadQuirkCombination =
    wuffs_json__error__bad_quirk_combination + 1;
const std::string JsonDecoderError::UnsupportedNumberLength =
    wuffs_json__error__unsupported_number_length + 1;
const std::string JsonDecoderError::UnsupportedRecursionDepth =
    wuffs_json__error__unsupported_recursion_depth + 1;

class JsonDecoder : public wuffs_aux::DecodeJsonCallbacks {
 public:
  struct Entry {
    Entry(pybind11::object&& jvalue_arg)
        : jvalue(std::move(jvalue_arg)), has_map_key(false), map_key() {}

    pybind11::object jvalue;
    bool has_map_key;
    std::string map_key;

    bool IsList() { return pybind11::isinstance<pybind11::list>(jvalue); }

    bool IsDict() { return pybind11::isinstance<pybind11::dict>(jvalue); }
  };

  explicit JsonDecoder(const JsonDecoderConfig& config)
      : quirks_vector_(utils::ConvertQuirks(config.quirks)),
        quirks_(wuffs_aux::DecodeJsonArgQuirks(quirks_vector_.data(),
                                               quirks_vector_.size())),
        json_pointer_(config.json_pointer) {}

  /* DecodeJsonCallbacks methods implementation */

  std::string Append(pybind11::object&& jvalue) {
    pybind11::dict a;
    if (stack_.empty()) {
      stack_.emplace_back(std::move(jvalue));
      return "";
    }
    Entry& top = stack_.back();
    if (top.IsList()) {
      top.jvalue.cast<pybind11::list>().append(std::move(jvalue));
      return "";
    } else if (top.IsDict()) {
      const pybind11::dict& jmap = top.jvalue.cast<pybind11::dict>();
      if (top.has_map_key) {
        top.has_map_key = false;
        if (jmap.contains(top.map_key)) {
          return JsonDecoderError::DuplicateMapKey + top.map_key;
        }
        jmap[top.map_key.c_str()] = jvalue;
        return "";
      } else if (pybind11::isinstance<pybind11::str>(jvalue)) {
        top.has_map_key = true;
        top.map_key = jvalue.cast<pybind11::str>();
        return "";
      }
      return "main: internal error: non-string map key";
    } else {
      return "main: internal error: non-container stack entry";
    }
  }

  std::string AppendNull() override { return Append(pybind11::none()); }

  std::string AppendBool(bool val) override {
    return Append(pybind11::bool_(val));
  }

  std::string AppendI64(int64_t val) override {
    return Append(pybind11::int_(val));
  }

  std::string AppendF64(double val) override {
    return Append(pybind11::float_(val));
  }

  std::string AppendTextString(std::string&& val) override {
    return Append(pybind11::str(val));
  }

  std::string Push(uint32_t flags) override {
    if (flags & WUFFS_BASE__TOKEN__VBD__STRUCTURE__TO_LIST) {
      stack_.emplace_back(pybind11::list());
      return "";
    } else if (flags & WUFFS_BASE__TOKEN__VBD__STRUCTURE__TO_DICT) {
      stack_.emplace_back(pybind11::dict());
      return "";
    }
    return "main: internal error: bad push";
  }

  std::string Pop(uint32_t) override {
    if (stack_.empty()) {
      return "main: internal error: bad pop";
    }
    pybind11::object jvalue = std::move(stack_.back().jvalue);
    stack_.pop_back();
    return Append(std::move(jvalue));
  }

  /* End of DecodeJsonCallbacks methods implementation */

  JsonDecodingResult Decode(const uint8_t* data, size_t size) {
    wuffs_aux::sync_io::MemoryInput input(data, size);
    return DecodeInternal(input);
  }

  JsonDecodingResult Decode(const std::string& path_to_file) {
    FILE* f = fopen(path_to_file.c_str(), "rb");
    if (!f) {
      JsonDecodingResult result;
      result.error_message = JsonDecoderError::FailedToOpenFile;
      result.parsed = pybind11::none();
      return result;
    }
    wuffs_aux::sync_io::FileInput input(f);
    JsonDecodingResult result = DecodeInternal(input);
    fclose(f);
    return result;
  }

 private:
  JsonDecodingResult DecodeInternal(wuffs_aux::sync_io::Input& input) {
    wuffs_aux::DecodeJsonResult decode_json_result =
        wuffs_aux::DecodeJson(*this, input, quirks_, json_pointer_);
    JsonDecodingResult decoding_result;
    decoding_result.error_message = std::move(decode_json_result.error_message);
    decoding_result.cursor_position = decode_json_result.cursor_position;
    if (stack_.size() != 1) {
      decoding_result.error_message = JsonDecoderError::BadDepth;
    }
    decoding_result.parsed = decoding_result.error_message.empty()
                                 ? std::move(stack_[0].jvalue)
                                 : pybind11::none();
    stack_.clear();
    return decoding_result;
  }

 private:
  std::vector<wuffs_aux::QuirkKeyValuePair> quirks_vector_;
  wuffs_aux::DecodeJsonArgQuirks quirks_;
  wuffs_aux::DecodeJsonArgJsonPointer json_pointer_;
  std::vector<Entry> stack_;
};

}  // namespace wuffs_aux_wrap
