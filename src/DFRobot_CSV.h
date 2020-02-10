#include "csv.h"

#ifndef DFRobot_CSV_H
#define DFRobot_CSV_H

template <class T>
class DFRobot_CSV::public stream
{
public:
  DFRobot_CSV(T &file);
  ~DFRobot_CSV();

  int count(uint16_t &row, uint16_t &field);
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  String readRow(uint16_t row);
  String readItem(uint16_t row, uint16_t field);

private:
  typedef struct csv_parser sCSVParse_t;
  sCSVParse_t _p;
  T * _file;
  int csvFileWrite(T *fp, const void *src, size_t src_size, unsigned char quote)
};

#endif