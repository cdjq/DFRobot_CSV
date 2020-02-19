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
  uint16_t temp;
    // read from the file until there's nothing else in it:
    while (_file->available()) {
      Serial.write(_file->read());
    }
   _file->seek(0);
   csv_init(&_p,0);
   while((temp =_file->read(readBuf, 512)) > 0) {
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

typedef struct plist{
	string pt;
	struct plist *ppt;
} sp_t;

static struct {
  size_t list;
  size_t row;
} flags;

typedef struct{
	sp_t *point;
	size_t row;
	size_t list;
} sRead_t;

static void cbAfterFieldCount(void *s, size_t i, void *data) {
    if(flags.row == (*(sRead_t *)data).row - 1) {
	    flags.list++;
		if(flags.list == (*(sRead_t *)data).list - 1) {
		    char arr[i+1];
		    for(int j=0;j<i;j++)
			    char arr[j] = *((char*)s+j);
		    arr[i] = 0;
		    String string(arr);
		    void *p = calloc(1,sizeof(sp_t));
		    memcpy(*p,&string,sizeof(string));
			(*(sRead_t *)data).point = (sp_t *)p;
		}
    }
}

static void cbAftRowCount(int c, void *data)
{
	flags.row++;
}

template <typename T1, T2>
String DFRobot_CSV::readItem(T1 row, T2 list)
{
	sRead_t data = {0,row,list};  
	uint8_t readBuf[512] = {0};
	uint16_t temp;
	String str;
	memset(&flags,0,sizeof(flags));
	sRead_t data = {0,row,list};
    while((flags.row < row) && (temp =_file->read(readBuf, 512))) {
        if(csv_parse(&_p,readBuf,temp,cbAfterFieldCount,cbAfterRowCount,&data)!=temp) {
          _file->close();
		  return -1;
		}
    }
    csv_fini(&_p,cbAftFieldWriFile,cbAftRowWriFile,outFile);
	str = (*(*(sRead_t *)data).point).pt;
	free(data.point.pt.c_str());
	free(data.point);
	data.point = NULL;
    csv_free(&_p);
	return str;
}

static void cbReadRow(void *s, size_t i, void *data)
{   
	if(flags.row == (*(sRead_t *)data).row-1) {
		char arr[i+1];
		for(int j=0;j<i;j++)
			char arr[j] = *((char*)s+j);
		arr[i] = 0;
		String string(arr);
		void *p = calloc(1,sizeof(sp_t));
		memcpy(*p,&string,sizeof(string));
        if((sRead_t)data.point) {
			sp_t *plast = (sRead_t)data.point;
			while((*plast).ppt) {
				plast = (*plast).ppt;
			}
			(*plast).ppt = (sp_t *)p;
		}else {
			(sRead_t)data.point = (sp_t*)p;
		}
	}
}


template <typename T>
String DFRobot_CSV::readRow(T row)
{
	uint8_t readBuf[512] = {0};
	uint16_t temp;
	String str;
	sRead_t data = {0,row};
	memset(&flags,0,sizeof(flags));
	while((flags.row < row) && (temp = _file->read(readBuf, 512))) {
        if(csv_parse(&_p, readBuf, temp, cbReadRow, cbAftRowCount, &data) != temp) {
          _file->close();
		  return -1;
		}
	}
	csv_fini(&_p, cbAftFieldWriFile, cbAftRowWriFile, &data);
	if(data.point) {
		sp_t *plast = data.point;
		do {
			str += (*plast).pt;
		    plast = (*plast).ppt;
		   }while(plast);	
	}
	while(data.point) {
		sp_t *plast = data.point;
		while((*plast).ppt) {
			plast = (*plast).ppt;
		}
		free((*plast).pt.c_str());
		free(plast);
		plast = NULL;
	}
	csv_free(&_p);
	return str;
}




