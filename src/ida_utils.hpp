#ifndef IDADWARF_IDA_UTILS_HPP
#define IDADWARF_IDA_UTILS_HPP

// local definitions
#include "defs.hpp"

// IDA headers
#include <ida.hpp>
#include <nalt.hpp>
#include <typeinf.hpp>

// local headers
#include "gcc_defs.hpp"

// enable format string warnings
extern int msg(char const *format, ...) GCC_PRINTF(1, 2);
extern void warning(char const *message, ...) GCC_PRINTF(1, 2);
extern void error(char const *format, ...) GCC_PRINTF(1, 2);

#define MSG(fmt, ...) msg("[" PLUGIN_NAME "] " fmt, ## __VA_ARGS__)

#ifndef NDEBUG
# define DEBUG(fmt, ...) msg("[" PLUGIN_NAME " at %s (%s:%d)] " fmt, __FUNCTION__, __FILE__, __LINE__, ## __VA_ARGS__)
#else
# define DEBUG(...) do {} while(0)
#endif

#define WARNING(fmt, ...) warning("[" PLUGIN_NAME " at %s (%s:%d)] " fmt, __FUNCTION__, __FILE__, __LINE__, ## __VA_ARGS__)

#define ERROR(fmt, ...) error("[" PLUGIN_NAME " at %s (%s:%d)] " fmt, __FUNCTION__, __FILE__, __LINE__, ## __VA_ARGS__)

type_t const *get_ptrs_base_type(type_t const *type);

void append_ordinal_name(qtype &type, ulong const ordinal);

void append_complex_type(qtype &new_type, qtype const *complex_type);

void append_complex_type(qtype &new_type, ulong const ordinal);

void make_new_type(qtype &new_type, type_t const *type, ulong const ordinal);

bool find_simple_type(char const *name, qtype const &ida_type, ulong *ordinal, bool *found);

bool set_simple_die_type(char const *name, qtype const &ida_type, ulong *ordinal);

flags_t fill_typeinfo(typeinfo_t *mt, ulong const ordinal, type_t const **type);

bool replace_func_return(qtype &new_type, qtype const &return_type, type_t const *func_type);

bool apply_type_ordinal(ea_t const addr, ulong const ordinal);

ulong get_typedef_ordinal(type_t const *typedef_type);

char const *get_typedef_name(type_t const *typedef_type);

ulong resolve_typedef_ordinal(type_t const *typedef_type);

ulong resolve_typedef_ordinal(ulong const typedef_ordinal);

char const *resolve_typedef_name(type_t const *typedef_type);

#endif // IDADWARF_IDA_UTILS_HPP
