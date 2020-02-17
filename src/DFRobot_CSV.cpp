#include <DFRobot_CSV.h>

template <class T>
DFRobot_CSV::DFRobot_CSV(T &file)
{
	csv_init(&_p,0);
	_file = &file;
	_writeError = 0;
}

DFRobot_CSV::~DFRobot_CSV()
{
	if(_p->entry_buf)
		_p->free_func(_p->entry_buf);
}


int DFRobot_CSV::csvFileWrite(T *fp, const void *src, size_t src_size, unsigned char quote)
{
  const unsigned char *csrc = (const unsigned char *)src;

  if (fp == NULL || src == NULL)
    return 0;

  if (fp->write(quote) == false)
    return -1;

  while (src_size) {
    if (*csrc == quote) {
      if (fp->write(quote) == false)
        return -1;
    }
    if (fp->write(*csrc) == false)
      return -1;
    src_size--;
    csrc++;
  }

  if (fp->write(quote) == false) {
    return -1;
  }

  return 0;
}

static void cbWrite(void *s, size_t i, void *) 
{
  csvFileWrite(_file, s, i, CSV_QUOTE);
  _file->write(",");
}

static void cbWriteLine (int c, void *) 
{
  _file->seek(_file->position()-1);
  _file->write("\n");
}


size_t DFRobot_CSV::write(uint8_t val)
{
  return write(&val, 1);
}

size_t DFRobot_CSV::write(const uint8_t *buf, size_t size)
{
  return _file->write(buf,size);
}

size_t DFRobot_CSV::print(const String &s)
{
  size_t t = write(s.c_str(), s.length());
  t += write(",");
  return t;
}

size_t DFRobot_CSV::print(const char str[])
{
  size_t t = write(str);
  t += write(",");
  return t;
}

size_t DFRobot_CSV::print(char c)
{
  size_t t = write(c);
  t+= write(",");
  return t;
}

size_t DFRobot_CSV::print(unsigned char b, int base)
{
  return print((unsigned long) b, base);
}

size_t DFRobot_CSV::print(int n, int base)
{
  return print((long) n, base);
}

size_t DFRobot_CSV::print(unsigned int n, int base)
{
  return print((unsigned long) n, base);
}

size_t DFRobot_CSV::print(long n, int base)
{
  size_t tem;
  if (base == 0) {
    tem = write(n);
	tem += write(",");
	return tem;
  } else if (base == 10) {
    if (n < 0) {
      int t = print('-');
      n = -n;
      tem = printNumber(n, 10) + t;
	  tem += write(",");
	  return tem;
    }
    tem = printNumber(n, 10);
	tem += write(",");
	return tem;
  } else {
    tem = printNumber(n, base);
	tem += write(",");
	return tem;
  }
}

size_t DFRobot_CSV::print(unsigned long n, int base)
{
  size_t t;
  if (base == 0) {t = write(n);t += write(",");return t;}
  else {t =  printNumber(n, base);t += write(",");return t;}
}

size_t DFRobot_CSV::print(double n, int digits)
{
  size_t t;
  t = printFloat(n, digits);
  t += write(",");
  return t;
}

size_t DFRobot_CSV::println(void)
{
  return write("\r\n");
}

size_t DFRobot_CSV::println(const String &s)
{
  size_t n = write(s.c_str(), s.length());
  n += println();
  return n;
}

size_t DFRobot_CSV::println(const char c[])
{
  size_t n = write(c);
  n += println();
  return n;
}

size_t DFRobot_CSV::println(char c)
{
  size_t n = write(c);
  n += println();
  return n;
}

size_t DFRobot_CSV::println(unsigned char b, int base)
{
  return println((unsigned long) b, base);
}

size_t DFRobot_CSV::println(int num, int base)
{
  return println((long) num, base);
}

size_t DFRobot_CSV::println(unsigned int num, int base)
{
  return println((unsigned long) num, base);
}

size_t DFRobot_CSV::println(long num, int base)
{
  size_t tem;
  if (base == 0) {
    tem = write(n);
	tem += write("\r\n");
	return tem;
  } else if (base == 10) {
    if (n < 0) {
      int t = print('-');
      n = -n;
      tem = printNumber(n, 10) + t;
	  tem += write("\r\n");
	  return tem;
    }
    tem = printNumber(n, 10);
	tem += write("\r\n");
	return tem;
  } else {
    tem = printNumber(n, base);
	tem += write("\r\n");
	return tem;
  }
}

size_t DFRobot_CSV::println(unsigned long num, int base)
{
  size_t t;
  if (base == 0) {t = write(n);t += write("\r\n");return t;}
  else {t =  printNumber(n, base);t += write("\r\n");return t;}
}

size_t DFRobot_CSV::println(double num, int digits)
{
  size_t n = print(num, digits);
  n += println();
  return n;
}


// Private Methods /////////////////////////////////////////////////////////////

size_t DFRobot_CSV::printNumber(unsigned long n, uint8_t base)
{
  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *str = &buf[sizeof(buf) - 1];

  *str = '\0';

  // prevent crash if called with base == 1
  if (base < 2) base = 10;

  do {
    char c = n % base;
    n /= base;

    *--str = c < 10 ? c + '0' : c + 'A' - 10;
  } while(n);

  return write(str);
}

