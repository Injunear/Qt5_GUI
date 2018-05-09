#include "mythread.h"
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringPiSPI.h>
#include <QDebug>
//#include <mainwindow.h>
//#include <QMainWindow>

#define addr (0x52)

mythread::mythread(QObject *parent) :
    QThread(parent)
{
   // connect(this, SIGNAL(doStopRunning()), mThread, SLOT(mThread::stopRunning()));
   // connect(mThread, SIGNAL(doStopRunning()), this, SLOT(mThread::stopRunning()));
 //   mythread::connect(MainWindow::MainWindow, SIGNAL(MainWindow::doStopRunning()), this, SLOT(mThread::stopRunning()));

  //  connect(MainWindow, SIGNAL(doStopRunning()), this, SLOT(stopRunning()));
}

// Destructor - stop thread
/*mythread::~mythread()
{
    int a=0, b=0, fd1=0, fd2=0;
    fd1=wiringPiI2CSetup(32);
    fd2=wiringPiI2CSetup(33);
    a = wiringPiI2CWriteReg8(fd1,18,129);
    b = wiringPiI2CWriteReg8(fd2,18,129);
}
*/


void mythread::stopRunning() // slot
{
    qDebug() << "stopRunning slot";
    running = false;
}


void mythread::run()
{
    //qDebug() << "Running";
    running = true;



    // VL6180X I2C

    // Program for VL6180
//    char reset, status, range_status;
 //   int range, reset;

    //VL6180X_Init();
    reset = ReadByte(0x016); // VL6180X_Init
    if(reset == 1) // Check to see if it has already been initialized
    {
        WriteByte(0x016, 0x00); // Change fresh out of set status to 0
    }

 //   qDebug() << "reset = " << (int)reset;
// How do the following register addresses compare with the offset register addresses listed on p49-50 on the data sheet?
// Below are the recommended settings required to be loaded onto the VL6180X during the initialisation of the device (see Section 1.3).

// The following may or may not be necessary. If so, I don't know why.
// I am unable to determine the register addresses even though I have posted the question twice on the ST forum.
// Mandatory : private registers
    WriteByte(0x0207, 0x01);
    WriteByte(0x0208, 0x01);
    WriteByte(0x0133, 0x01); // From ST forum post
    WriteByte(0x0096, 0x00);
    WriteByte(0x0097, 0xfd);
    WriteByte(0x00e3, 0x00);
    WriteByte(0x00e4, 0x04);
    WriteByte(0x00e5, 0x02);
    WriteByte(0x00e6, 0x01);
    WriteByte(0x00e7, 0x03);
    WriteByte(0x00f5, 0x02);
    WriteByte(0x00d9, 0x05);
    WriteByte(0x00db, 0xce);
    WriteByte(0x00dc, 0x03);
    WriteByte(0x00dd, 0xf8);
    WriteByte(0x009f, 0x00);
    WriteByte(0x00a3, 0x3c);
    WriteByte(0x00b7, 0x00);
    WriteByte(0x00bb, 0x3c);
    WriteByte(0x00b2, 0x09);
    WriteByte(0x00ca, 0x09);
    WriteByte(0x0198, 0x01);
    WriteByte(0x01b0, 0x17);
    WriteByte(0x01ad, 0x00);
    WriteByte(0x00ff, 0x05);
    WriteByte(0x0100, 0x05);
    WriteByte(0x0199, 0x05);
    WriteByte(0x01a6, 0x1b);
    WriteByte(0x01ac, 0x3e);
    WriteByte(0x01a7, 0x1f);
    WriteByte(0x0030, 0x00);

    // Recommended : Public registers - See data sheet for more detail
    WriteByte(0x0011, 0x10); // Enables polling for ‘New Sample ready’
    // when measurement completes

    WriteByte(0x010a, 0x30); // Set the averaging sample period
    // (compromise between lower noise and increased execution time)

    WriteByte(0x003f, 0x46); // Sets the light and dark gain (upper nibble).
    // Dark gain should not be changed

    WriteByte(0x0031, 0xFF); // sets the # of range measurements after
    // which auto calibration of system is performed

    WriteByte(0x0040, 0x63); // Set ALS integration time to 100ms
    qDebug() << "WriteBytes Init done";


    while(1)
    {
        // VL6180X_Start_Range();
        WriteByte(0x018, 0x01); //  VL6180X_Start_Range **Put this in a subroutine?**  0x18 is 22
        msleep(0); // This is needed
       // qDebug() << "Start Range";

        // VL6180X_Poll_Range();
        status = ReadByte(0x04F); //  VL6180X_Poll_Range 0x4F is 79
        qDebug() << "Begin Poll Range";

        range_status = status & 0x07;//  **Put this in a subroutine?**
        qDebug() << "???"; // This is needed. Above command hangs without it.

        // msleep(3000) doesn't make a difference

        while(range_status != 0x04) // No Difference with 0x02
        { // Some difference with ==
            status = ReadByte(0x04F); // 0x4F is 79
            qDebug() << "Status = " << (int)status;
            range_status = status & 0x07;
            qDebug() << "range_status = " << (int)range_status;
            qDebug();
            msleep(100);
        }
        qDebug() << "End Poll Range";

      // range = VL6180X_Read_Range();
       range = ReadByte(0x062); //  VL6180X_Read_Range()

      // VL6180X_Clear_Interrupts();
        WriteByte(0x015, 0x07); // VL6180X_Clear_Interrupts

        qDebug() << "While Range = " << (int)range;
        // wait a bit
        // msleep(100);
    }

} // End run VL6180X I2C




