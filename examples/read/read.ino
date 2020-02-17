#include <SPI.h>
#include <UD.h>
#include<DFRobot_CSV.h>

File myFile;
int row,field;
void setup() {
  // Open serial communications and wait for port to open:
  SerialUSB.begin(115200);
  SerialUSB.print("Initializing FlashDisk...");
  if (!UD.begin(32)) {
    SerialUSB.println("initialization failed!");
    while (1);
  }
  SerialUSB.println("initialization done.");
  myFile = UD.open("abc.txt");
  DFRobot_CSV csv(myFile);
  if (myFile) {
      csv.count(row,field);
      SerialUSB.print("Rows: ");
      SerialUSB.print(row);SerialUSB.print("  ");
      SerialUSB.print("Fields per row:")
      SerialUSB.println(field);
      String row1 = csv.readRow(1);
      String row2 = csv.readRow(2);
      SerialUSB.println("The first row is:");
      SerialUSB.println(row1);
      SerialUSB.println("The second row is:");
      SerialUSB.println(row2);
      String item = csv.readItem(2,3);
      SerialUSB.println("The field (2,3) is:");
      SerialUSB.println(item);

    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    SerialUSB.println("error opening test.txt");
  }
}
