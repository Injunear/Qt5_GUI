#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <QtCore> //?

#include <QMainWindow>

class mythread : public QThread
{
    Q_OBJECT


public:
    explicit mythread(QObject *parent = 0);
    void run();
    bool running;
    char reset, status, range, range_status, addrW = 0x52, addrR = 0x53;
    int  a, num = 0;

    void WriteByte(wchar_t reg, char data)
    {
        char data_write[3];

        data_write[0] = (reg >> 8) & 0xFF; // MSB of register address
        data_write[1] = reg & 0xFF; // LSB of register address
        data_write[2] = data & 0xFF;

        a = wiringPiI2CWriteReg16(addrW, (int)data_write, 3);
    }


    char ReadByte(wchar_t reg)
    {
       // char data_write[2];
        char data_write[2], data_read;
    //    int data_read[2]; // Why make an array of one???  char
    //    char data_read;
        data_read = 0x00;

        data_write[0] = (reg >> 8) & 0xFF; // MSB of register address
        data_write[1] = reg & 0xFF; // LSB of register address

     //   qDebug() << "ReadByte reg = " << reg;

    //    data_read[0] = reg & 0xFF; // Example code adds “& 0xFF”
    //    data_read[1] = 0x00;

     //   qDebug() << "ReadByte 2";

        a = wiringPiI2CWriteReg16(addrW, (int)data_write, 2); // Does this output good data?

        a = wiringPiI2CReadReg8(addrR, data_read);

        qDebug() << "ReadByte data_read out = " << (int)data_read;
       // return data_read[1];
        return data_read;
    }


//int VL6180X_Init();
/*
int VL6180X_Init()
{
char reset;
reset = ReadByte(0x016);
if(reset == 1) // Check to see if it has already been initialized
{
    WriteByte(0x016, 0x00); // Change fresh out of set status to 0
}
return 0; // Why not make this void return?
}


int VL6180X_Poll_Range()
{
char status, range_status;
status = ReadByte(0x04F);
range_status = status & 0x07;

while(range_status != 0x04)
{
    status = ReadByte(0x04F);
    range_status = status & 0x07;
}
return 0; // Why not make this void return?
}


int VL6180X_Read_Range()
{
int range;
range = ReadByte(0x015); // IS 0x015 CORRECT?????????????????????????
return range;
}



int VL6180X_Clear_Interrupts()
{
WriteByte(0x015, 0x07);
return 0;
}


int VL6180X_Start_Range() // Start a range measurement in Single Shot mode
{
WriteByte(0x018, 0x01); // Why not put this in main() ?
return 0; // Why not make this void return?
}
*/

public slots:
    void stopRunning();
  //  void doStopRunning();

signals:
    void Number1Changed(int);
    void Number2Changed(int);





};

#endif // MYTHREAD_H
