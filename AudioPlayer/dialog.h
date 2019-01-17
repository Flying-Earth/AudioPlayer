#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QTimer>
#include <QLCDNumber>
#include <QGridLayout>
#include <QCheckBox>
#include <QMediaPlaylist>
#include <QSqlQuery>
#include <QProgressDialog>
#include <QFileDialog>
#include <QDir>
#include <QMediaPlayer>
#include <QSlider>
#include<QSqlTableModel>
#include<QPixmap>
#include<QPalette>
#include<QResizeEvent>

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = 0);
    ~Dialog();
public slots:
  void onTimerOut();
  void musicFile();
  void playMusic();
  void doubleClick();
  void modelMusic();
  void previousMusic();
  void nextMusic();
  void musicPlay();
  void voiceMusic();
  void upDatePosition(qint64 position);
  void upDateDuration(qint64 duration);
  void setMusicPosition(int position);
  void setVoice(int position);
  void deleteMusic();
  void openVideo();
  void receiverSignal();
signals:
  void showVideo();
private:
    QListWidget *musicList = NULL;
    QStackedWidget *stack;

    QTimer *Timer;
    QLCDNumber *lcdNumber;

    QSqlTableModel *table;
    QSqlQuery query;

    QMediaPlaylist *playList = NULL;
    QMediaPlayer *music;

    QPushButton *play;
    QPushButton *next;
    QPushButton *last;
    QPushButton *model;
    QPushButton *voice;
    QPushButton *video;

    QSlider *voiceProgress;
    QSlider *musicProgress;

    QLabel *currentDuration;
    QLabel *musicDuration;

    int currentVoice = 0;
};

#endif // DIALOG_H
