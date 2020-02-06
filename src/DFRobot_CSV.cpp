#include <DFRobot_CSV.h>


DFRobot_CSV::DFRobot_CSV()
{
	csv_init(&_p,0);
}

DFRobot_CSV::DFRobot_CSV(uint8_t option = 0)
{
	csv_init(&_p,option);
}

DFRobot_CSV::~DFRobot_CSV()
{
	if(_p->entry_buf)
		_p->free_func(_p->entry_buf);
}

static int DFRobot_CSV::csvFileWrite(File *fp, const void *src, size_t src_size, unsigned char quote)
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

static void cbAftFieldWriFile (void *s, size_t i, void *outFile) 
{
  csvFileWrite((File *)outFile, s, i, CSV_QUOTE);
  ((File *)outFile)->write(",");
}

static void cbAftRowWriFile (int c, void *outFile) 
{
  ((File *)outFile)->seek(((File *)outFile)->position()-1);
  ((File *)outFile)->write("\n");
}


int DFRobot_CSV::convert(const char *fileIn, const char *fileOut) 
{
    size_t i;
    uint16_t size;
    void  *outFile; 
    uint16_t pt = 0;
    uint8_t readBuf[512] = {0};
    File myFile;	
    SD.begin(32);
    myFile = SD.open(fileIn);
    size = myFile.size();
    while(size) {
        uint16_t temp;
        if(size>512) temp = 512;
        else temp = size;
        myFile.read(readBuf,temp);
        myFile.close();
        myFile = SD.open(fileOut,O_CREAT|O_WRITE|O_APPEND);
        outFile = (void *)&myFile;
        if(csv_parse(&_p,readBuf,temp,cbAftFieldWriFile,cbAftRowWriFile,outFile)!=temp) {
           myFile.close();
		   return -1;
		}
        myFile.close();
        myFile = SD.open(fileIn);
        myFile.seek(pt+=temp);
        size -= temp;
    }
    myFile.close();
    myFile = SD.open(fileOut,O_WRITE|O_APPEND);
    outFile = (void *)&myFile;
    csv_fini(&_p,cbAftFieldWriFile,cbAftRowWriFile,outFile);   
    // close the file:
    myFile.close();
//    csv_free(&_p);
    return 0;
}



struct counts {
long unsigned fields;
long unsigned rows;
};
static void cbAftFieldCountField (void *s, size_t len, void *data) 
{
((struct counts *)data)->fields++; 
}
static void cbAftRowCountRow (int c, void *data) 
{
((struct counts *)data)->rows++; 
}

int DFRobot_CSV::count(const char *fileName, void * field, void * row)
{
  struct counts c = {0};
  uint32_t size;   
  uint8_t readBuf[512] = {0};
  struct counts c = {0};
  Serial.print("Initializing SD card...");

  if (!SD.begin(32)) {
    Serial.println("initialization failed!");
    while (1);
  }

  // re-open the file for reading:
   myFile = SD.open(fileName);

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
   myFile.seek(0);
   size = myFile.size();
//   Serial.print("a3.txt: Size="); Serial.println(size);
   csv_init(&_p,0);
   while(size) {
      uint16_t temp;
      if(size>512) temp = 512;
      else temp = size;
      myFile.read(readBuf,temp);
      for(uint8_t i=0;i<20;i++)
      Serial.write(readBuf[i]);Serial.println();
      if(csv_parse(&_p,readBuf,temp,cbAftFieldCountField,cbAftRowCountRow,&c)!=temp) {
        myFile.close();
		return -1;
	  }
      size -= temp;
    }
    csv_fini(&_p,cbAftFieldCountField,cbAftRowCountRow,&c);     
    // close the file:
    myFile.close();
 //   Serial.print("fields: ");Serial.println(c.fields);
 //   Serial.print("rows: ");Serial.println(c.rows);
    (uint32_t *)field = c.fields;
	(uint32_t *)row = c.rows;
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

int DFRobot_CSV::writeCSV(const char *fileName, const char *csv) 
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
		myFile.close();
		return -1;
    }
    csv_fini(&_p,cb1,cb2,outFile);   
    // close the file:
    myFile.close();
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
}

int DFRobot_CSV::readCSV(const char *fileName, void *des)
{
    uint32_t size;
	pos = 0;
    void  *outFile;
    uint32_t pt = 0;
	uint8_t readBuf[512] = {0};
    SD.begin(32);
    myFile = SD.open(fileName);
    size = myFile.size();
    while(size) {
        uint16_t temp;
        if(size>512) temp = 512;
        else temp = size;
        myFile.read(readBuf,temp);
        if(csv_parse(&_p,readBuf,temp,cbReadAfterField,cbReadAfterRow,des)!=temp) {
          myFile.close();
		  return -1;
		}
        myFile.seek(pt+=temp);
        size -= temp;
    }
    csv_fini(&_p,cbAftFieldWriFile,cbAftRowWriFile,outFile);   
    // close the file:
    myFile.close();
//	csv_free(&_p);
	return 0;
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

int DFRobot_CSV::readRow(const char *fileName, uint16_t row, void *des)
{
	File myFile;
	uint32_t size;
    void  *outFile;
    uint32_t pt = 0;
	uint8_t readBuf[512] = {0};
	memset(&countForReadRow,0,sizeof(countForReadRow));
	readPar.des = des;
	readPar.row = row;
	myFile = SD.open(fileName);
	size = myFile.size();
	while(!countForReadRow.end && size) {
		uint16_t temp;
        if(size>512) temp = 512;
        else temp = size;
        myFile.read(readBuf,temp);
        if(csv_parse(&_p,readBuf,temp,cbReadRow,cbAftRowCount,(void *)&readPar)!=temp) {
          myFile.close();
		  return -1;
		}
        myFile.seek(pt+=temp);
        size -= temp;
	}
	csv_fini(&_p,cbAftFieldWriFile,cbAftRowWriFile,outFile);   
    myFile.close();
//	csv_free(&_p);
	return 0;
}

DFRobot_CSV csv;


