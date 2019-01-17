#include "dialog.h"
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

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    playList = new QMediaPlaylist(this);
    music = new QMediaPlayer(this);

    playList->setPlaybackMode(QMediaPlaylist::Loop);

    //堆栈界面一
    QGridLayout *ButtonLayout = new QGridLayout();

    musicProgress = new QSlider(this);
    musicProgress->setOrientation(Qt::Horizontal);
    musicProgress->setMaximum(100);
    musicProgress->setMinimum(0);

    QHBoxLayout *MButtonLayout = new QHBoxLayout();
    currentDuration = new QLabel(tr("00:00"));
    musicDuration =new QLabel(tr("00:00"));
    MButtonLayout->addWidget(currentDuration);
    MButtonLayout->addWidget(musicProgress);
    MButtonLayout->addWidget(musicDuration);
    ButtonLayout->addLayout(MButtonLayout,0,0,1,6);

    voiceProgress = new QSlider(this);
    voiceProgress->setOrientation(Qt::Horizontal);
    voiceProgress->setMaximum(100);
    voiceProgress->setMinimum(0);
    voiceProgress->setValue(30);

    connect(music,SIGNAL(positionChanged(qint64)),this,SLOT(upDatePosition(qint64)));
    connect(music,SIGNAL(durationChanged(qint64)),this,SLOT(upDateDuration(qint64)));
    connect(musicProgress,SIGNAL(sliderMoved(int)),this,SLOT(setMusicPosition(int)));
    connect(voiceProgress,SIGNAL(sliderMoved(int)),this,SLOT(setVoice(int)));

    play = new QPushButton();
    next = new QPushButton();
    last = new QPushButton();
    model = new QPushButton();
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

    QIcon modelIcon(":/images/image/cycle.png");
    model->setIconSize(QSize(32,32));
    model->setFlat(true);
    model->setIcon(modelIcon);

    QIcon voiceIcon(":/images/image/volume.png");
    voice->setIconSize(QSize(32,32));
    voice->setFlat(true);
    voice->setIcon(voiceIcon);

    connect(play,SIGNAL(clicked(bool)),this,SLOT(playMusic()));
    connect(model,SIGNAL(clicked(bool)),this,SLOT(modelMusic()));
    connect(last,SIGNAL(clicked(bool)),this,SLOT(previousMusic()));
    connect(next,SIGNAL(clicked(bool)),this,SLOT(nextMusic()));
    connect(voice,SIGNAL(clicked(bool)),this,SLOT(voiceMusic()));

    video = new QPushButton();
    QIcon videoIcon(":/images/image/video.png");
    video->setIconSize(QSize(48,48));
    video->setFlat(true);
    video->setIcon(videoIcon);
    connect(video,SIGNAL(clicked(bool)),this,SLOT(openVideo()));

    QHBoxLayout *HButtonLayout = new QHBoxLayout();
    HButtonLayout->addWidget(last);
    HButtonLayout->addWidget(play);
    HButtonLayout->addWidget(next);
    HButtonLayout->addStretch();
    HButtonLayout->addWidget(model);
    HButtonLayout->addWidget(voice);
    HButtonLayout->addWidget(voiceProgress);
    HButtonLayout->addWidget(video);

    ButtonLayout->addLayout(HButtonLayout,1,0,1,6);
    QGridLayout *mainRightLayout = new QGridLayout(this);
    mainRightLayout->addLayout(ButtonLayout,0,0);

    //主分割窗口
    QSplitter *splitterMain = new QSplitter(Qt::Vertical,0);
    musicList = new QListWidget();

    query.exec("select * from MUSIC");
    while(query.next())
    {
        QString Name=query.value(1).toString();
        QString FileName=query.value(2).toString();
        if(Name!=""&&FileName!="")
        {
            QListWidgetItem *item=new QListWidgetItem;
            item->setIcon(QIcon(":/images/image/music.png"));
            item->setText(Name);
            musicList->addItem(item);
            playList->addMedia(QUrl::fromLocalFile(FileName));
        }
    }
    splitterMain->addWidget(musicList);
    connect(musicList,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(doubleClick()));

    //右部分窗口
    QSplitter *splitterRight = new QSplitter(Qt::Vertical,splitterMain);
    splitterRight->setOpaqueResize(false);

    QWidget *mainRightFirstTop = new QWidget();
    mainRightFirstTop->setLayout(mainRightLayout);

    QHBoxLayout *ButtomLayout = new QHBoxLayout();
    QPushButton *addBtn = new QPushButton(tr("添加歌曲"));
    QPushButton *deleteBtn = new QPushButton(tr("删除歌曲"));

    connect(addBtn,SIGNAL(clicked(bool)),this,SLOT(musicFile()));
    connect(deleteBtn,SIGNAL(clicked(bool)),this,SLOT(deleteMusic()));

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

    setWindowTitle("音乐播放器");
}

void Dialog::openVideo()
{
    music->pause();
    this->hide();
    emit showVideo();
}

void Dialog::receiverSignal()
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

