#ifndef IDADWARF_DIE_UTILS_HPP
#define IDADWARF_DIE_UTILS_HPP

#include <exception>
#include <map>
#include <memory>
#include <sstream>

// additional libs headers
#include <dwarf.h>
#include <libdwarf.h>

// local headers
#include "die_cache.hpp"

using namespace std;

#define CHECK_DWERR2(cond, err, fmt, ...) if(cond) { throw DieException(__FILE__, __LINE__, err, fmt, ## __VA_ARGS__); }
#define CHECK_DWERR(cond, err, fmt, ...) CHECK_DWERR2((cond) != DW_DLV_OK, err, fmt, ## __VA_ARGS__)
#define THROW_DWERR(fmt, ...) throw DieException(__FILE__, __LINE__, NULL, fmt, ## __VA_ARGS__);

int get_small_encoding_value(Dwarf_Attribute attrib, Dwarf_Signed *val, Dwarf_Error *err);

class DieException : public exception
{
public:
  DieException(char const *file, int const line, Dwarf_Error err, char const *fmt, ...) throw()
    : m_err(err)
  {
    ostringstream oss;
    va_list ap;
    char buffer[MAXSTR];

    va_start(ap, fmt);
    (void)qvsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

    oss << '(' << file << ':' << line << ") " <<
      buffer << " (" << dwarf_errno(err) << ": " << dwarf_errmsg(err) << ')';
    m_msg = oss.str();
  }

  virtual ~DieException(void) throw()
  {

  }

  virtual char const *what(void) const throw()
  {
    return m_msg.c_str();
  }

  Dwarf_Error get_error(void) const throw()
  {
    return m_err;
  }

private:
  Dwarf_Error m_err;
  string m_msg;
};

// RAII-powered DIE holder to avoid dwarf_dealloc nightmare
class DieHolder
{
public:
  DieHolder(Dwarf_Debug dbg, Dwarf_Die die, bool const dealloc_die=true) throw();

  DieHolder(Dwarf_Debug dbg, Dwarf_Off offset, bool const dealloc_die=true);

  ~DieHolder(void) throw();

  // operators

  bool operator==(DieHolder const &other)
  {
    return (m_dbg == other.m_dbg && m_die == other.m_die);
  }

  // common getters

  Dwarf_Die get_die(void) const throw()
  {
    return m_die;
  }

  Dwarf_Debug get_dbg(void) const throw()
  {
    return m_dbg;
  }

  // warning: this is the real DIE name, not the one in idati!
  // these 2 names might be different if there was a conflict
  char const *get_name(void);

  Dwarf_Attribute get_attr(int attr);

  Dwarf_Off get_ref_from_attr(int attr);

  Dwarf_Unsigned get_member_offset(void);

  Dwarf_Signed get_attr_small_val(int attr);

  Dwarf_Unsigned get_bytesize(void);

  Dwarf_Off get_offset(void);

  Dwarf_Off get_CU_offset_range(Dwarf_Off *cu_length);

  Dwarf_Half get_tag(void);

  Dwarf_Die get_child(void);

  Dwarf_Die get_sibling(void);

  // DieCache wrappers

  bool in_cache();

  bool get_cache(die_cache *cache);

  bool get_cache_type(die_cache *cache);

  void cache_useless(void);

  void cache_type(ulong const ordinal, bool second_pass=false, ulong base_ordinal=0);

  bool get_ordinal(ulong *ordinal);

  typedef auto_ptr<DieHolder> Ptr;

private:
  Dwarf_Debug m_dbg;
  Dwarf_Die m_die;
  Dwarf_Off m_offset;
  char *m_name;
  typedef map<int, Dwarf_Attribute> MapAttrs;
  MapAttrs m_attrs;
  bool m_offset_used;
  bool m_dealloc_die;

  // no copying or assignment
  DieHolder(DieHolder const &);
  DieHolder &operator=(DieHolder const &);

  // common member vars init for the constructors
  void init(Dwarf_Debug dbg, Dwarf_Die die, bool const dealloc_die);
};

// compilation unit DIEs are kept in this object
// to only have to retrieve them one time
class CUsHolder : public qvector<Dwarf_Die>
{
public:
  CUsHolder(Dwarf_Debug dbg)
    : m_dbg(dbg)
  {

  }

  virtual ~CUsHolder(void) throw()
  {
    for(size_t idx = 0; idx < size(); ++idx)
    {
      dwarf_dealloc(m_dbg, (*this)[idx], DW_DLA_DIE);
      (*this)[idx] = NULL;
    }

    clear();
  }

private:
  Dwarf_Debug m_dbg;

  // no copying or assignment
  CUsHolder(CUsHolder const &);
  CUsHolder &operator=(CUsHolder const &);
};

// DIE visiting function
// warning: should not throw any exception!
typedef void(*die_visitor_fun)(DieHolder &die);

void do_dies_traversal(Dwarf_Debug dbg, CUsHolder const &cus_holder,
                       die_visitor_fun visit);

#endif // IDADWARF_DIE_UTILS_HPP