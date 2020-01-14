#include <DFRobot_CSV.h>
int DFRobot_CSV::csvFileWrite(File *fp, const void *src, size_t src_size, unsigned char quote)
{
  const unsigned char *csrc = (const unsigned char *)src;

  if (fp == NULL || src == NULL)
    return 0;

  if (fp->write(quote) == false)
    return false;

  while (src_size) {
    if (*csrc == quote) {
      if (fp->write(quote) == false)
        return false;
    }
    if (fp->write(*csrc) == false)
      return false;
    src_size--;
    csrc++;
  }

  if (fp->write(quote) == false) {
    return false;
  }

  return 0;
}

static void cb1 (void *s, size_t i, void *outfile) 
{
  csv_fwrite((File *)outfile, s, i);
  ((File *)outfile)->write(",");
}

static void cb2 (int c, void *outfile) 
{
  ((File *)outfile)->seek(((File *)outfile)->position()-1);
  ((File *)outfile)->write("\n");
}


int DFRobot_CSV::fileTofile(const char *fileIn, const char *fileOut) 
{
    size_t i;
    uint16_t size;
    struct csv_parser p;
    void  *outfile; 
    uint16_t pt = 0;
    uint8_t readBuf[512] = {0};
    File myFile;	
    csv_init(&p,0);
    SD.begin(32);
    myFile = SD.open("fileIn.txt");
    size = myFile.size();
    while(size) {
        uint16_t temp;
        if(size>512) temp = 512;
        else temp = size;
        myFile.read(readBuf,temp);
        myFile.close();
        myFile = SD.open("fileOut.txt",O_CREAT|O_WRITE|O_APPEND);
        outfile = (void *)&myFile;
        if(csv_parse(&p,readBuf,temp,cb1,cb2,outfile)!=temp)
          Serial.println("error in csv_parse");
        myFile.close();
        myFile = SD.open("fileIn.txt");
        myFile.seek(pt+=temp);
        size -= temp;
    }
    myFile.close();
    myFile = SD.open("fileOut.txt",O_WRITE|O_APPEND);
    outfile = (void *)&myFile;
    csv_fini(&p,cb1,cb2,outfile);   
    // close the file:
    myFile.close();
//    csv_free(&p);
    return 0;
}


File myFile;
struct counts {
long unsigned fields;
long unsigned rows;
};
void cb1 (void *s, size_t len, void *data) {
((struct counts *)data)->fields++; }
void cb2 (int c, void *data) {
((struct counts *)data)->rows++; }
uint8_t readBuf[512] = {0};
struct counts c = {0};
void setup() {
     struct csv_parser p;
     uint32_t size;   
  // Open serial communications and wait for port to open:
  Serial.begin(115200);

  Serial.print("Initializing SD card...");

  if (!SD.begin(32)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // re-open the file for reading:
  myFile = SD.open("a3.txt");
    Serial.println("a3.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
   myFile.seek(0);
   size = myFile.size();
   Serial.print("a3.txt: Size="); Serial.println(size);
   csv_init(&p,0);
   while(size) {
      uint16_t temp;
      if(size>512) temp = 512;
      else temp = size;
      myFile.read(readBuf,temp);
      for(uint8_t i=0;i<20;i++)
      Serial.write(readBuf[i]);Serial.println();
      if(csv_parse(&p,readBuf,temp,cb1,cb2,&c)!=temp)
        Serial.println("error in csv_parse");   
      size -= temp;
    }
    csv_fini(&p,cb1,cb2,&c);     
    // close the file:
    myFile.close();
    Serial.print("fields: ");Serial.println(c.fields);
    Serial.print("rows: ");Serial.println(c.rows);
//    csv_free(&p);
    Serial.println("okkkkkkkkkkkkkkkk");
}