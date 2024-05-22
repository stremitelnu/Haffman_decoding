#include <QCoreApplication>
#include <iostream>
#include "encoding.h"
#include <QString>
#include <QTextStream>
#include <QDataStream>
QTextStream cout(stdout);
QTextStream cin(stdin);

int main(int argc, char *argv[])
{
   // int x;
    encoding Encod;

   // QString cmd_name="encoding";
   // QString command;
   // QString file_name;

    QCoreApplication a(argc, argv);
    if(argc>1)
    {
     //cout << "enter command" << endl;
    // command=cin.readLine();

     //x= QString::compare(cmd_name,command);
     //if(x==0)
     //{
       //cout << "enter file path" << endl;
       //file_name=cin.readLine();
       //QByteArray byteArray1 = file_name.toUtf8();
       //const char* inputFile = byteArray1.constData();
       Encod.huffmanEncode(argv[1]);
       //cout << "encoding success" << endl;
     //}
    }
    a.exit();
    return 1;
}
