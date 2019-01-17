#include "video.h"
#include<QSplitter>
#include<QLabel>
#include<QLineEdit>
#include<QComboBox>
#include<QCheckBox>
#include<QLCDNumber>
#include<QDateTime>
#include<QPushButton>
#include<QDateTime>
#include<QFrame>
#include<QTimer>
#include<QGridLayout>
#include<QPixmap>
#include<QHBoxLayout>
#include<QDockWidget>
#include<QIcon>

Video::Video(QWidget *parent)
    : QDialog(parent)
{
    playList = new QMediaPlaylist(this);
    video = new QMediaPlayer(this);

    playList->setPlaybackMode(QMediaPlaylist::Loop);

    //堆栈界面一
    QGridLayout *ButtonLayout = new QGridLayout();

    videoProgress = new QSlider(this);
    videoProgress->setOrientation(Qt::Horizontal);
    videoProgress->setMaximum(100);
    videoProgress->setMinimum(0);

    QHBoxLayout *MButtonLayout = new QHBoxLayout();
    currentDuration = new QLabel(tr("00:00"));
    videoDuration =new QLabel(tr("00:00"));
    MButtonLayout->addWidget(currentDuration);
    MButtonLayout->addWidget(videoProgress);
    MButtonLayout->addWidget(videoDuration);
    ButtonLayout->addLayout(MButtonLayout,0,0,1,6);

    voiceProgress = new QSlider(this);
    voiceProgress->setOrientation(Qt::Horizontal);
    voiceProgress->setMaximum(100);
    voiceProgress->setMinimum(0);
    voiceProgress->setValue(30);

    connect(video,SIGNAL(positionChanged(qint64)),this,SLOT(upDatePosition(qint64)));
    connect(video,SIGNAL(durationChanged(qint64)),this,SLOT(upDateDuration(qint64)));
    connect(videoProgress,SIGNAL(sliderMoved(int)),this,SLOT(setVideoPosition(int)));
    connect(voiceProgress,SIGNAL(sliderMoved(int)),this,SLOT(setVoice(int)));

    play = new QPushButton();
    next = new QPushButton();
    last = new QPushButton();
    voice = new QPushButton();

    QIcon lastIcon(":/images/image/previous.png");
    last->setIconSize(QSize(32,32));
    last->setFlat(true);
    last->setIcon(lastIcon);

    QIcon nextIcon(":/images/image/next.png");
    next->setIconSize(QSize(32,32));
    next->setFlat(true);
    next->setIcon(nextIcon);

    QIcon playIcon(":/images/image/play.png");
    play->setIconSize(QSize(32,32));
    play->setFlat(true);
    play->setIcon(playIcon);

    QIcon voiceIcon(":/images/image/volume.png");
    voice->setIconSize(QSize(32,32));
    voice->setFlat(true);
    voice->setIcon(voiceIcon);

    connect(play,SIGNAL(clicked(bool)),this,SLOT(playVideo()));
    connect(last,SIGNAL(clicked(bool)),this,SLOT(previousVideo()));
    connect(next,SIGNAL(clicked(bool)),this,SLOT(nextVideo()));
    connect(voice,SIGNAL(clicked(bool)),this,SLOT(voiceVideo()));

    music = new QPushButton();
    QIcon videoIcon(":/images/image/logo.png");
    music->setIconSize(QSize(32,32));
    music->setFlat(true);
    music->setIcon(videoIcon);
    connect(music,SIGNAL(clicked(bool)),this,SLOT(openMusic()));

    QHBoxLayout *HButtonLayout = new QHBoxLayout();
    HButtonLayout->addWidget(last);
    HButtonLayout->addWidget(play);
    HButtonLayout->addWidget(next);
    HButtonLayout->addStretch();
    HButtonLayout->addWidget(voice);
    HButtonLayout->addWidget(voiceProgress);
    HButtonLayout->addWidget(music);

    ButtonLayout->addLayout(HButtonLayout,1,0,1,6);

    QGridLayout *mainRightLayout = new QGridLayout(this);
    mainRightLayout->addLayout(ButtonLayout,0,0);

    //主分割窗口
    QSplitter *splitterMain = new QSplitter(Qt::Horizontal,0);
    videoList = new QListWidget();

    query.exec("select * from VIDEO");
    while(query.next())
    {
        QString Name=query.value(1).toString();
        QString FileName=query.value(2).toString();
        if(Name!=""&&FileName!="")
        {
            QListWidgetItem *item=new QListWidgetItem;
            item->setIcon(QIcon(":/images/image/cemera.png"));
            item->setText(Name);
            videoList->addItem(item);
            playList->addMedia(QUrl::fromLocalFile(FileName));
        }
    }
    splitterMain->addWidget(videoList);
    connect(videoList,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(doubleClick()));

    //右部分窗口
    QSplitter *splitterRight = new QSplitter(Qt::Vertical,splitterMain);
    splitterRight->setOpaqueResize(false);

    QWidget *mainRightFirstTop = new QWidget();
    mainRightFirstTop->setLayout(mainRightLayout);

    videoWidget = new QVideoWidget();
    video->setVideoOutput(videoWidget);
    stack = new QStackedWidget(this);
    stack->addWidget(videoWidget);
    stack->setStyleSheet("background-color:rgb(0,0,0)");
    QHBoxLayout *ButtomLayout = new QHBoxLayout();
    QPushButton *addBtn = new QPushButton(tr("添加视频"));
    QPushButton *deleteBtn = new QPushButton(tr("删除视频"));

    connect(addBtn,SIGNAL(clicked(bool)),this,SLOT(videoFile()));
    connect(deleteBtn,SIGNAL(clicked(bool)),this,SLOT(deleteVideo()));

    //时钟
    lcdNumber = new QLCDNumber();
    lcdNumber->setDigitCount(19);
    lcdNumber->setSegmentStyle(QLCDNumber::Flat);
    Timer = new QTimer();
    Timer->setInterval(1000);
    Timer->start();
    QPalette icdPat = lcdNumber->palette();
    icdPat.setColor(QPalette::Normal,QPalette::WindowText,Qt::black);
    lcdNumber->setPalette(icdPat);

    ButtomLayout->addWidget(lcdNumber);
    ButtomLayout->addStretch();
    ButtomLayout->addWidget(addBtn);
    ButtomLayout->addWidget(deleteBtn);

    QVBoxLayout *mainRight = new QVBoxLayout();
    mainRight->addWidget(stack);
    mainRight->addWidget(mainRightFirstTop);
    mainRight->addLayout(ButtomLayout);

    connect(Timer, SIGNAL(timeout()), this, SLOT(onTimerOut()));

    QWidget *mainRightWidget = new QWidget();
    mainRightWidget->setLayout(mainRight);
    splitterRight->addWidget(mainRightWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(15);
    mainLayout->setSpacing(10);
    mainLayout->addWidget(splitterMain);
    mainLayout->setStretchFactor(splitterMain,1);
    mainLayout->setSizeConstraint(QLayout::SetDefaultConstraint);

    setMinimumSize(500,500);
    setWindowTitle("视频播放器");
}

void Video::openMusic()
{
    video->pause();
    this->hide();
    emit showMusic();
}

void Video::receiverSignal()
{
    this->show();
}

static QString Time(qint64 time)//转换时间
{
    qint64 seconds=time/1000;
    const qint64 minutes=seconds/60;
    seconds-=minutes*60;
    return QStringLiteral("%1:%2")
            .arg(minutes, 2, 10, QLatin1Char('0'))
            .arg(seconds, 2, 10, QLatin1Char('0'));
}

void Video::onTimerOut()
{
    lcdNumber->display(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}

void Video::playVideo()
{
    if(video->state() == QMediaPlayer::PlayingState)
    {
        video->pause();
        QIcon playIcon(":/images/image/play.png");
        play->setIconSize(QSize(32,32));
        play->setIcon(playIcon);
    }
    else
    {
        videoPlay();
    }
}

void Video::previousVideo()
{
    int currentIndex=playList->currentIndex();
    if(--currentIndex<0)
    {
        currentIndex=0;
    }
    videoList->setCurrentRow(currentIndex);
    playList->setCurrentIndex(currentIndex);
    videoPlay();
}

void Video::nextVideo()
{
    int currentIndex=playList->currentIndex();
//    musicList->setCurrentIndex(0);
    if(++currentIndex==playList->mediaCount())
    {
        currentIndex=0;
    }
    videoList->setCurrentRow(currentIndex);
    playList->setCurrentIndex(currentIndex);
    videoPlay();
}

void Video::videoPlay()
{
    video->play();
    QIcon pauseIcon(":/images/image/pause.png");
    play->setIconSize(QSize(32,32));
    play->setIcon(pauseIcon);
}

void Video::voiceVideo()
{
    if(voiceProgress->value()!=0)
    {
        currentVoice = voiceProgress->value();
        QIcon voiceIcon(":/images/image/silent.png");
        voice->setIcon(voiceIcon);
        voiceProgress->setValue(0);
    } else {
        QIcon voiceIcon(":/images/image/volume.png");
        voice->setIcon(voiceIcon);
        voiceProgress->setValue(currentVoice);
    }
    video->setVolume(voiceProgress->value());
}

void Video::upDatePosition(qint64 position)
{
    videoProgress->setValue(position);
    currentDuration->setText(Time(position));
}

void Video::upDateDuration(qint64 duration)
{
    videoProgress->setRange(0,duration);
    videoProgress->setEnabled(duration>0);
    videoProgress->setPageStep(duration/10000);
    videoDuration->setText(Time(duration));
}

void Video::setVideoPosition(int position)
{
    video->setPosition(position);
}

void Video::setVoice(int position)
{
    if(position == 0)
    {
        QIcon voiceIcon(":/images/image/silent.png");
        voice->setIcon(voiceIcon);
    } else {
        QIcon voiceIcon(":/images/image/volume.png");
        voice->setIcon(voiceIcon);
    }
    video->setVolume(position);
}

void Video::videoFile()//添加菜单  添加至所有音乐
{
    QSqlQuery query;
    query.exec("select * from VIDEO");
    QStringList list=QFileDialog::getOpenFileNames(this,tr("文件"),"D:/",tr("视频文件(*.wmv)"));
    if(!list.isEmpty())
    {
        QProgressDialog *progressdialog=new QProgressDialog(this);
        progressdialog->setWindowModality(Qt::WindowModal);
        progressdialog->setMinimumDuration(100);
        progressdialog->resize(500,200);
        progressdialog->setWindowTitle(tr("添加视频"));
        progressdialog->setLabelText(tr("添加中..."));
        progressdialog->setCancelButtonText(tr("关闭"));
        progressdialog->setRange(0,list.size()-1);
        for(int i=0;i<list.size();i++)
        {
            QListWidgetItem *item=new QListWidgetItem;
            item->setIcon(QIcon(":/image/image/cemera.png"));
            QString path=QDir::toNativeSeparators(list.at(i));
            progressdialog->setValue(i);
            if(i==list.size()-1)
            {
                progressdialog->close();
            }
            if(progressdialog->wasCanceled())
                progressdialog->close();
            if(!path.isEmpty())
            {
               playList->addMedia(QUrl::fromLocalFile(path));
               QString Name=path.split("\\").last();
               Name.remove(QString(".wmv"));
               item->setIcon(QIcon(":/images/image/cemera.png"));
               item->setText(QString("%1").arg(Name));
               videoList->addItem(item);
               videoList->addItem(item);
               query.exec(QString("insert into VIDEO values (%1,'%2','%3',%4)").arg(qrand()%10000).arg(Name).arg(path).arg(0));
            }
        }
    }
}

void Video::deleteVideo()
{
    table = new QSqlTableModel(this);
    if(video->playlist()==playList)
    {

        int Row=playList->currentIndex();
        int row=videoList->currentIndex().row();
        if(row<Row)
        {
            int Position;
            if(video->state()==QMediaPlayer::PlayingState)
            {
                Position=video->position();
            }
            playList->setCurrentIndex(0);
            videoList->takeItem(row);
            playList->removeMedia(row,row);
            table->setTable("VIDEO");
            table->select();
            table->removeRow(row);
            table->submitAll();
            video->setPlaylist(playList);
            playList->setCurrentIndex(Row-1);
            video->setPosition(Position);
            videoProgress->setValue(Position);
            video->play();
        }
        else
        {
            videoList->takeItem(row);
            playList->removeMedia(row,row);
            table->setTable("VIDEO");
            table->select();
            table->removeRow(row);
            table->submitAll();
        }
    }
    else
    {
        int row=videoList->currentIndex().row();
        videoList->takeItem(row);
        playList->removeMedia(row,row);
        table->setTable("VIDEO");
        table->select();
        table->removeRow(row);
        table->submitAll();
    }
}

void Video::doubleClick()
{
    video->setPlaylist(playList);
    playList->setCurrentIndex(videoList->currentIndex().row());
    video->play();
    QIcon pauseIcon(":/images/image/pause.png");
    play->setIconSize(QSize(32,32));
    play->setIcon(pauseIcon);
}

Video::~Video()
{
}
