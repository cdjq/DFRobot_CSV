#include "csv.h"
#include <WString.h>
#include <Stream.h>
#include <math.h>
#include <UD.h>
#include <variant.h>

#ifndef DFRobot_CSV_H
#define DFRobot_CSV_H
#define DEC 10
#define HEX 16
#define OCT 8
#ifdef BIN // Prevent warnings if BIN is previously defined in "iotnx4.h" or similar
#undef BIN
#endif
#define BIN 2

class DFRobot_CSV
{
public:
    DFRobot_CSV();
  //  template <class T>
    DFRobot_CSV(File &file);
    ~DFRobot_CSV();

    size_t write(uint8_t);
    size_t write(const char *str) {
      if (str == NULL) return 0;
      return write((const uint8_t *)str, strlen(str));
    }
    size_t write(const uint8_t *buf, size_t size);
    size_t write(const char *buffer, size_t size) {
      return write((const uint8_t *)buffer, size);
    }

//  size_t print(const __FlashStringHelper *);
    size_t print(const String &);
    size_t print(const char[]);
    size_t print(char);
    size_t print(unsigned char, int = DEC);
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
//    size_t print(const Printable&);
    size_t print(unsigned long, int = DEC);
    size_t print(double, int = 2);
  
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
	
	template <typename T1, typename T2>
    int count(T1 &row, T2 &field);
	template <typename T>
    String readRow(T row);
	template <typename T1, typename T2>
    String readItem(T1 row, T2 list);
//    operator bool();
private:
    typedef struct csv_parser sCSVParse_t;
    sCSVParse_t _p;
    File * _file; 
    size_t printNumber(unsigned long, uint8_t);
    size_t printFloat(double, uint8_t); 

};

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

template<class T1, class T2>
int DFRobot_CSV::count(T1 &row, T2 &field)
{
  struct counts c = {0};  
  uint8_t readBuf[512] = {0};
  uint16_t temp;
    // read from the file until there's nothing else in it:
 //   while (_file->available()) {
 //     Serial.write(_file->read());
 //   }
 //  _file->seek(0);
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
	String pt;
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

static void cbAftFieldCount(void *s, size_t i, void *data) {
    if(flags.row == (*(sRead_t *)data).row - 1) {
	    flags.list++;
		if(flags.list == (*(sRead_t *)data).list) {
		    char arr[i+1];
		    for(int j=0;j<i;j++)
			    arr[j] = *((char*)s+j);
		    arr[i] = 0;
		    String string(arr);
		    void *p = calloc(1,sizeof(sp_t));
		    ((sp_t *)p)->pt += string;
			((sRead_t *)data)->point = (sp_t *)p;
		}
    }
}

static void cbAftRowCount(int c, void *data)
{
	flags.row++;
}

template <typename T1, typename T2>
String DFRobot_CSV::readItem(T1 row, T2 list)
{
	sRead_t data = {0,row,list};  
	uint8_t readBuf[512] = {0};
	uint16_t temp;
	String str;
	memset(&flags,0,sizeof(flags));
	_file->seek(0);
    while((flags.row < row) && (temp =_file->read(readBuf, 512))) {
        if(csv_parse(&_p,readBuf,temp,cbAftFieldCount,cbAftRowCount,&data)!=temp) {
          _file->close();
		  return String();
		}
    }
    csv_fini(&_p,cbAftFieldCount,cbAftRowCount,&data);
	if(data.point) {
	    str = (*(data.point)).pt;
	    free((char *)(data.point->pt.c_str()));
	    free(data.point);
	    data.point = NULL;
	}
 //   csv_free(&_p);
	return str;
}

static void cbReadRow(void *s, size_t i, void *data)
{   
	if(flags.row == (*(sRead_t *)data).row-1) {
		char arr[i+1];
		for(int j=0;j<i;j++)
			arr[j] = *((char*)s+j);
		arr[i] = 0;
		String string(arr);
//		Serial.print("string");Serial.println(string);
		void *p = calloc(1,sizeof(sp_t));
		
		((sp_t *)p)->pt += string;
/*		((sp_t *)p)->pt.buffer = (char *)malloc(i+1);
		((sp_t *)p)->pt.capacity = i;
		if(i == 0) buffer[0] = 0;
		((sp_t *)p)->pt.len = i;
		strcpy(buffer, arr);*/

//		memcpy(p,&string,sizeof(string));
        if(((sRead_t *)data)->point) {
			sp_t *plast = ((sRead_t *)data)->point;
			while((*plast).ppt) {
				plast = (*plast).ppt;
			}
			(*plast).ppt = (sp_t *)p;
		}else {
			((sRead_t *)data)->point = (sp_t*)p;
		}
	}
}


template <typename T>
String DFRobot_CSV::readRow(T row)
{
	sp_t *plast, *pll;
	uint8_t readBuf[512] = {0};
	uint16_t temp;
	String str;
	sRead_t data = {0,row};
	memset(&flags,0,sizeof(flags));
	_file->seek(0);
	while((flags.row < row) && (temp = _file->read(readBuf, 512))) {
        if(csv_parse(&_p, readBuf, temp, cbReadRow, cbAftRowCount, &data) != temp) {
          _file->close();
		  return String();
		}
	}
	csv_fini(&_p, cbReadRow, cbAftRowCount, &data);
	if(data.point) {
		plast = data.point;
		do {
			str += (*plast).pt;
		    plast = (*plast).ppt;
			if(plast)
			    str += String(',');
		   }while(plast);	
	}
	while(data.point) {
		pll = plast = data.point;
		while((*plast).ppt) {
			pll = plast;
			plast = (*plast).ppt;
		}
		free((char *)((*plast).pt.c_str()));
		pll->ppt = NULL;
		free(plast);
		if(plast == data.point)
			data.point = NULL;
	}
//	csv_free(&_p);
	return str;
}
#endif