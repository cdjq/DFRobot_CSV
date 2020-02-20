#include <DFRobot_CSV.h>
#include <variant.h>
DFRobot_CSV::DFRobot_CSV()
{
	csv_init(&_p,0);
	_file = NULL;
}


DFRobot_CSV::DFRobot_CSV(File &file)
{
	csv_init(&_p,0);
	_file = & file;
}


DFRobot_CSV::~DFRobot_CSV()
{
	if(_p.entry_buf)
		_p.free_func(_p.entry_buf);
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
    tem = write(num);
	tem += write("\r\n");
	return tem;
  } else if (base == 10) {
    if (num < 0) {
      int t = print('-');
      num = -num;
      tem = printNumber(num, 10) + t;
	  tem += write("\r\n");
	  return tem;
    }
    tem = printNumber(num, 10);
	tem += write("\r\n");
	return tem;
  } else {
    tem = printNumber(num, base);
	tem += write("\r\n");
	return tem;
  }
}

size_t DFRobot_CSV::println(unsigned long n, int base)
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

/*
DFRobot_CSV::operator bool() {
  return (_file->available());
}
*/

