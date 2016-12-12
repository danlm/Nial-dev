#ifndef _NCELLS_H
#define _NCELLS_H 1

/**
 * ncells.h
 *
 * Provide a simple mechanism for mapping C pointers to Nial integers
 * with some cross checking of types and re-use. This is intended
 * to provide a single access point for this type of functionality
 * which is used in several Nial modules.
 */


typedef struct {
  void *cell_ptr;          /* Pointer to C structure */
  int  cell_type;          /* type of cell           */
  int  cell_iteration;     /* Current use indicator  */
} NCell, *NCellPtr;


/**
 * Pointer to table of ncells
 */
extern NCellPtr *ncell_list;


/**
 * The current number of ncells in use
 */ 
extern unsigned int num_ncells;


/**
 * Allocate a new cell reference and return its index and 
 * current iteration. The code will return -1 if there is 
 * no memory for allocating a cell.
 */
extern unsigned int new_ncell(void *c_ptr, int c_type, int *c_iter);

/* Free a cell */
extern int free_ncell(unsigned int c_idx, int c_type, int c_iter);

/**
 * Return the C pointer associated with a cell.
 * This will cross check type and iteration.
 */
inline void *get_ncell(unsigned int c_idx, int c_type, int c_iter) {
  if (0 <= c_idx && c_idx < num_cells && 
      ncell_list[c_idx].cell_type == c_type &&
      ncell_list[c_idx].cell_iteration == c_iter)
    return ncell_list[c_idx].cell_ptr;
  else
    return NULL;
}

  
#endif /* _NCELLS_H */
