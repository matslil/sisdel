#ifndef HAVE_LTTNG
#define tracepoint(...)
#else
#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER tp_sisdel

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE sisdel_tracepoints.h

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */


#if !defined(_SISDEL_TRACEPOINTS_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _SISDEL_TRACEPOINTS_H

#include <lttng/tracepoint.h>
#include <stdint.h>

/*
 * Sbucket tracepoints
 */

TRACEPOINT_EVENT (
	tp_sisdel,
	tp_sbucket_init,
	TP_ARGS(const void *, handle),
	TP_FIELDS(ctf_integer_hex(uintptr_t, handle, (uintptr_t)handle))
)

TRACEPOINT_EVENT (
	tp_sisdel,
	tp_sbucket_destroy,
	TP_ARGS(const void *, handle),
	TP_FIELDS(ctf_integer_hex(uintptr_t, handle, (uintptr_t)handle))
)

TRACEPOINT_EVENT (
	tp_sisdel,
	tp_sbucket_find,
	TP_ARGS(
		uint32_t, hash,
		const char *, str, size_t, str_len,
		size_t, key
	),
	TP_FIELDS(
		ctf_integer_hex(uint32_t, hash, hash)
		ctf_sequence_text(char, str, str, size_t, str_len)
		ctf_integer_hex(size_t, key, key)
	)
)

TRACEPOINT_EVENT (
	tp_sisdel,
	tp_sbucket_index,
	TP_ARGS(
		size_t, key,
		const char *, str
	),
	TP_FIELDS(
		ctf_integer_hex(size_t, key, key)
		ctf_string(str, str)
	)
)

TRACEPOINT_EVENT (
	tp_sisdel,
	tp_sbucket_add,
	TP_ARGS(
		uint32_t, hash,
		const char *, str, size_t, str_len,
		size_t, key
	),
	TP_FIELDS(
		ctf_integer_hex(uint32_t, hash, hash)
		ctf_sequence_text(char, str, str, size_t, str_len)
		ctf_integer_hex(size_t, key, key)
	)
)

TRACEPOINT_EVENT (
	tp_sisdel,
	tp_sbucket_idx_expand,
	TP_ARGS(size_t, new_size_entries),
	TP_FIELDS(ctf_integer_hex(size_t, new_size_entries, new_size_entries))
)

TRACEPOINT_EVENT (
	tp_sisdel,
	tp_sbucket_string_expand,
	TP_ARGS(size_t, new_size_bytes),
	TP_FIELDS(ctf_integer_hex(size_t, new_size_bytes, new_size_bytes))
)

/*
 * Status tracepoints
 */

TRACEPOINT_EVENT (
	tp_sisdel,
	tp_status_out_of_memory,
	TP_ARGS(size_t, alloc_size),
	TP_FIELDS(ctf_integer(size_t, alloc_size, alloc_size))
)

#endif /* _SISDEL_TRACEPOINTS_H */

#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HAVE_LTTNG */
