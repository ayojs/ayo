#ifndef SRC_NODE_CONTEXTIFY_H_
#define SRC_NODE_CONTEXTIFY_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "v8.h"

namespace node {

class Environment;

v8::MaybeLocal<v8::Context> ContextFromContextifiedSandbox(
    Environment* env,
    v8::Local<v8::Object> sandbox);

}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS


#endif  // SRC_NODE_CONTEXTIFY_H_
