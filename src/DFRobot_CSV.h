#include "csv.h"
#include "UD.h"

#ifndef DFRobot_CSV_H
#define DFRobot_CSV_H

class DFRobot_CSV
{
public:
  DFRobot_CSV();
  DFRobot_CSV(uint8_t option);
  ~DFRobot_CSV();
  int convert(const char *fileIn, const char *fileOut);
  int count(const char *fileName, void * field, void * row);
  int write(const char *fileName, const char *csv) 
  int read(const char *fileName, void *des)
  int readRow(const char *fileName, uint16_t row, void *des)

private:
  typedef struct csv_parser sCSVParse_t;
  sCSVParse_t _p;
};

extern DFRobot_CSV csv;

#endif