/* macro.h - header file for macro support for gas
   Copyright (C) 1994-2021 Free Software Foundation, Inc.

   Written by Steve and Judy Chamberlain of Cygnus Support,
      sac@cygnus.com

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#ifndef MACRO_H

#define MACRO_H

/* Structures used to store macros.

   Each macro knows its name and included text.  It gets built with a
   list of formal arguments, and also keeps a hash table which points
   into the list to speed up formal search.  Each formal knows its
   name and its default value.  Each time the macro is expanded, the
   formals get the actual values attached to them.  */

enum formal_type
  {
    FORMAL_OPTIONAL,
    FORMAL_REQUIRED,
    FORMAL_VARARG
  };

/* Describe the formal arguments to a macro.  */

typedef struct formal_struct {
  struct formal_struct *next;	/* Next formal in list.  */
  sb name;			/* Name of the formal.  */
  sb def;			/* The default value.  */
  sb actual;			/* The actual argument (changed on each expansion).  */
  int index;			/* The index of the formal 0..formal_count - 1.  */
  enum formal_type type;	/* The kind of the formal.  */
} formal_entry;

/* Other values found in the index field of a formal_entry.  */
#define QUAL_INDEX (-1)
#define NARG_INDEX (-2)
#define LOCAL_INDEX (-3)

/* Support unique identifiers to bind macroses each other.  */

typedef unsigned short int bindItemT;

typedef struct bindArrayS {
    bindItemT *array;   /* Allocated memory address (malloc).  */
    size_t    len;      /* Array item amount.  */
} bindArrayT;

/* Describe the macro.  */

typedef struct macro_struct
{
  sb sub;				/* Substitution text.  */
  int formal_count;			/* Number of formal args.  */
  formal_entry *formals;		/* Pointer to list of formal_structs.  */
  struct htab *formal_hash;		/* Hash table of formals.  */
  const char *name;			/* Macro name.  */
  const char *file;			/* File the macro was defined in.  */
  unsigned int line;			/* Line number of definition.  */
  bindItemT bind_nest;                  /* Interconnected macroses nest number.  */
  bindArrayT bind_counter_per_nest ;    /* Counter for each nest number.  */
} macro_entry;

/* Whether any macros have been defined.  */

extern int macro_defined;

/* The macro nesting level.  */

extern int macro_nest;

/* The macro hash table.  */

extern struct htab *macro_hash;

struct macro_hash_entry
{
  const char *name;
  macro_entry *macro;
};

typedef struct macro_hash_entry macro_hash_entry_t;

/* Hash function for a macro_hash_entry.  */

static inline hashval_t
hash_macro_entry (const void *e)
{
  const macro_hash_entry_t *entry = (const macro_hash_entry_t *) e;
  return htab_hash_string (entry->name);
}

/* Equality function for a macro_hash_entry.  */

static inline int
eq_macro_entry (const void *a, const void *b)
{
  const macro_hash_entry_t *ea = (const macro_hash_entry_t *) a;
  const macro_hash_entry_t *eb = (const macro_hash_entry_t *) b;

  return strcmp (ea->name, eb->name) == 0;
}

static inline macro_hash_entry_t *
macro_entry_alloc (const char *name, macro_entry *macro)
{
  macro_hash_entry_t *entry = XNEW (macro_hash_entry_t);
  entry->name = name;
  entry->macro = macro;
  return entry;
}

static inline macro_entry *
macro_entry_find (htab_t table, const char *name)
{
  macro_hash_entry_t needle = { name, NULL };
  macro_hash_entry_t *entry = htab_find (table, &needle);
  return entry != NULL ? entry->macro : NULL;
}

struct formal_hash_entry
{
  const char *name;
  formal_entry *formal;
};

typedef struct formal_hash_entry formal_hash_entry_t;

/* Hash function for a macro_hash_entry.  */

static inline hashval_t
hash_formal_entry (const void *e)
{
  const formal_hash_entry_t *entry = (const formal_hash_entry_t *) e;
  return htab_hash_string (entry->name);
}

/* Equality function for a formal_hash_entry.  */

static inline int
eq_formal_entry (const void *a, const void *b)
{
  const formal_hash_entry_t *ea = (const formal_hash_entry_t *) a;
  const formal_hash_entry_t *eb = (const formal_hash_entry_t *) b;

  return strcmp (ea->name, eb->name) == 0;
}

static inline formal_hash_entry_t *
formal_entry_alloc (const char *name, formal_entry *formal)
{
  formal_hash_entry_t *entry = XNEW (formal_hash_entry_t);
  entry->name = name;
  entry->formal = formal;
  return entry;
}

static inline formal_entry *
formal_entry_find (htab_t table, const char *name)
{
  formal_hash_entry_t needle = { name, NULL };
  formal_hash_entry_t *entry = htab_find (table, &needle);
  return entry != NULL ? entry->formal : NULL;
}

extern int buffer_and_nest (const char *, const char *, sb *,
			    size_t (*) (sb *));
extern void macro_init (int, int, int,
			size_t (*) (const char *, size_t, sb *, offsetT *));
extern void macro_set_alternate (int);
extern void macro_mri_mode (int);
extern const char *define_macro (size_t, sb *, sb *, size_t (*) (sb *),
				 const char *, unsigned int, const char **);
extern int check_macro (const char *, sb *, const char **, macro_entry **);
extern void delete_macro (const char *);
extern const char *expand_irp (int, size_t, sb *, sb *, size_t (*) (sb *));

#endif
