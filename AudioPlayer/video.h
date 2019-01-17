#ifndef VIDEO_H
#define VIDEO_H
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
#include<QVideoWidget>
class Video : public QDialog
{
    Q_OBJECT

public:
    Video(QWidget *parent = 0);
    ~Video();
public slots:
  void onTimerOut();
  void videoFile();
  void playVideo();
  void doubleClick();
  void previousVideo();
  void nextVideo();
  void videoPlay();
  void voiceVideo();
  void upDatePosition(qint64 position);
  void upDateDuration(qint64 duration);
  void setVideoPosition(int position);
  void setVoice(int position);
  void deleteVideo();
  void openMusic();
  void receiverSignal();
signals:
  void showMusic();
private:
    QListWidget *videoList = NULL;
    QStackedWidget *stack;
    QTimer *Timer;
    QLCDNumber *lcdNumber;

    QSqlTableModel *table;
    QSqlQuery query;

    QMediaPlaylist *playList = NULL;
    QMediaPlayer *video;
    QVideoWidget* videoWidget;

    QPushButton *play;
    QPushButton *next;
    QPushButton *last;
    QPushButton *voice;
    QPushButton *music;

    QSlider *voiceProgress;
    QSlider *videoProgress;

    QLabel *currentDuration;
    QLabel *videoDuration;

    int currentVoice = 0;
};

#endif // VIDEO_H
