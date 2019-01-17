#include "dialog.h"
#include "video.h"
#include <QApplication>
#include"sql.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(!CreatConnection())
    {
        return 1;
    }
    Video v;
    v.setWindowIcon(QIcon(":/images/image/video.png"));

    Dialog w;
    w.setWindowIcon(QIcon(":/images/image/logo.png"));
    w.show();

    QObject::connect(&w,SIGNAL(showVideo()),&v,SLOT(receiverSignal()));
    QObject::connect(&v,SIGNAL(showMusic()),&w,SLOT(receiverSignal()));

    return a.exec();
}
