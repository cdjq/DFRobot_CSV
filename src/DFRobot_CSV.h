#include "csv.h"
#include "Steam.h"

#ifndef DFRobot_CSV_H
#define DFRobot_CSV_H

class DFRobot_CSV::public Stream 
{
public:
  DFRobot_CSV();
  DFRobot_CSV(uint8_t option);
  ~DFRobot_CSV();
  int csvFileWrite(File *fp, const void *src, size_t src_size, unsigned char quote);


















private:
  static struct csv_parser _p;
};

typedef struct csv_parser sCsvPar_t;

sCsvPar_t DFRobot_CSV::_p = {
	.entry_buf = NULL;
    .pstate = ROW_NOT_BEGUN;
    .quoted = 0;
    .spaces = 0;
    .entry_pos = 0;
    .entry_size = 0;
    .status = 0;
    .options = options;
    .quote_char = CSV_QUOTE;
    .delim_char = CSV_COMMA;
    .is_space = NULL;
    .is_term = NULL;
    .blk_size = MEM_BLK_SIZE;
    .malloc_func = NULL;
    .realloc_func = realloc;
    .free_func = free;
};














#endif