// End VL6180X I2C section



// SPI MCP3002 ADC
/*   int ret = 0;
unsigned char buff[3] = {0};
unsigned char LoByte = 0, HiByte = 0;

if(wiringPiSPISetup(0, 5000) == -1) // Could this be too slow?  500k is more standard
{
    qDebug() << "Could not inialize SPI.";
    return;
}

 while(1)
{
   // Appears to be reading the MCP3002 linearly
   // YAY!
   // With 120 ohm R on GND side of pot the ADC outputs from 762 to 32767

   buff[0] = 0x09; // Sets three config registers and gives start bit
   buff[1] = 0x00;
   buff[2] = 0x00;

   ret = wiringPiSPIDataRW(0, buff, 3);
   HiByte = buff[1];SPI MCP3002 ADC
   LoByte = buff[2];

   qDebug() << HiByte << "   " << LoByte << "   " << (HiByte << 8) + LoByte ;
   msleep(500);
 }

} // End thread run for SPI MCP3002 ADC
*/




/*
//  I2C
    int     fd1 = 0, fd2 = 0, a= 0, b= 0, c = 0; //, acc2 = 0
    quint8  xL = 0, yL = 0, zL = 0; // qint8 = signed char, quint8 = unsigned char
    qint8   Lval = 0, xH = 0, yH = 0, zH = 0;
    int16_t Hval = 0;
    msleep(500); // Delay to make sure chips have time to setup after power on.
    fd1=wiringPiI2CSetup(32); // Set device numbers on I2C bus
    fd2=wiringPiI2CSetup(33);
  //  acc2=wiringPiI2CSetup(25);
    // set pin i/o dir
    a= wiringPiI2CWriteReg8(fd1,0,0); // Set chip 1 port a to output
    a= wiringPiI2CWriteReg8(fd1,1,255); // Set chip 1 port b to input
    a= wiringPiI2CWriteReg8(fd2,0,0); // Set chip 2 port a to output
    a= wiringPiI2CWriteReg8(fd2,1,255); // Set chip 2 port b to input
    a= wiringPiI2CWriteReg8(fd1,13,255); // Set chip 1 port b to pu
    a= wiringPiI2CWriteReg8(fd2,13,255); // Set chip 2 port b to pu
    a= wiringPiI2CWriteReg8(fd1,18,129);
    a= wiringPiI2CWriteReg8(fd2,18,129);

 //   a= wiringPiI2CWriteReg8(acc2,30,144);  // p34 - 0x1E 30d CTRL_REG0 144d 0x90
 //   a= wiringPiI2CWriteReg8(acc2,32,119);  // p35 - 0x20 32d CTRL_REG1 0x77 119d                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         ); // 146
 //   a= wiringPiI2CWriteReg8(amitcc2,35,136); // p37 - 0x23  35d CTRL_REG4  0x88 136d  0x80 128d
   // a= wiringPiI2CWriteReg8(acc2,46,0);    // p40 - 0x2E 46d FIFO_CTRL_REG 0
//    msleep(200); // Allow chips time to set new configurations

/*    for(int I=0;I<10000;I++)
    {
        a = wiringPi    int     fd1 = 0, fd2 = 0, a= 0, b= 0, c = 0; //, acc2 = 0
    quint8  xL = 0, yL = 0, zL = 0; // qint8 = signed char, quint8 = unsigned char
    qint8   Lval = 0, xH = 0, yH = 0, zH = 0;
    int16_t Hval = 0;
    msleep(500); // Delay to make sure chips have time to setup after power on.
    fd1=wiringPiI2CSetup(32); // Set device numbers on I2C bus
    fd2=wiringPiI2CSetup(33);
  //  acc2=wiringPiI2CSetup(25);
    // set pin i/o dir
    a= wiringPiI2CWriteReg8(fd1,0,0); // Set chip 1 port a to output
    a= wiringPiI2CWriteReg8(fd1,1,255); // Set chip 1 port b to input
    a= wiringPiI2CWriteReg8(fd2,0,0); // Set chip 2 port a to output
    a= wiringPiI2CWriteReg8(fd2,1,255); // Set chip 2 port b to input
    a= wiringPiI2CWriteReg8(fd1,13,255); // Set chip 1 port b to pu
    a= wiringPiI2CWriteReg8(fd2,13,255); // Set chip 2 port b to pu
    a= wiringPiI2CWriteReg8(fd1,18,129);
    a= wiringPiI2CWriteReg8(fd2,18,129);

 //   a= wiringPiI2CWriteReg8(acc2,30,144);  // p34 - 0x1E 30d CTRL_REG0 144d 0x90
 //   a= wiringPiI2CWriteReg8(acc2,32,119);  // p35 - 0x20 32d CTRL_REG1 0x77 119d                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         ); // 146
 //   a= wiringPiI2CWriteReg8(amitcc2,35,136); // p37 - 0x23  35d CTRL_REG4  0x88 136d  0x80 128d
   // a= wiringPiI2CWriteReg8(acc2,46,0);    // p40 - 0x2E 46d FIFO_CTRL_REG 0
//    msleep(200); // Allow chips time to set new configurations

/*    for(int I=0;I<10000;I++)
    {
        a = wiringPiI2CWriteReg8(fd1,18,129);
        a = wiringPiI2CWriteReg8(fd2,18,0);
        msleep(250);
        a = wiringPiI2CWriteReg8(fd1,18,0);
        a = wiringPiI2CWriteReg8(fd2,18,129);
        msleep(250);
        b = wiringPiI2CReadReg8(fd1,19);
        c = wiringPiI2CReadReg8(fd2,19);

        qDebug() << b << "  doStopRunning()" << c;
    }
* /
   // qDebug() << running;

    while(running) // mythread::
    {

/*
 *      // I2C accelerometer section
 *      xL = wiringPiI2CReadReg8(acc2,40);
        xH = wiringPiI2CReadReg8(acc2,41); // Does it matter which byte is read first?
        yH = wiringPiI2CReadReg8(acc2,43);
        yL = wiringPiI2CReadReg8(acc2,42);
        zH = wiringPiI2CReadReg8(acc2,45);
        zL = wiringPiI2CReadReg8(acc2,44);

        if(xH & 0x80)
        {
            Lval = (-(xL >> 4))+ 1;
            Hval = -((xH & 0x7f) << 4);
        }
        else
        {
            Lval = (xL >> 4);
            Hval = xH << 4;
        }

        qDebug() << Hval << "   "  << Lval << "   " << Hval + Lval; //
        msleep(200);
* /

        b = wiringPiI2CReadReg8(fd1,19);
        c = wiringPiI2CReadReg8(fd2,19);
        qDebug() << b << "   " << c;
        msleep(300);

        emit Number1Changed(b);
        emit Number2Changed(c);

        if(b==254)
         {
            a = wiringPiI2CWriteReg8(fd1,18,1);
         }
        else
        {
            a = wiringPiI2CWriteReg8(fd1,18,129);
        }

        if(c==254)
         {
            a = wiringPiI2CWriteReg8(fd2,18,1);
         }
        else
        {
            a = wiringPiI2CWriteReg8(fd2,18,129);
        }



        if(b==127)
         {
            a = wiringPiI2CWriteReg8(fd1,18,128);
         }
        else
        {
            a = wiringPiI2CWriteReg8(fd1,18,129);
        }

        if(c==127)
         {
            a = wiringPiI2CWriteReg8(fd2,18,128);
         }
        else
        {
            a = wiringPiI2CWriteReg8(fd2,18,129);
        }
    } // End while runningI2CWriteReg8(fd1,18,129);
        a = wiringPiI2CWriteReg8(fd2,18,0);
        msleep(250);
        a = wiringPiI2CWriteReg8(fd1,18,0);
        a = wiringPiI2CWriteReg8(fd2,18,129);
        msleep(250);
        b = wiringPiI2CReadReg8(fd1,19);
        c = wiringPiI2CReadReg8(fd2,19);

        qDebug() << b << "  doStopRunning()" << c;
    }
* /
   // qDebug() << running;

    while(running) // mythread::
    {

/*
 *      // I2C accelerometer section
 *      xL = wiringPiI2CReadReg8(acc2,40);
        xH = wiringPiI2CReadReg8(acc2,41); // Does it matter which byte is read first?
        yH = wiringPiI2CReadReg8(acc2,43);
        yL = wiringPiI2CReadReg8(acc2,42);
        zH = wiringPiI2CReadReg8(acc2,45);
        zL = wiringPiI2CReadReg8(acc2,44);

        if(xH & 0x80)
        {
            Lval = (-(xL >> 4))+ 1;
            Hval = -((xH & 0x7f) << 4);
        }define addr (0x52)
        else
        {
            Lval = (xL >> 4);
            Hval = xH << 4;
        }

        qDebug() << Hval << "   "  << Lval << "   " << Hval + Lval; //
        msleep(200);
* /

        b = wiringPiI2CReadReg8(fd1,19);
        c = wiringPiI2CReadReg8(fd2,19);
        qDebug() << b << "   " << c;
        msleep(300);

        emit Number1Changed(b);
        emit Number2Changed(c);

        if(b==254)
         {
            a = wiringPiI2CWriteReg8(fd1,18,1);
         }
        else
        {
            a = wiringPiI2CWriteReg8(fd1,18,129);
        }

        if(c==254)
         {
            a = wiringPiI2CWriteReg8(fd2,18,1);
         }
        else
        {
            a = wiringPiI2CWriteReg8(fd2,18,129);
        }



        if(b==127)
         {
            a = wiringPiI2CWriteReg8(fd1,18,128);
         }
        else
        {
            a = wiringPiI2CWriteReg8(fd1,18,129);
        }

        if(c==127)
         {
            a = wiringPiI2CWriteReg8(fd2,18,128);
         }
        else
        {
            a = wiringPiI2CWriteReg8(fd2,18,129);
        }
    } // End while running
* /
    qDebug() << "Before thread exit";
    mythread::exit(); // mythread::
    qDebug() << "After thread exit";



    /*
     *
    a= wiringPiI2CWriteReg8(acc2,30,144);  // p34 - 0x1E 30d CTRL_REG0 144d 0x90(yH << 4)
    a= wiringPi

// VL6180X I2C

    // Program VL6180

    //#define addr (0x52)

    void WriteByte(wchar_t, char);

    char ReadByte(wchar_t);

    int VL6180X_Init();

    int VL6180X_Start_Range();

    int VL6180X_Poll_Range();

    int VL6180X_Read_Range();

    int VL6180X_Clear_Interrupts();


   // void main()
   // {
        int range;
        VL6180X_Init();

        while(1)
        {
            VL6180X_Start_Range();
            VL6180X_Poll_Range();
            range = VL6180X_Read_Range();
            VL6180X_Clear_Interrupts();

            pc.printf(“%d\r\n”, range);
            // wait a bit
        }
    // }



    void WriteByte(wchar_t reg, char data)
    {
        char data_write[3];

        data_write[0] = reg >> 8; // Example code adds “& 0xFF”
        data_write[1] = reg;
        data_write[2] = data;

        // Change -> i2c.write(addr, data_write, 3);
    }


    char ReadByte(wchar_t reg)
    {
        char data_write[2];
        char data_read[1]; // Why make this an array of one???

        data_write[0] = reg >> 8; // Example code adds “& 0xFF”
        data_write[1] = reg;


        // Change -> i2c.write(addr, data_write, 2);
        // Change -> i2c.read(addr, data_read, 1);
        return data_read[0];
    }



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


    int VL6180X_Start_Range() // Start a range measurement in Single Shot mode
    {
        WriteByte(0x018, 0x01); // Why not put this in main() ?
        return 0; // Why not make this void return?
    {



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
        range = ReadByte(0x015, 0x07);
        return range;
    }



    int VL6180X_Clear_Interrupts()
    {
        WriteByte(0x015, 0x07);
        return 0;
    }


I2CWriteReg8(acc2,32,119);  // p35 - 0x20 32d CTRL_REG1 0x77 119d                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         ); // 146
    a= wiringPiI2CWriteReg8(acc2,35,136);  // p37 - 0x23  35d CTRL_REG4  0x88 136d  0x80 128d
    a= wiringPiI2CWriteReg8(acc2,46,0);    // p40 - 0x2E 46d FIFO_CTRL_REG 0
     *
     */








