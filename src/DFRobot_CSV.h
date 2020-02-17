#include "csv.h"

#ifndef DFRobot_CSV_H
#define DFRobot_CSV_H


class DFRobot_CSV
{
public:
    template<class T>
    DFRobot_CSV(T &file);
    ~DFRobot_CSV(); 

    int getWriteError() { return _writeError; }
    void clearWriteError() { setWriteError(0); }
    size_t write(uint8_t);
    size_t write(const char *str) {
      if (str == NULL) return 0;
      return write((const uint8_t *)str, strlen(str));
    }
    size_t write(const uint8_t *buf, size_t size);
    size_t write(const char *buffer, size_t size) {
      return write((const uint8_t *)buffer, size);
    }

//    size_t print(const __FlashStringHelper *);
    size_t print(const String &);
    size_t print(const char[]);
    size_t print(char);
    size_t print(unsigned char, int = DEC);
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    size_t print(double, int = 2);
    size_t print(const Printable&);

//  size_t println(const __FlashStringHelper *);
    size_t println(const String &s);
    size_t println(const char[]);
    size_t println(char);
    size_t println(unsigned char, int = DEC);
    size_t println(int, int = DEC);
    size_t println(unsigned int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    size_t println(double, int = 2);
//  size_t println(const Printable&);
    size_t println(void);
    int count(uint16_t &row, uint16_t &field);	
    String readRow(uint16_t row);
    String readItem(uint16_t row, uint16_t field);

protected:
    void setWriteError(int err = 1) { _writeError = err; }

private:
    typedef struct csv_parser sCSVParse_t;
    sCSVParse_t _p;
    Stream * _file;
  
    int _writeError;
    size_t printNumber(unsigned long, uint8_t);
    size_t printFloat(double, uint8_t);
  
    int csvFileWrite(T *fp, const void *src, size_t src_size, unsigned char quote);
};

#endif