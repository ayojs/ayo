#ifndef SRC_NODE_MESSAGING_H_
#define SRC_NODE_MESSAGING_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "env.h"
#include "node_mutex.h"
#include <list>
#include <memory>

namespace node {
namespace worker {

class ReferenceCountedSAB;
class MessagePortData;
class MessagePort;

// The default message type: No special handling, this is a userland message.
#define MESSAGE_FLAG_NONE 0

// Generated in C++, handled in C++
#define MESSAGE_FLAG_STOP_THREAD_ORDER 1
#define MESSAGE_FLAG_THREAD_STOPPED 2
#define MESSAGE_FLAG_MAX_HANDLED_INTERNALLY 2

// Generic offset for use by the JS core modules
#define MESSAGE_FLAG_CUSTOM_OFFSET 100

// Any further flagged message codes are defined by the modules that use them.

// This is an object associated with a SharedArrayBuffer, which keeps track
// of a cross-thread reference count. Once a SharedArrayBuffer is transferred
// for the first time (or is attempted to be transferred), one of these objects
// is created, and the SharedArrayBuffer is moved from internalized mode into
// externalized mode (i.e. the JS engine no longer frees the memory on its own).
//
// This will always be referred to using a std::shared_ptr, since it keeps
// a reference count and is guaranteed to be thread-safe.
typedef std::shared_ptr<ReferenceCountedSAB> ExternalSABReference;
class ReferenceCountedSAB
    : public std::enable_shared_from_this<ReferenceCountedSAB> {
 public:
  static ExternalSABReference ForIncomingSharedArrayBuffer(
      Environment* env, v8::Local<v8::Context> context,
      v8::Local<v8::SharedArrayBuffer> source);
  ~ReferenceCountedSAB();

  // Create a SharedArrayBuffer object for a specific Environment and Context.
  // The created SharedArrayBuffer will be in externalized mode and has
  // a hidden object attached to it, during whose lifetime the reference
  // count if increased by 1.
  v8::MaybeLocal<v8::SharedArrayBuffer> GetSharedArrayBuffer(
      Environment* env, v8::Local<v8::Context> context);

  ReferenceCountedSAB(ReferenceCountedSAB&& other) = delete;
  ReferenceCountedSAB& operator=(ReferenceCountedSAB&& other) = delete;
  ReferenceCountedSAB& operator=(const ReferenceCountedSAB&) = delete;
  ReferenceCountedSAB(const ReferenceCountedSAB&) = delete;

 private:
  explicit ReferenceCountedSAB(void* data, size_t size);

  // Attach a lifetime tracker object with a reference count to `target`.
  v8::Maybe<bool> AssignToSharedArrayBuffer(
      Environment* env,
      v8::Local<v8::Context> context,
      v8::Local<v8::SharedArrayBuffer> target);

  void* data = nullptr;
  size_t size = 0;
};


// Represents a single communication message. The only non-standard extension
// here is passing of a separate flag that the Workers implementation uses
// for internal cross-thread information passing.
class Message {
 public:
  explicit Message(int32_t flag = MESSAGE_FLAG_NONE);
  Message(void* buf, size_t length, int32_t flag);
  ~Message();

  Message(Message&& other);
  Message& operator=(Message&& other);
  Message& operator=(const Message&) = delete;
  Message(const Message&) = delete;

  // Deserialize the contained JS value. May onle be called once, and only
  // after Serialize() has been called (e.g. by another thread).
  v8::MaybeLocal<v8::Value> Deserialize(Environment* env,
                                        v8::Local<v8::Context> context);
  // Serialize a JS value, and optionally transfer objects, into this message.
  // The Message object retains ownership of all transferred objects until
  // deserialization.
  v8::Maybe<bool> Serialize(Environment* env,
                            v8::Local<v8::Context> context,
                            v8::Local<v8::Value> input,
                            v8::Local<v8::Value> transfer_list);

  // Internal method of Message that is called when a new SharedArrayBuffer
  // object is encountered in the incoming value's structure.
  void AddSharedArrayBuffer(ExternalSABReference ref);
  // Internal method of Message that is called once serialization finishes
  // and that transfers ownership of `data` to this message.
  void AddMessagePort(std::unique_ptr<MessagePortData>&& data);

 private:
  int32_t flag_ = MESSAGE_FLAG_NONE;
  uv_buf_t main_message_buf_;
  std::vector<uv_buf_t> array_buffer_contents_;
  std::vector<ExternalSABReference> shared_array_buffers_;
  std::vector<std::unique_ptr<MessagePortData>> message_ports_;