/*   Misc - unused
    int     a= 0; //
    //quint8  xL = 0, yL = 0, zL = 0; // qint8 = signed char, quint8 = unsigned char
   // qint8   Lval = 0, xH = 0, yH = 0, zH = 0;
    //int16_t Hval = 0;
    msleep(50); // Delay to make sure chip has time to setup after power on.
   // fd1=wiringPiI2CSetup(32); // Set device numbers on I2C bus
   // fd2=wiringPiI2CSetup(33);
  //  acc2=wiringPiI2CSetup(25);
    // set pin i/o dir
    a= wiringPiI2CWriteReg8(fd1,0,0); // Set chip 1 port a to output
    a= wiringPiI2CWriteReg8(fd1,1,255); // Set chip 1 port b to input
    a= wiringPiI2CWriteReg8(fd2,0,0); // Set chip 2 port a to output
    a= wiringPiI2CWriteReg8(fd2,1,255); // Set chip 2 port b to input
    a= wiringPiI2CWriteReg8(fd1,13,255); // Set chip 1 port b to pu
    a= wiringPiI2CWriteReg8(fd2,13,255); // Set chip 2 port b to pu
    a= wiringPiI2CWriteReg8(fd1,18,129);
    a= wiringPiI2CWriteReg8(fd2,18,129);

 //   a= wiringPiI2CWriteReg8(acc2,30,144);  // p34 - 0x1E 30d CTRL_REG0 144d 0x90
 //   a= wiringPiI2CWriteReg8(acc2,32,119);  // p35 - 0x20 32d CTRL_REG1 0x77 119d                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         ); // 146
 //   a= wiringPiI2CWriteReg8(amitcc2,35,136); // p37 - 0x23  35d CTRL_REG4  0x88 136d  0x80 128d
   // a= wiringPiI2CWriteReg8(acc2,46,0);    // p40 - 0x2E 46d FIFO_CTRL_REG 0
//    msleep(200); // Allow chips time to set new configurations

/*    for(int I=0;I<10000;I++)
    {
        a = wiringPiI2CWriteReg8(fd1,18,129);
        a = wiringPiI2CWriteReg8(fd2,18,0);
        msleep(250);
        a = wiringPiI2CWriteReg8(fd1,18,0);
        a = wiringPiI2CWriteReg8(fd2,18,129);
        msleep(250);
        b = wiringPiI2CReadReg8(fd1,19);
        c = wiringPiI2CReadReg8(fd2,19);

        qDebug() << b << "  doStopRunning()" << c;
    }
* /
   // qDebug() << running;

    while(running) // mythread::
    {

/*
 *      // I2C accelerometer section
 *      xL = wiringPiI2CReadReg8(acc2,40);
        xH = wiringPiI2CReadReg8(acc2,41); // Does it matter which byte is read first?
        yH = wiringPiI2CReadReg8(acc2,43);
        yL = wiringPiI2CReadReg8(acc2,42);
        zH = wiringPiI2CReadReg8(acc2,45);
        zL = wiringPiI2CReadReg8(acc2,44);

        if(xH & 0x80)
        {
            Lval = (-(xL >> 4))+ 1;
            Hval = -((xH & 0x7f) << 4);
        }
        else
        {
            Lval = (xL >> 4);
            Hval = xH << 4;
        }

        qDebug() << Hval << "   "  << Lval << "   " << Hval + Lval; //
        msleep(200);
* /

        b = wiringPiI2CReadReg8(fd1,19);
        c = wiringPiI2CReadReg8(fd2,19);
        qDebug() << b << "   " << c;
        msleep(300);

        emit Number1Changed(b);
        emit Number2Changed(c);

        if(b==254)
         {
            a = wiringPiI2CWriteReg8(fd1,18,1);
         }
        else
        {
            a = wiringPiI2CWriteReg8(fd1,18,129);
        }

        if(c==254)
         {
            a = wiringPiI2CWriteReg8(fd2,18,1);
         }
        else
        {
            a = wiringPiI2CWriteReg8(fd2,18,129);
        }



        if(b==127)
         {
            a = wiringPiI2CWriteReg8(fd1,18,128);
         }
        else
        {
            a = wiringPiI2CWriteReg8(fd1,18,129);
        }

        if(c==127)
         {
            a = wiringPiI2CWriteReg8(fd2,18,128);
         }
        else
        {
            a = wiringPiI2CWriteReg8(fd2,18,129);
        }
    } // End while running
*/

