#include <DFRobot_CSV.h>

template <class T>
DFRobot_CSV<T>::DFRobot_CSV(T &file)
{
	csv_init(&_p,0);
	_file = &file;
}

template <class T>
DFRobot_CSV<T>::~DFRobot_CSV()
{
	if(_p->entry_buf)
		_p->free_func(_p->entry_buf);
}

template <class T>
int DFRobot_CSV<T>::csvFileWrite(T *fp, const void *src, size_t src_size, unsigned char quote)
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

template <class T>
size_t DFRobot_CSV<T>::write(uint8_t val)
{
  return write(&val, 1);
}

template <class T>
size_t DFRobot_CSV<T>::write(const uint8_t *buf, size_t size)
{
  size_t t;
  csv_parse(&_p,buf,size,cbWrite,cbWriteLine,NULL);
  return t;
}

struct counts {
long unsigned fields;
uint rows;
};

static void cbAftFieldCountField (void *s, size_t len, void *data) 
{
((struct counts *)data)->fields++; 
}
static void cbAftRowCountRow (int c, void *data) 
{
((struct counts *)data)->rows++; 
}

template <class T>
int DFRobot_CSV<T>::count(uint16_t &row, uint16_t &field)
{
  struct counts c = {0};
  uint32_t size;   
  uint8_t readBuf[512] = {0};
    // read from the file until there's nothing else in it:
    while (_file->available()) {
      Serial.write(_file->read());
    }
   _file->seek(0);
   size = _file->size();
//   Serial.print("a3.txt: Size="); Serial.println(size);
   csv_init(&_p,0);
   while(size) {
      uint16_t temp;
      if(size>512) temp = 512;
      else temp = size;
      _file->read(readBuf,temp);
      for(uint8_t i=0;i<20;i++)
      Serial.write(readBuf[i]);Serial.println();
      if(csv_parse(&_p,readBuf,temp,cbAftFieldCountField,cbAftRowCountRow,&c)!=temp) {
        _file->close();
		return -1;
	  }
      size -= temp;
    }
    csv_fini(&_p,cbAftFieldCountField,cbAftRowCountRow,&c);     
    // close the file:
 //   Serial.print("fields: ");Serial.println(c.fields);
 //   Serial.print("rows: ");Serial.println(c.rows);
    field = c.fields;
	row = c.rows;
//    csv_free(&_p);
//    Serial.println("okkkkkkkkkkkkkkkk");
	return 0;
}


static void cbWriteField (void *s, size_t i, void *outFile) {
  csv_fwrite((File *)outFile, s, i);
  ((File *)outFile)->write(",");
}

static void writeLine (int c, void *outFile) {
  ((File *)outFile)->seek(((File *)outFile)->position()-1);
  ((File *)outFile)->write("\n");
}

template <class T>
int DFRobot_CSV<T>::writeCSV(const char *fileName, const char *csv) 
{
    size_t i;
    uint16_t size;
    void  *outFile;
	File myFile;
    uint16_t pt = 0;
    SD.begin(32);
    myFile = SD.open(fileName,O_CREAT|O_WRITE|O_APPEND);
    outFile = (void *)&myFile;
    size = strlen(csv);
    if(csv_parse(&_p,csv,size,cbWriteField,cbWriteLine,outFile)!=size) {
		_file->close();
		return -1;
    }
    csv_fini(&_p,cb1,cb2,outFile);   
    // close the file:
    _file->close();
//    csv_free(&_p);
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

template <class T>
String DFRobot_CSV<T>::readItem(uint16_t row, uint16_t field)
{
    uint32_t size;
	String des;
	pos = 0;
    void  *outFile;
    uint32_t pt = 0;
	uint8_t readBuf[512] = {0};
    size = _file->size();
    while(size) {
        uint16_t temp;
        if(size>512) temp = 512;
        else temp = size;
        _file->read(readBuf,temp);
        if(csv_parse(&_p,readBuf,temp,cbReadAfterField,cbReadAfterRow,des)!=temp) {
          _file->close();
		  return -1;
		}
        _file->seek(pt+=temp);
        size -= temp;
    }
    csv_fini(&_p,cbAftFieldWriFile,cbAftRowWriFile,outFile);   
    // close the file:
//	csv_free(&_p);
	return des;
}

static struct {
  bool flag;
  uint16_t positon;
  uint16_t row;
  bool end;
} countForReadRow;

static struct readPa{
	void *des;
	uint16_t row;
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

template <class T>
String DFRobot_CSV<T>::readRow(uint16_t row)
{
	uint32_t size;
    void  *outFile;
    uint32_t pt = 0;
	uint8_t readBuf[512] = {0};
	memset(&countForReadRow,0,sizeof(countForReadRow));
	readPar.des = des;
	readPar.row = row;
	size = _file->size();
	while(!countForReadRow.end && size) {
		uint16_t temp;
        if(size>512) temp = 512;
        else temp = size;
        _file->read(readBuf,temp);
        if(csv_parse(&_p,readBuf,temp,cbReadRow,cbAftRowCount,String des)!=temp) {
          _file->close();
		  return -1;
		}
        _file->seek(pt+=temp);
        size -= temp;
	}
	csv_fini(&_p,cbAftFieldWriFile,cbAftRowWriFile,outFile);   
//	csv_free(&_p);
	return des;
}




