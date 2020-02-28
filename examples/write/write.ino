/*!
 * @file write.ino
 * @brief 创建一个abc.csv文件并往里输入数据，最后读出文件内容
 * @n 实验现象：串口监视器打印出abc.csv文件内容
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [LiYue](liyue.wang@dfrobot.com)
 * @version  V1.0
 * @date  2019-2-28
 * @get from https://www.dfrobot.com
 * @url https://github.com/cdjq/DFRobot_CSV
 */
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

  myFile = UD.open("abc.csv", O_WRITE|O_CREAT);                   //以写的方式打开abc.csv文件，没有的话会创建改文件
  DFRobot_CSV csv(myFile);
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.println("Writing to test.txt...");
    csv.print("姓名");csv.print("学号");csv.println("分数");        //以csv格式往文件写入数据，用print输入每列的值，用println输入值并换行
    csv.print("Tom\",&Jerry");csv.print("1");csv.println(3.65);
    csv.print("Herny");csv.print("10");csv.println(99.5);
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
*/
  // re-open the file for reading:
  myFile = SD.open("abc.csv");
  if (myFile) {
    Serial.println("abc.csv:");

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

void loop() {
  }
