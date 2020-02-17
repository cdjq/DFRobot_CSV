
#include <SPI.h>
#include <UD.h>
#include<DFRobot_CSV.h>

File myFile;

void setup() {
  // Open serial communications and wait for port to open:
  SerialUSB.begin(115200);
  SerialUSB.print("Initializing FlashDisk...");
  if (!UD.begin(32)) {
    Serial.println("initialization failed!");
    while (1);
  }
  SerialUSB.println("initialization done.");

  myFile = UD.open("abc.txt", FILE_WRITE);
  DFRobot_CSV csv(myFile);
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    csv.print("a");csv.print("b");csv.println("c");
    csv.print("1");csv.print("2");csv.println("3");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
*/
  // re-open the file for reading:
  myFile = SD.open("abc.txt");
  if (myFile) {
    Serial.println("abc.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}