  friend class MessagePort;
};

// This contains all data for a `MessagePort` instance that is not tied to
// a specific Environment/Isolate/event loop, for easier transfer.
class MessagePortData {
 public:
  explicit MessagePortData(MessagePort* owner);
  ~MessagePortData();

  MessagePortData(MessagePortData&& other) = delete;
  MessagePortData& operator=(MessagePortData&& other) = delete;
  MessagePortData(const MessagePortData& other) = delete;
  MessagePortData& operator=(const MessagePortData& other) = delete;

  // Add a message to the incoming queue and notify the receiver.
  // This may be called from any thread.
  void AddToQueue(Message&& message);

  // Returns true if and only this MessagePort is currently not entangled
  // with another message port.
  bool IsSiblingClosed() const;

  // Turns `a` and `b` into siblings, i.e. connects the sending side of one
  // to the receiving side of the other.
  static void Entangle(MessagePortData* a, MessagePortData* b);
  // Removes any possible sibling.
  void Disentangle();

 private:
  // After disentangling this message port, the owner handle (if any)
  // is asynchronously triggered.
  void PingOwnerAfterDisentanglement();

  // This mutex protects all fields below it, with the exception of
  // sibling_.
  mutable Mutex mutex_;
  bool started_ = false;
  std::list<Message> incoming_messages_;
  MessagePort* owner_ = nullptr;
  // This mutex protects the sibling_ field and is shared between two entangled
  // MessagePorts. If both mutexes are acquired, this one needs to be
  // acquired first.
  std::shared_ptr<Mutex> sibling_mutex_ = std::make_shared<Mutex>();
  MessagePortData* sibling_ = nullptr;

  friend class MessagePort;
};

// A message port that receives messages from other threads.
class MessagePort : public HandleWrap {
 public:
  // Listener interface that is used for C++-to-C++ status indication.
  class FlaggedMessageListener {
   public:
    virtual ~FlaggedMessageListener() {}
    virtual void HandleMessage(int32_t flag) = 0;
  };

  MessagePort(Environment* env,
              v8::Local<v8::Context> context,
              v8::Local<v8::Object> wrap,
              std::unique_ptr<FlaggedMessageListener>&& listener);
  ~MessagePort();

  static MessagePort* New(Environment* env,
                          v8::Local<v8::Context> context,
                          std::unique_ptr<FlaggedMessageListener> listener
                              = nullptr,
                          std::unique_ptr<MessagePortData> data = nullptr);

  // Send a message, i.e. deliver it into the sibling's incoming queue.
  // If there is no sibling, i.e. this port is closed,
  // this message is silently discarded.
  void Send(Message&& message);
  void Send(const v8::FunctionCallbackInfo<v8::Value>& args);
  // Deliver a single message into this port's incoming queue.
  void AddToQueue(Message&& message);

  // Start processing messages on this port as a receiving end.
  void Start();

  /* constructor */
  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  /* prototype methods */
  static void PostMessage(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void StartBinding(const v8::FunctionCallbackInfo<v8::Value>& args);

  /* static */
  static void MoveToContext(const v8::FunctionCallbackInfo<v8::Value>& args);

  static void Entangle(MessagePort* a, MessagePort* b);
  static void Entangle(MessagePort* a, MessagePortData* b);

  // Detach this port's data for transferring. After this, the MessagePortData
  // is no longer associated with this handle, although it can still receive
  // messages.
  std::unique_ptr<MessagePortData> Detach();

  // Returns true if `Detach()` may be called on this port. This is currently
  // true whenever there is no listener for flagged messages.
  bool IsTransferable() const;
  bool IsSiblingClosed() const;

  // Mark this MessagePort as a privileged port, i.e. one that doesn't discard
  // flags set on incoming messages.
  void MarkAsPrivileged();

  // Allow a half-open state.
  void DoNotCloseWhenSiblingCloses();

  size_t self_size() const override;

 private:
  void OnClose() override;
  void OnMessage();
  void TriggerAsync();
  inline uv_async_t* async();

  std::unique_ptr<MessagePortData> data_ = nullptr;
  std::unique_ptr<FlaggedMessageListener> fm_listener_ = nullptr;
  bool is_privileged_ = false;
  bool close_when_sibling_closes_ = true;

  friend class MessagePortData;
};

v8::MaybeLocal<v8::Function> GetMessagePortConstructor(
    Environment* env, v8::Local<v8::Context> context);

}  // namespace worker
}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS


#endif  // SRC_NODE_MESSAGING_H_
