#ifndef SRC_NODE_HTTP2_H_
#define SRC_NODE_HTTP2_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "node_http2_core-inl.h"
#include "node_http2_state.h"
#include "stream_base-inl.h"
#include "string_bytes.h"

#include <queue>

namespace node {
namespace http2 {

using v8::Array;
using v8::Context;
using v8::EscapableHandleScope;
using v8::Isolate;
using v8::MaybeLocal;

// Unlike the HTTP/1 implementation, the HTTP/2 implementation is not limited
// to a fixed number of known supported HTTP methods. These constants, therefore
// are provided strictly as a convenience to users and are exposed via the
// require('http2').constants object.
#define HTTP_KNOWN_METHODS(V)                                                 \
  V(ACL, "ACL")                                                               \
  V(BASELINE_CONTROL, "BASELINE-CONTROL")                                     \
  V(BIND, "BIND")                                                             \
  V(CHECKIN, "CHECKIN")                                                       \
  V(CHECKOUT, "CHECKOUT")                                                     \
  V(CONNECT, "CONNECT")                                                       \
  V(COPY, "COPY")                                                             \
  V(DELETE, "DELETE")                                                         \
  V(GET, "GET")                                                               \
  V(HEAD, "HEAD")                                                             \
  V(LABEL, "LABEL")                                                           \
  V(LINK, "LINK")                                                             \
  V(LOCK, "LOCK")                                                             \
  V(MERGE, "MERGE")                                                           \
  V(MKACTIVITY, "MKACTIVITY")                                                 \
  V(MKCALENDAR, "MKCALENDAR")                                                 \
  V(MKCOL, "MKCOL")                                                           \
  V(MKREDIRECTREF, "MKREDIRECTREF")                                           \
  V(MKWORKSPACE, "MKWORKSPACE")                                               \
  V(MOVE, "MOVE")                                                             \
  V(OPTIONS, "OPTIONS")                                                       \
  V(ORDERPATCH, "ORDERPATCH")                                                 \
  V(PATCH, "PATCH")                                                           \
  V(POST, "POST")                                                             \
  V(PRI, "PRI")                                                               \
  V(PROPFIND, "PROPFIND")                                                     \
  V(PROPPATCH, "PROPPATCH")                                                   \
  V(PUT, "PUT")                                                               \
  V(REBIND, "REBIND")                                                         \
  V(REPORT, "REPORT")                                                         \
  V(SEARCH, "SEARCH")                                                         \
  V(TRACE, "TRACE")                                                           \
  V(UNBIND, "UNBIND")                                                         \
  V(UNCHECKOUT, "UNCHECKOUT")                                                 \
  V(UNLINK, "UNLINK")                                                         \
  V(UNLOCK, "UNLOCK")                                                         \
  V(UPDATE, "UPDATE")                                                         \
  V(UPDATEREDIRECTREF, "UPDATEREDIRECTREF")                                   \
  V(VERSION_CONTROL, "VERSION-CONTROL")

// These are provided strictly as a convenience to users and are exposed via the
// require('http2').constants objects
#define HTTP_KNOWN_HEADERS(V)                                                 \
  V(STATUS, ":status")                                                        \
  V(METHOD, ":method")                                                        \
  V(AUTHORITY, ":authority")                                                  \
  V(SCHEME, ":scheme")                                                        \
  V(PATH, ":path")                                                            \
  V(ACCEPT_CHARSET, "accept-charset")                                         \
  V(ACCEPT_ENCODING, "accept-encoding")                                       \
  V(ACCEPT_LANGUAGE, "accept-language")                                       \
  V(ACCEPT_RANGES, "accept-ranges")                                           \
  V(ACCEPT, "accept")                                                         \
  V(ACCESS_CONTROL_ALLOW_CREDENTIALS, "access-control-allow-credentials")     \
  V(ACCESS_CONTROL_ALLOW_HEADERS, "access-control-allow-headers")             \
  V(ACCESS_CONTROL_ALLOW_METHODS, "access-control-allow-methods")             \
  V(ACCESS_CONTROL_ALLOW_ORIGIN, "access-control-allow-origin")               \
  V(ACCESS_CONTROL_EXPOSE_HEADERS, "access-control-expose-headers")           \
  V(ACCESS_CONTROL_MAX_AGE, "access-control-max-age")                         \
  V(ACCESS_CONTROL_REQUEST_HEADERS, "access-control-request-headers")         \
  V(ACCESS_CONTROL_REQUEST_METHOD, "access-control-request-method")           \
  V(AGE, "age")                                                               \
  V(ALLOW, "allow")                                                           \
  V(AUTHORIZATION, "authorization")                                           \
  V(CACHE_CONTROL, "cache-control")                                           \
  V(CONNECTION, "connection")                                                 \
  V(CONTENT_DISPOSITION, "content-disposition")                               \
  V(CONTENT_ENCODING, "content-encoding")                                     \
  V(CONTENT_LANGUAGE, "content-language")                                     \
  V(CONTENT_LENGTH, "content-length")                                         \
  V(CONTENT_LOCATION, "content-location")                                     \
  V(CONTENT_MD5, "content-md5")                                               \
  V(CONTENT_RANGE, "content-range")                                           \
  V(CONTENT_TYPE, "content-type")                                             \
  V(COOKIE, "cookie")                                                         \
  V(DATE, "date")                                                             \
  V(DNT, "dnt")                                                               \
  V(ETAG, "etag")                                                             \
  V(EXPECT, "expect")                                                         \
  V(EXPIRES, "expires")                                                       \
  V(FORWARDED, "forwarded")                                                   \
  V(FROM, "from")                                                             \
  V(HOST, "host")                                                             \
  V(IF_MATCH, "if-match")                                                     \
  V(IF_MODIFIED_SINCE, "if-modified-since")                                   \
  V(IF_NONE_MATCH, "if-none-match")                                           \
  V(IF_RANGE, "if-range")                                                     \
  V(IF_UNMODIFIED_SINCE, "if-unmodified-since")                               \
  V(LAST_MODIFIED, "last-modified")                                           \
  V(LINK, "link")                                                             \
  V(LOCATION, "location")                                                     \
  V(MAX_FORWARDS, "max-forwards")                                             \
  V(PREFER, "prefer")                                                         \
  V(PROXY_AUTHENTICATE, "proxy-authenticate")                                 \
  V(PROXY_AUTHORIZATION, "proxy-authorization")                               \
  V(RANGE, "range")                                                           \
  V(REFERER, "referer")                                                       \
  V(REFRESH, "refresh")                                                       \
  V(RETRY_AFTER, "retry-after")                                               \
  V(SERVER, "server")                                                         \
  V(SET_COOKIE, "set-cookie")                                                 \
  V(STRICT_TRANSPORT_SECURITY, "strict-transport-security")                   \
  V(TRAILER, "trailer")                                                       \
  V(TRANSFER_ENCODING, "transfer-encoding")                                   \
  V(TE, "te")                                                                 \
  V(TK, "tk")                                                                 \
  V(UPGRADE_INSECURE_REQUESTS, "upgrade-insecure-requests")                   \
  V(UPGRADE, "upgrade")                                                       \
  V(USER_AGENT, "user-agent")                                                 \
  V(VARY, "vary")                                                             \
  V(VIA, "via")                                                               \
  V(WARNING, "warning")                                                       \
  V(WWW_AUTHENTICATE, "www-authenticate")                                     \
  V(X_CONTENT_TYPE_OPTIONS, "x-content-type-options")                         \
  V(X_FRAME_OPTIONS, "x-frame-options")                                       \
  V(HTTP2_SETTINGS, "http2-settings")                                         \
  V(KEEP_ALIVE, "keep-alive")                                                 \
  V(PROXY_CONNECTION, "proxy-connection")

enum http_known_headers {
HTTP_KNOWN_HEADER_MIN,
#define V(name, value) HTTP_HEADER_##name,
HTTP_KNOWN_HEADERS(V)
#undef V
HTTP_KNOWN_HEADER_MAX
};

// While some of these codes are used within the HTTP/2 implementation in
// core, they are provided strictly as a convenience to users and are exposed
// via the require('http2').constants object.
#define HTTP_STATUS_CODES(V)                                                  \
  V(CONTINUE, 100)                                                            \
  V(SWITCHING_PROTOCOLS, 101)                                                 \
  V(PROCESSING, 102)                                                          \
  V(OK, 200)                                                                  \
  V(CREATED, 201)                                                             \
  V(ACCEPTED, 202)                                                            \
  V(NON_AUTHORITATIVE_INFORMATION, 203)                                       \
  V(NO_CONTENT, 204)                                                          \
  V(RESET_CONTENT, 205)                                                       \
  V(PARTIAL_CONTENT, 206)                                                     \
  V(MULTI_STATUS, 207)                                                        \
  V(ALREADY_REPORTED, 208)                                                    \
  V(IM_USED, 226)                                                             \
  V(MULTIPLE_CHOICES, 300)                                                    \
  V(MOVED_PERMANENTLY, 301)                                                   \
  V(FOUND, 302)                                                               \
  V(SEE_OTHER, 303)                                                           \
  V(NOT_MODIFIED, 304)                                                        \
  V(USE_PROXY, 305)                                                           \
  V(TEMPORARY_REDIRECT, 307)                                                  \
  V(PERMANENT_REDIRECT, 308)                                                  \
  V(BAD_REQUEST, 400)                                                         \
  V(UNAUTHORIZED, 401)                                                        \
  V(PAYMENT_REQUIRED, 402)                                                    \
  V(FORBIDDEN, 403)                                                           \
  V(NOT_FOUND, 404)                                                           \
  V(METHOD_NOT_ALLOWED, 405)                                                  \
  V(NOT_ACCEPTABLE, 406)                                                      \
  V(PROXY_AUTHENTICATION_REQUIRED, 407)                                       \
  V(REQUEST_TIMEOUT, 408)                                                     \
  V(CONFLICT, 409)                                                            \
  V(GONE, 410)                                                                \
  V(LENGTH_REQUIRED, 411)                                                     \
  V(PRECONDITION_FAILED, 412)                                                 \
  V(PAYLOAD_TOO_LARGE, 413)                                                   \
  V(URI_TOO_LONG, 414)                                                        \
  V(UNSUPPORTED_MEDIA_TYPE, 415)                                              \
  V(RANGE_NOT_SATISFIABLE, 416)                                               \
  V(EXPECTATION_FAILED, 417)                                                  \
  V(TEAPOT, 418)                                                              \
  V(MISDIRECTED_REQUEST, 421)                                                 \
  V(UNPROCESSABLE_ENTITY, 422)                                                \
  V(LOCKED, 423)                                                              \
  V(FAILED_DEPENDENCY, 424)                                                   \
  V(UNORDERED_COLLECTION, 425)                                                \
  V(UPGRADE_REQUIRED, 426)                                                    \
  V(PRECONDITION_REQUIRED, 428)                                               \
  V(TOO_MANY_REQUESTS, 429)                                                   \
  V(REQUEST_HEADER_FIELDS_TOO_LARGE, 431)                                     \
  V(UNAVAILABLE_FOR_LEGAL_REASONS, 451)                                       \
  V(INTERNAL_SERVER_ERROR, 500)                                               \
  V(NOT_IMPLEMENTED, 501)                                                     \
  V(BAD_GATEWAY, 502)                                                         \
  V(SERVICE_UNAVAILABLE, 503)                                                 \
  V(GATEWAY_TIMEOUT, 504)                                                     \
  V(HTTP_VERSION_NOT_SUPPORTED, 505)                                          \
  V(VARIANT_ALSO_NEGOTIATES, 506)                                             \
  V(INSUFFICIENT_STORAGE, 507)                                                \
  V(LOOP_DETECTED, 508)                                                       \
  V(BANDWIDTH_LIMIT_EXCEEDED, 509)                                            \
  V(NOT_EXTENDED, 510)                                                        \
  V(NETWORK_AUTHENTICATION_REQUIRED, 511)

enum http_status_codes {
#define V(name, code) HTTP_STATUS_##name = code,
HTTP_STATUS_CODES(V)
#undef V
};

// The Padding Strategy determines the method by which extra padding is
// selected for HEADERS and DATA frames. These are configurable via the
// options passed in to a Http2Session object.
enum padding_strategy_type {
  // No padding strategy. This is the default.
  PADDING_STRATEGY_NONE,
  // Padding will ensure all data frames are maxFrameSize
  PADDING_STRATEGY_MAX,
  // Padding will be determined via a JS callback. Note that this can be
  // expensive because the callback is called once for every DATA and
  // HEADERS frame. For performance reasons, this strategy should be
  // avoided.
  PADDING_STRATEGY_CALLBACK
};

// These are the error codes provided by the underlying nghttp2 implementation.
#define NGHTTP2_ERROR_CODES(V)                                                 \
  V(NGHTTP2_ERR_INVALID_ARGUMENT)                                              \
  V(NGHTTP2_ERR_BUFFER_ERROR)                                                  \
  V(NGHTTP2_ERR_UNSUPPORTED_VERSION)                                           \
  V(NGHTTP2_ERR_WOULDBLOCK)                                                    \
  V(NGHTTP2_ERR_PROTO)                                                         \
  V(NGHTTP2_ERR_INVALID_FRAME)                                                 \
  V(NGHTTP2_ERR_EOF)                                                           \
  V(NGHTTP2_ERR_DEFERRED)                                                      \
  V(NGHTTP2_ERR_STREAM_ID_NOT_AVAILABLE)                                       \
  V(NGHTTP2_ERR_STREAM_CLOSED)                                                 \
  V(NGHTTP2_ERR_STREAM_CLOSING)                                                \
  V(NGHTTP2_ERR_STREAM_SHUT_WR)                                                \
  V(NGHTTP2_ERR_INVALID_STREAM_ID)                                             \
  V(NGHTTP2_ERR_INVALID_STREAM_STATE)                                          \
  V(NGHTTP2_ERR_DEFERRED_DATA_EXIST)                                           \
  V(NGHTTP2_ERR_START_STREAM_NOT_ALLOWED)                                      \
  V(NGHTTP2_ERR_GOAWAY_ALREADY_SENT)                                           \
  V(NGHTTP2_ERR_INVALID_HEADER_BLOCK)                                          \
  V(NGHTTP2_ERR_INVALID_STATE)                                                 \
  V(NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE)                                     \
  V(NGHTTP2_ERR_FRAME_SIZE_ERROR)                                              \
  V(NGHTTP2_ERR_HEADER_COMP)                                                   \
  V(NGHTTP2_ERR_FLOW_CONTROL)                                                  \
  V(NGHTTP2_ERR_INSUFF_BUFSIZE)                                                \
  V(NGHTTP2_ERR_PAUSE)                                                         \
  V(NGHTTP2_ERR_TOO_MANY_INFLIGHT_SETTINGS)                                    \
  V(NGHTTP2_ERR_PUSH_DISABLED)                                                 \
  V(NGHTTP2_ERR_DATA_EXIST)                                                    \
  V(NGHTTP2_ERR_SESSION_CLOSING)                                               \
  V(NGHTTP2_ERR_HTTP_HEADER)                                                   \
  V(NGHTTP2_ERR_HTTP_MESSAGING)                                                \
  V(NGHTTP2_ERR_REFUSED_STREAM)                                                \
  V(NGHTTP2_ERR_INTERNAL)                                                      \
  V(NGHTTP2_ERR_CANCEL)                                                        \
  V(NGHTTP2_ERR_FATAL)                                                         \
  V(NGHTTP2_ERR_NOMEM)                                                         \
  V(NGHTTP2_ERR_CALLBACK_FAILURE)                                              \
  V(NGHTTP2_ERR_BAD_CLIENT_MAGIC)                                              \
  V(NGHTTP2_ERR_FLOODED)

const char* nghttp2_errname(int rv) {
  switch (rv) {
#define V(code) case code: return #code;
  NGHTTP2_ERROR_CODES(V)
#undef V
    default:
      return "NGHTTP2_UNKNOWN_ERROR";
  }
}

#define DEFAULT_SETTINGS_HEADER_TABLE_SIZE 4096
#define DEFAULT_SETTINGS_ENABLE_PUSH 1
#define DEFAULT_SETTINGS_INITIAL_WINDOW_SIZE 65535
#define DEFAULT_SETTINGS_MAX_FRAME_SIZE 16384
#define MAX_MAX_FRAME_SIZE 16777215
#define MIN_MAX_FRAME_SIZE DEFAULT_SETTINGS_MAX_FRAME_SIZE
#define MAX_INITIAL_WINDOW_SIZE 2147483647

// The Http2Options class is used to parse the options object passed in to
// a Http2Session object and convert those into an appropriate nghttp2_option
// struct. This is the primary mechanism by which the Http2Session object is
// configured.
class Http2Options {
 public:
  explicit Http2Options(Environment* env);