size_t DFRobot_CSV::printFloat(double number, uint8_t digits) 
{ 
  size_t n = 0;
  
  if (isnan(number)) return print("nan");
  if (isinf(number)) return print("inf");
  if (number > 4294967040.0) return print ("ovf");  // constant determined empirically
  if (number <-4294967040.0) return print ("ovf");  // constant determined empirically
  
  // Handle negative numbers
  if (number < 0.0)
  {
     n += write('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  n += printNumber(int_part,DEC);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0) {
    n += write('.'); 
  }

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    unsigned int toPrint = (unsigned int)(remainder);
    n += printNumber(n, DEC);
    remainder -= toPrint; 
  } 
  
  return n;
}

struct counts {
size_t fields;
size_t rows;
};

static void cbAftFieldCountField (void *s, size_t len, void *data) 
{
((struct counts *)data)->fields++; 
}
static void cbAftRowCountRow (int c, void *data) 
{
((struct counts *)data)->rows++; 
}

template<class T1, T2>
int DFRobot_CSV::count(T1 &row, T2 &field)
{
  struct counts c = {0};  
  uint8_t readBuf[512] = {0};
    // read from the file until there's nothing else in it:
    while (_file->available()) {
      Serial.write(_file->read());
    }
   _file->seek(0);
   csv_init(&_p,0);
   while(_file->available()) {
      uint16_t temp;
      temp = _file->read(readBuf,512);
      for(uint8_t i=0;i<20;i++)
      Serial.write(readBuf[i]);Serial.println();
      if(csv_parse(&_p,readBuf,temp,cbAftFieldCountField,cbAftRowCountRow,&c)!=temp) {
        _file->close();
		return -1;
	  }
    }
    csv_fini(&_p,cbAftFieldCountField,cbAftRowCountRow,&c);     
    // close the file:
 //   Serial.print("fields: ");Serial.println(c.fields);
 //   Serial.print("rows: ");Serial.println(c.rows);
    field = c.fields/c.rows;
	row = c.rows;
//    csv_free(&_p);
//    Serial.println("okkkkkkkkkkkkkkkk");
	return 0;
}


static size_t pos;

static void cbReadAfterField(void *s, size_t i, void *des) {
  size_t j;
  for(j=0;j<i;j++)
	  *((char *)des + pos +j) = *((char *)s+j);
  pos += j;
  *((char *)des + pos) = CSV_COMMA;
  pos ++;
}

static void cbReadAfterRow(int c, void *des) {
  pos--;
  *((char *)des + pos) = CSV_LF;
    if (event_ptr->event_type != CSV_COL)
    fail_parser(test_name, "didn't expect a column");

  /* Check the actual size against the expected size */
  if (event_ptr->size != len)
    fail_parser(test_name, "actual data length doesn't match expected data length");

  /* Check the actual data against the expected data */
  if ((event_ptr->data == NULL || data == NULL)) {
    if (event_ptr->data != data)
      fail_parser(test_name, "actual data doesn't match expected data");
  }

}

template <typename T1, T2>
String DFRobot_CSV::readItem(T1 row, T2 field)
{
	String des;
	pos = 0;
    void  *outFile;
    uint32_t pt = 0;
	uint8_t readBuf[512] = {0};
    while(_file->available()) {
		uint16_t temp;
        temp = _file->read(readBuf,512);
        if(csv_parse(&_p,readBuf,temp,cbReadAfterField,cbReadAfterRow,des)!=temp) {
          _file->close();
		  return -1;
		}
    }
    csv_fini(&_p,cbAftFieldWriFile,cbAftRowWriFile,outFile);   
    csv_free(&_p);
	return des;
}

static struct {
  bool flag;
  size_t positon;
  size_t row;
  bool end;
} countForReadRow;

static struct readPa{
	void *des;
	size_t row;
} readPar;

static void cbReadRow(void *s, size_t i, void *readPar)
{
	if(flag || (readPa)readPar.row == 1) {
		for(uint8_t j=0;j<i;j++)
			*((uint8_t *)((readPa)readPar.des) + countForReadRow.positon + j) = *((uint8_t *)s + j);
		countForReadRow.positon += i;
	}
}

static void cbAftRowCount(int c, void *readPar)
{
	countForReadRow.row++;
	if (countForReadRow.row == (readPa)readPar.row - 1) countForReadRow.flag = 1;
	else if (countForReadRow.row == (readPa)readPar.row) {
		countForReadRow.flag = 0;countForReadRow.end = 1;
	}
}

template <typename T>
String DFRobot_CSV::readRow(T row)
{
    void  *outFile;
    uint32_t pt = 0;
	uint8_t readBuf[512] = {0};
	memset(&countForReadRow,0,sizeof(countForReadRow));
	readPar.des = des;
	readPar.row = row;
	while(!countForReadRow.end && _file->available()) {
		uint16_t tem;
        tem = _file->read(readBuf,512);
        if(csv_parse(&_p,readBuf,temp,cbReadRow,cbAftRowCount,String des)!=temp) {
          _file->close();
		  return -1;
		}
	}
	csv_fini(&_p,cbAftFieldWriFile,cbAftRowWriFile,outFile);   
	csv_free(&_p);
	return des;
}




