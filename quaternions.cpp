#include<iostream>
#include<stdio.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<linux/i2c.h>
#include<linux/i2c-dev.h>
#include<iomanip>
#include<MadgwickAHRS.h>
using namespace std;

// Small macro to display value in hexadecimal with 2 places
#define HEX(x) setw(2) << setfill('0') << hex << (int)(x)
//Gyroscope
#define DEVID       0x0f
#define GX0      0x28
#define GX1      0x29
#define GY0      0x2A
#define GY1      0x2B
#define GZ0      0x2C
#define GZ1      0x2D
//Magnetometer
#define MX0      0x08
#define MX1      0x09
#define MY0      0x0A
#define MY1      0x0B
#define MZ0      0x0C
#define MZ1      0x0D
//Accelerometer
#define AX0      0x28
#define AX1      0x29
#define AY0      0x2A
#define AY1      0x2B
#define AZ0      0x2C
#define AZ1      0x2D
#define BUFFER_SIZE 0x40
unsigned char dataBuffer[BUFFER_SIZE];

int writeRegister(int file, unsigned char address, char value){
   unsigned char buffer[2];
   buffer[0] = address;
   buffer[1] = value;
   if (write(file, buffer, 2)!=2){
      cout << "Failed write to the device" << endl;
      return 1;
   }
   return 0;
}

int readRegisters(int file, char reg){
char value;
char temp = reg;

  write(file,&temp,1);
  read(file,&value,1);


   return value;
}

// short is 16-bits in size on the BBB
short combineValues(unsigned char msb, unsigned char lsb){
   //shift the msb right by 8 bits and OR with lsb
   return ((short)msb<<8)|(short)lsb;
}

int main(){
   int file;
   int file1;
   cout << "Starting the process" << endl;
   if((file=open("/dev/i2c-1", O_RDWR)) < 0){
      cout << "failed to open the bus" << endl;
      return 1;
   }
   if((file1=open("/dev/i2c-1", O_RDWR)) < 0){
      cout << "failed to open the bus" << endl;
      return 1;
   }

   if(ioctl(file, I2C_SLAVE, 0x6b) < 0){
      cout << "Failed to connect to the sensor" << endl;
      return 1;
   }

   if(ioctl(file1, I2C_SLAVE, 0x1d) < 0){
      cout << "Failed to connect to the sensor" << endl;
      return 1;
   }


   cout << "The Device ID of Gyroscope is: 0x" << HEX(readRegisters(file, DEVID)) << endl;
   cout << "The Device ID of Accelerometer and Magnetometer is: 0x" << HEX(readRegisters(file1, DEVID)) << endl;
   cout << dec << endl;

   // Now loop a display the x, y, z accelerometer for 60 seconds
   int count=0;
   while(count < 60000){
      short gx = combineValues(readRegisters(file,GX1), readRegisters(file,GX0));
      short gy = combineValues(readRegisters(file,GY1), readRegisters(file,GY0));
      short gz = combineValues(readRegisters(file,GZ1), readRegisters(file,GZ0));
      short ax = combineValues(readRegisters(file1,AX1), readRegisters(file,AX0));
      short ay = combineValues(readRegisters(file1,AY1), readRegisters(file,AY0));
      short az = combineValues(readRegisters(file1,AZ1), readRegisters(file,AZ0));
      short mx = combineValues(readRegisters(file1,MX1), readRegisters(file,MX0));
      short my = combineValues(readRegisters(file1,MY1), readRegisters(file,MY0));
      short mz = combineValues(readRegisters(file1,MZ1), readRegisters(file,MZ0));
      //Use \r and flush to write the output on the same line
      cout<<"Gyroscope values 			Accelerometer values 			Magnetometer values"<<endl<<endl;
      cout << "gx="<<gx<<" gy="<<gy<<" gz="<<gz<<"		ax = "<<ax<<" ay = "<<ay<<" az = "<<az<<"		mx = "<<mx<<" my = "<<my<<" mz = "<<mz<<" sample="<<count<<endl<<endl;
      MadgwickAHRSupdate(gx,gy,gz,ax,ay,az,mx,my,mz);
      cout<<"q0 = "<<q0<<" q1 = "<<q1<<" q2 = "<<q2<<" q3 = "<<q3<<endl<<endl;
      usleep(100000);
       //read the sensor again
      count++;
   }
   close(file);
   close(file1);
   return 0;
}