/*
// SPI LIS3DH
    int ret = 0;
    unsigned char buff[3] = {0};
    unsigned char   LoByte = 0, HiByte = 0, buff2a = 0, buff2b = 0; // , who = 0
    qint8   Lval = 0, StatByte = 0;
    qint16  Hval = 0;

    if(wiringPiSPISetup(0, 500000) == -1)
    {
        qDebug() << "Could not inialize SPI.";
        return;
    }

    buff[0] = 0x1E; // write CTRL_REG0
    buff[1] = 0x90; // set SDO_PU_DISC to 1 (0x80) to disable SDO/SAO pull up, 0x10 is required
    ret = wiringPiSPIDataRW(0, buff, 2);

    buff[0] = 0x20; // write CTRL_REG1
    buff[1] = 0x21; // Set ODR Hz speed  0x7?=400, /*0x6?=200, 0x5?=100, 0x4?=50, 0x3?=25, 0x9?=1.344k
                    // Enable X, Y, Z axis,  0x?1 = X, 0x?2 = Y, Ox?4 = Z, 0x?7 = X, Y, & Z
    ret = wiringPiSPIDataRW(0, buff, 2);

    buff[0] = 0x23; // write CTRL_REG4
    buff[1] = 0x80; // 0x8? = out regs not updated until LSB & MSB read, 0x?8 = high res out
    ret = wiringPiSPIDataRW(0, buff, 2);

//    buff[0] = 0x2E; // write FIFO_CTRL_REG
//    buff[1] = 0x00; // 0x00 = FIFO bypass
//    ret = wiringPiSPIDataRW(0, buff, 2);

    msleep(500); // Allow chips time to set new configurations

    while(1)
    {
        // XXXXXXX
        // Set buff to read output registers
        buff[0] = 0xA8; // 0xAA read OUT_Y_L, 0xA8 read OUT_X_L
        buff[1] = 0x00; // unused data
        ret = wiringPiSPIDataRW(0, buff, 3);
        LoByte = buff[1];

        buff[0] = 0xA9; // 0xAB read OUT_Y_H, 0xA9 read OUT_X_H
        buff[1] = 0x00; // unused data
        ret = wiringPiSPIDataRW(0, buff, 2);
        HiByte = buff[1];

/*        buff[0] = 0x07; // Read Status Reg
        buff[1] = 0x00; // unused data
        ret = wiringPiSPIDataRW(0, buff, 2);
        StatByte = buff[1] & 0x07; // 01 = X axis?

        buff[0] = 0x8F; // Read WHO_AM_I reg
        buff[1] = 0x00; // unused data
        ret = wiringPiSPIDataRW(0, buff, 2);
        who = buff[1];
* /

        if(HiByte & 0x80)
        {
            Lval = (-(LoByte >> 6)) + 1;
            Hval = (-(HiByte & 0x7f)); // << 2
        }
        else
        {
            Lval = LoByte >> 6;
            Hval = HiByte ; // << 2
        }

        qDebug()  << Hval <<  "   "
using namespace std;  // ? << Lval << "   " << Hval + Lval; //
        msleep(300);
*/

    // SPI ADIS16201
