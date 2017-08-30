#ifndef SRC_NODE_REVERT_H_
#define SRC_NODE_REVERT_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "node.h"

/**
 * Note that it is expected for this list to vary across specific LTS and
 * Stable versions! Only CVE's whose fixes require *breaking* changes within
 * a given LTS or Stable may be added to this list, and only with TSC
 * consensus.
 *
 * For *master* this list should always be empty!
 **/
namespace node {

#define SECURITY_REVERSIONS(XX)
//  XX(CVE_2016_PEND, "CVE-2016-PEND", "Vulnerability Title")

enum reversion {
#define V(code, ...) SECURITY_REVERT_##code,
  SECURITY_REVERSIONS(V)
#undef V
};

extern unsigned int reverted;

inline const char* RevertMessage(const reversion cve) {
#define V(code, label, msg) case SECURITY_REVERT_##code: return label ": " msg;
  switch (cve) {
    SECURITY_REVERSIONS(V)
    default:
      return "Unknown";
  }
#undef V
}

inline void Revert(const reversion cve) {
  reverted |= 1 << cve;
  printf("SECURITY WARNING: Reverting %s\n", RevertMessage(cve));
}

inline void Revert(const char* cve) {
#define V(code, label, _)                                                     \
  if (strcmp(cve, label) == 0) return Revert(SECURITY_REVERT_##code);
  SECURITY_REVERSIONS(V)
#undef V
  printf("Error: Attempt to revert an unknown CVE [%s]\n", cve);
  exit(12);
}

inline bool IsReverted(const reversion cve) {
  return reverted & (1 << cve);
}

inline bool IsReverted(const char* cve) {
#define V(code, label, _)                                                     \
  if (strcmp(cve, label) == 0) return IsReverted(SECURITY_REVERT_##code);
  SECURITY_REVERSIONS(V)
  return false;
#undef V
}

}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#endif  // SRC_NODE_REVERT_H_