void Dialog::onTimerOut()
{
    lcdNumber->display(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}

void Dialog::playMusic()
{
    if(music->state() == QMediaPlayer::PlayingState)
    {
        music->pause();
        QIcon playIcon(":/images/image/play.png");
        play->setIconSize(QSize(32,32));
        play->setIcon(playIcon);
    }
    else
    {
        musicPlay();
    }
}

void Dialog::previousMusic()
{
    int currentIndex=playList->currentIndex();
    if(playList->playbackMode()==QMediaPlaylist::Loop)
    {
        if(--currentIndex<0)
        {
            currentIndex=0;
        }
    }
    else if(playList->playbackMode()==QMediaPlaylist::Random)
    {
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        currentIndex =qrand()%playList->mediaCount();
    }
    playList->setCurrentIndex(currentIndex);
    musicList->setCurrentRow(currentIndex);
    musicPlay();
}

void Dialog::nextMusic()
{
    int currentIndex=playList->currentIndex();
    if(playList->playbackMode()==QMediaPlaylist::Loop)
    {
        if(++currentIndex==playList->mediaCount())
        {
            currentIndex=0;
        }
    }
    else if(playList->playbackMode()==QMediaPlaylist::Random)
    {
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        currentIndex =qrand()%playList->mediaCount();
    }
    musicList->setCurrentRow(currentIndex);
    playList->setCurrentIndex(currentIndex);
    musicPlay();
}

void Dialog::musicPlay()
{
    music->play();
    QIcon pauseIcon(":/images/image/pause.png");
    play->setIconSize(QSize(32,32));
    play->setIcon(pauseIcon);
}

void Dialog::voiceMusic()
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
    music->setVolume(voiceProgress->value());
}

void Dialog::modelMusic()
{
    if(playList->playbackMode()==QMediaPlaylist::Loop)
    {
        model->setIcon(QIcon(":/images/image/random.png"));
        model->setToolTip(tr("随机播放"));
        playList->setPlaybackMode(QMediaPlaylist::Random);
    }
    else if(playList->playbackMode()==QMediaPlaylist::Random)
    {
        model->setIcon(QIcon(":/images/image/single.png"));
        model->setToolTip(tr("单曲循环"));
        playList->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    }
    else if(playList->playbackMode()==QMediaPlaylist::CurrentItemInLoop)
    {
        model->setIcon(QIcon(":/images/image/cycle.png"));
        model->setToolTip(tr("顺序播放"));
        playList->setPlaybackMode(QMediaPlaylist::Loop);
    }
}

void Dialog::upDatePosition(qint64 position)
{
    musicProgress->setValue(position);
    currentDuration->setText(Time(position));
}

void Dialog::upDateDuration(qint64 duration)
{
    musicProgress->setRange(0,duration);
    musicProgress->setEnabled(duration>0);
    musicProgress->setPageStep(duration/10000);
    musicDuration->setText(Time(duration));
}

void Dialog::setMusicPosition(int position)
{
    music->setPosition(position);
}

void Dialog::setVoice(int position)
{
    if(position == 0)
    {
        QIcon voiceIcon(":/images/image/silent.png");
        voice->setIcon(voiceIcon);
    } else {
        QIcon voiceIcon(":/images/image/volume.png");
        voice->setIcon(voiceIcon);
    }
    music->setVolume(position);
}

void Dialog::musicFile()//添加菜单  添加至所有音乐
{
    QSqlQuery query;
    query.exec("select * from MUSIC");
    QStringList list=QFileDialog::getOpenFileNames(this,tr("文件"),"D:/",tr("音频文件(*.mp3)"));
    if(!list.isEmpty())
    {
        QProgressDialog *progressdialog=new QProgressDialog(this);
        progressdialog->setWindowModality(Qt::WindowModal);
        progressdialog->setMinimumDuration(100);
        progressdialog->resize(500,200);
        progressdialog->setWindowTitle(tr("添加歌曲"));
        progressdialog->setLabelText(tr("添加中..."));
        progressdialog->setCancelButtonText(tr("关闭"));
        progressdialog->setRange(0,list.size()-1);
        for(int i=0;i<list.size();i++)
        {
            QListWidgetItem *item=new QListWidgetItem;
            item->setIcon(QIcon(":/image/image/Music_32px_1144946_easyicon.net.png"));
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
               Name.remove(QString(".mp3"));
               item->setIcon(QIcon(":/images/image/music.png"));
               item->setText(QString("%1").arg(Name));
               musicList->addItem(item);
               musicList->addItem(item);
               query.exec(QString("insert into MUSIC values (%1,'%2','%3',%4)").arg(qrand()%10000).arg(Name).arg(path).arg(0));
            }
        }
    }
}

void Dialog::deleteMusic()
{
    table = new QSqlTableModel(this);
    if(music->playlist()==playList)
    {

        int Row=playList->currentIndex();
        int row=musicList->currentIndex().row();
        if(row<Row)
        {
            int Position;
            if(music->state()==QMediaPlayer::PlayingState)
            {
                Position=music->position();
            }
            playList->setCurrentIndex(0);
            musicList->takeItem(row);
            playList->removeMedia(row,row);
            table->setTable("MUSIC");
            table->select();
            table->removeRow(row);
            table->submitAll();
            music->setPlaylist(playList);
            playList->setCurrentIndex(Row-1);
            music->setPosition(Position);
            musicProgress->setValue(Position);
            music->play();
        }
        else
        {
            musicList->takeItem(row);
            playList->removeMedia(row,row);
            table->setTable("MUSIC");
            table->select();
            table->removeRow(row);
            table->submitAll();
        }
    }
    else
    {
        int row=musicList->currentIndex().row();
        musicList->takeItem(row);
        playList->removeMedia(row,row);
        table->setTable("MUSIC");
        table->select();
        table->removeRow(row);
        table->submitAll();
    }
}

void Dialog::doubleClick()
{
    music->setPlaylist(playList);
    playList->setCurrentIndex(musicList->currentIndex().row());
    music->play();
    QIcon pauseIcon(":/images/image/pause.png");
    play->setIconSize(QSize(32,32));
    play->setIcon(pauseIcon);
}

Dialog::~Dialog()
{
}