/*        int ret = 0;
        unsigned char buff[4] = {0};
        unsigned char   LoByte = 0, HiByte = 0; //
      //  qint8   Lval = 0;
        qint16  Hval = 0, Lval = 0;

        if(wiringPiSPISetup(0, 1000) == -1)
        {
            qDebug() << "Could not inialize SPI.";
            return;
        }


        while(1)
        {
            // Set buff to read output registers
            buff[0] = 0x0C; // 0x0C = XINCL_OUT,  0x0E = YINCL_OUT  0 = read
            buff[1] = 0x00; // unused data
            buff[2] = 0x00;
            buff[3] = 0x00;
            ret = wiringPiSPIDataRW(0, buff, 4); // According to the forum, the second two bytes are the data output
            HiByte = buff[2];
            LoByte = buff[3];

/*
            buff[0] = 0x0C; // 0x0C = XINCL_OUT,  0x0E = YINCL_OUT  0 = read
            buff[1] = 0x00; // unused data
            ret = wiringPiSPIDataRW(0, buff, 2);
            ret = wiringPiSPIDataRW(0, buff, 2); // This also seems a legitimate way to read the data out but it reads zeros.
            HiByte = buff[0];
            LoByte = buff[1];
*/

/*
            if(HiByte & 0x20) // Is this the twos compliment sign bit?
            {
                Lval = (-(LoByte)) + 1;
                Hval = (-(HiByte & 0x1F)) << 8; //
            }
            else
            {
                Lval = LoByte;
                Hval = (HiByte & 0x1F) << 8; //
            }

            // The data is unchanging no matter what the PCB orientation is.
            qDebug()  << (HiByte) <<  "   "  << LoByte << "   " << Hval + Lval; // & 0x1F
            msleep(300);

        }
*/

