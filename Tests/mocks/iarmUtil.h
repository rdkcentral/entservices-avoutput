#pragma once

// --------------------------------------------------------------------
// Minimal mock of iarmUtil.h to allow CI/native Linux builds to succeed.
// Real hardware builds must compile with USE_IARM=ON to use the actual
// RDK IARM utilities.
// --------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

static inline int IARM_Bus_Init(const char*) { return 0; }
static inline int IARM_Bus_Term() { return 0; }
static inline int IARM_Bus_Connect() { return 0; }
static inline int IARM_Bus_Disconnect() { return 0; }
static inline int IARM_Bus_Call(const char*, void*) { return 0; }

#ifdef __cplusplus
}
#endif