  ~Http2Options() {
    nghttp2_option_del(options_);
  }

  nghttp2_option* operator*() const {
    return options_;
  }

  void SetPaddingStrategy(padding_strategy_type val) {
#if DEBUG
    CHECK_LE(val, PADDING_STRATEGY_CALLBACK);
#endif
    padding_strategy_ = static_cast<padding_strategy_type>(val);
  }

  padding_strategy_type GetPaddingStrategy() {
    return padding_strategy_;
  }

 private:
  nghttp2_option* options_;
  padding_strategy_type padding_strategy_ = PADDING_STRATEGY_NONE;
};

// This allows for 4 default-sized frames with their frame headers
static const size_t kAllocBufferSize = 4 * (16384 + 9);

typedef uint32_t(*get_setting)(nghttp2_session* session,
                               nghttp2_settings_id id);

class Http2Session : public AsyncWrap,
                     public StreamBase,
                     public Nghttp2Session {
 public:
  Http2Session(Environment* env,
               Local<Object> wrap,
               nghttp2_session_type type);
  ~Http2Session() override;

  static void OnStreamAllocImpl(size_t suggested_size,
                                uv_buf_t* buf,
                                void* ctx);
  static void OnStreamReadImpl(ssize_t nread,
                               const uv_buf_t* bufs,
                               uv_handle_type pending,
                               void* ctx);

 protected:
  ssize_t OnMaxFrameSizePadding(size_t frameLength,
                                size_t maxPayloadLen);

  ssize_t OnCallbackPadding(size_t frame,
                            size_t maxPayloadLen);

  bool HasGetPaddingCallback() override {
    return padding_strategy_ == PADDING_STRATEGY_MAX ||
           padding_strategy_ == PADDING_STRATEGY_CALLBACK;
  }

  ssize_t GetPadding(size_t frameLength, size_t maxPayloadLen) override {
    if (padding_strategy_ == PADDING_STRATEGY_MAX) {
      return OnMaxFrameSizePadding(frameLength, maxPayloadLen);
    }

#if defined(DEBUG) && DEBUG
    CHECK_EQ(padding_strategy_, PADDING_STRATEGY_CALLBACK);
#endif

    return OnCallbackPadding(frameLength, maxPayloadLen);
  }

  void OnHeaders(
      Nghttp2Stream* stream,
      std::queue<nghttp2_header>* headers,
      nghttp2_headers_category cat,
      uint8_t flags) override;
  void OnStreamClose(int32_t id, uint32_t code) override;
  void Send(uv_buf_t* bufs, size_t total) override;
  void OnDataChunk(Nghttp2Stream* stream, uv_buf_t* chunk) override;
  void OnSettings(bool ack) override;
  void OnPriority(int32_t stream,
                  int32_t parent,
                  int32_t weight,
                  int8_t exclusive) override;
  void OnGoAway(int32_t lastStreamID,
                uint32_t errorCode,
                uint8_t* data,
                size_t length) override;
  void OnFrameError(int32_t id, uint8_t type, int error_code) override;
  void OnTrailers(Nghttp2Stream* stream,
                  const SubmitTrailers& submit_trailers) override;
  void AllocateSend(uv_buf_t* buf) override;

  int DoWrite(WriteWrap* w, uv_buf_t* bufs, size_t count,
              uv_stream_t* send_handle) override;

  AsyncWrap* GetAsyncWrap() override {
    return static_cast<AsyncWrap*>(this);
  }

  void* Cast() override {
    return reinterpret_cast<void*>(this);
  }

  // Required for StreamBase
  bool IsAlive() override {
    return true;
  }

  // Required for StreamBase
  bool IsClosing() override {
    return false;
  }

  // Required for StreamBase
  int ReadStart() override { return 0; }

  // Required for StreamBase
  int ReadStop() override { return 0; }

  // Required for StreamBase
  int DoShutdown(ShutdownWrap* req_wrap) override {
    return 0;
  }

  uv_loop_t* event_loop() const override {
    return env()->event_loop();
  }
 public:
  void Consume(Local<External> external);
  void Unconsume();

  static void New(const FunctionCallbackInfo<Value>& args);
  static void Consume(const FunctionCallbackInfo<Value>& args);
  static void Unconsume(const FunctionCallbackInfo<Value>& args);
  static void Destroying(const FunctionCallbackInfo<Value>& args);
  static void Destroy(const FunctionCallbackInfo<Value>& args);
  static void SubmitSettings(const FunctionCallbackInfo<Value>& args);
  static void SubmitRstStream(const FunctionCallbackInfo<Value>& args);
  static void SubmitResponse(const FunctionCallbackInfo<Value>& args);
  static void SubmitFile(const FunctionCallbackInfo<Value>& args);
  static void SubmitRequest(const FunctionCallbackInfo<Value>& args);
  static void SubmitPushPromise(const FunctionCallbackInfo<Value>& args);
  static void SubmitPriority(const FunctionCallbackInfo<Value>& args);
  static void SendHeaders(const FunctionCallbackInfo<Value>& args);
  static void ShutdownStream(const FunctionCallbackInfo<Value>& args);
  static void StreamWrite(const FunctionCallbackInfo<Value>& args);
  static void StreamReadStart(const FunctionCallbackInfo<Value>& args);
  static void StreamReadStop(const FunctionCallbackInfo<Value>& args);
  static void SetNextStreamID(const FunctionCallbackInfo<Value>& args);
  static void SendShutdownNotice(const FunctionCallbackInfo<Value>& args);
  static void SubmitGoaway(const FunctionCallbackInfo<Value>& args);
  static void DestroyStream(const FunctionCallbackInfo<Value>& args);
  static void FlushData(const FunctionCallbackInfo<Value>& args);
  static void UpdateChunksSent(const FunctionCallbackInfo<Value>& args);

  template <get_setting fn>
  static void GetSettings(const FunctionCallbackInfo<Value>& args);

  size_t self_size() const override {
    return sizeof(*this);
  }

  char* stream_alloc() {
    return stream_buf_;
  }

  void Close() override;

 private:
  StreamBase* stream_;
  StreamResource::Callback<StreamResource::AllocCb> prev_alloc_cb_;
  StreamResource::Callback<StreamResource::ReadCb> prev_read_cb_;
  padding_strategy_type padding_strategy_ = PADDING_STRATEGY_NONE;

  // use this to allow timeout tracking during long-lasting writes
  uint32_t chunks_sent_since_last_write_ = 0;
  uv_prepare_t* prep_ = nullptr;

  char stream_buf_[kAllocBufferSize];
};

class ExternalHeader :
    public String::ExternalOneByteStringResource {
 public:
  explicit ExternalHeader(nghttp2_rcbuf* buf)
     : buf_(buf), vec_(nghttp2_rcbuf_get_buf(buf)) {
  }

  ~ExternalHeader() override {
    nghttp2_rcbuf_decref(buf_);
    buf_ = nullptr;
  }

  const char* data() const override {
    return const_cast<const char*>(reinterpret_cast<char*>(vec_.base));
  }

  size_t length() const override {
    return vec_.len;
  }

  static inline
  MaybeLocal<String> GetInternalizedString(Environment* env,
                                           const nghttp2_vec& vec) {
    return String::NewFromOneByte(env->isolate(),
                                  vec.base,
                                  v8::NewStringType::kInternalized,
                                  vec.len);
  }

  template<bool may_internalize>
  static MaybeLocal<String> New(Environment* env, nghttp2_rcbuf* buf) {
    if (nghttp2_rcbuf_is_static(buf)) {
      auto& static_str_map = env->isolate_data()->http2_static_strs;
      v8::Eternal<v8::String>& eternal = static_str_map[buf];
      if (eternal.IsEmpty()) {
        Local<String> str =
            GetInternalizedString(env, nghttp2_rcbuf_get_buf(buf))
                .ToLocalChecked();
        eternal.Set(env->isolate(), str);
        return str;
      }
      return eternal.Get(env->isolate());
    }

    nghttp2_vec vec = nghttp2_rcbuf_get_buf(buf);
    if (vec.len == 0) {
      nghttp2_rcbuf_decref(buf);
      return String::Empty(env->isolate());
    }

    if (may_internalize && vec.len < 64) {
      // This is a short header name, so there is a good chance V8 already has
      // it internalized.
      return GetInternalizedString(env, vec);
    }

    ExternalHeader* h_str = new ExternalHeader(buf);
    MaybeLocal<String> str = String::NewExternalOneByte(env->isolate(), h_str);
    if (str.IsEmpty())
      delete h_str;

    return str;
  }

 private:
  nghttp2_rcbuf* buf_;
  nghttp2_vec vec_;
};

class Headers {
 public:
  Headers(Isolate* isolate, Local<Context> context, Local<Array> headers);
  ~Headers() {}

  nghttp2_nv* operator*() {
    return reinterpret_cast<nghttp2_nv*>(*buf_);
  }

  size_t length() const {
    return count_;
  }

 private:
  size_t count_;
  MaybeStackBuffer<char, 3000> buf_;
};

}  // namespace http2
}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#endif  // SRC_NODE_HTTP2_H_
