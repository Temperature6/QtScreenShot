#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QUrl>
#include <QFile>
#include <QSound>
#include <QScreen>
#include <QDateTime>
#include <QIODevice>
#include <windows.h>
#include <QArrayData>
#include <QJsonArray>
#include <QJsonValue>
#include <QCloseEvent>
#include <QStringList>
#include <QMessageBox>
#include <QJsonObject>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDesktopServices>
#include <qxtglobalshortcut.h>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":pic\\logo.png"));
    //Json部分
    //读文件
    QFile file("config.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString config_str = file.readAll();
    file.close();
    //判断错误
    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(config_str.toUtf8(),&parseJsonErr);
    if(!(parseJsonErr.error == QJsonParseError::NoError))
    {
        QMessageBox::critical(NULL, "错误", "解析json文件错误!", QMessageBox::Ok);
        return;
    }
    //设置初始化
    config = document.object();
    items = config["items"].toArray();
    for (int i = 0; i < items.count(); i++)
    {
        QString subject_ =items[i].toString();
        QDir dir;
        if (!dir.exists(QDir::currentPath() + "\\Picture\\" + subject_))
        {
             dir.mkpath(QDir::currentPath() + "\\Picture\\" + subject_);
        }
        ui->subject->addItem(subject_);
        ui->subject_list->addItem(subject_);
    }
    ui->file_type->addItems({"jpg", "png", "bmp"});
    ui->file_type->setCurrentText(config["filetype"].toString());
    ui->delay_value->setValue(config["sleep"].toDouble());
    ui->transit_value->setValue(config["transit"].toDouble());
    ui->isTop->setChecked(config["top"].toInt());
    SetWindowTop();
    ui->subject->setCurrentText(config["lastchoice"].toString());
    //全局快捷键
    QxtGlobalShortcut *shortcut = new QxtGlobalShortcut(this);
    shortcut->setShortcut(QKeySequence("Alt+S"));
    //槽函数链接
    connect(shortcut, &QxtGlobalShortcut::activated, this, &MainWindow::Shot);
    connect(ui->shot, &QPushButton::clicked, this, &MainWindow::Shot);
    connect(ui->open_dir, &QAction::triggered, this, &MainWindow::OpenWorkPath);
    connect(ui->subject_dir, &QAction::triggered, this, &MainWindow::OpenSubjectPath);
    connect(ui->isTop, &QCheckBox::clicked, this, &MainWindow::SetWindowTop);
    connect(ui->add_item, &QPushButton::clicked, this, &MainWindow::AddSubject);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Shot()
{
    float delay = (ui->delay_value->value() + ui->transit_value->value()) * 1000;
    QString file_type = ui->file_type->currentText();
    if (delay != 0)
    {
        this->hide();
        Sleep(delay);
    }
    QScreen *screen = QGuiApplication::primaryScreen();
    QString filePathName = QDir::currentPath() + "\\Picture\\" + ui->subject->currentText() + "\\";
    QDir dir;
    if (!dir.exists(filePathName))
    {
         dir.mkpath(filePathName);
    }
    filePathName += QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    filePathName += ".";
    filePathName += file_type;
    screen->grabWindow(0).save(filePathName);
    if (delay != 0)
    {
        this->show();
    }
    QSound::play("C:/Windows/Media/Windows Notify Email.wav");
    //QApplication::beep();
}

void MainWindow::OpenWorkPath()
{
    QString path = "file:";
    path += QDir::currentPath() + "\\Picture\\";
    QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
    return;
}

void MainWindow::OpenSubjectPath()
{
    QString path = "file:";
    path += QDir::currentPath() + "\\Picture\\" + ui->subject->currentText() + "\\";
    QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
    return;
}

void MainWindow::SetWindowTop()
{
    bool open = ui->isTop->isChecked();
    if (open)
    {
        Qt::WindowFlags m_flags = windowFlags();
        setWindowFlags(m_flags | Qt::WindowStaysOnTopHint);
        this->show();
    }
    else if (!open)
    {
        setWindowFlags(Qt::Widget);
        this->show();
    }
}

void MainWindow::on_subject_list_customContextMenuRequested(const QPoint &pos)
{
    //删除项目列表里的项目
    QListWidgetItem* curItem = ui->subject_list->itemAt( pos );
    if( curItem == NULL )
        return;
    QMenu *popMenu = new QMenu( this );
    QAction *deleteSeed = new QAction(tr("删除"), this);
    deleteSeed->setIcon(QIcon(":\\pic\\delete.png"));
    popMenu->addAction(deleteSeed);
    connect( deleteSeed, &QAction::triggered, this,&MainWindow::DeleteSubject);
    popMenu->exec( QCursor::pos());
}

void MainWindow::DeleteSubject()
{
    int ch = QMessageBox::warning(NULL, "警告", "您确定删除这一项吗?", QMessageBox::Yes|QMessageBox::No);
    if ( ch != QMessageBox::Yes )
        return;
    QListWidgetItem * item = ui->subject_list->currentItem();
    if( item == NULL )
        return;
    int curIndex = ui->subject_list->row(item);
    ui->subject_list->takeItem(curIndex);
    items.removeAt(curIndex);
    ui->subject->removeItem(curIndex);
    delete item;
}

void MainWindow::AddSubject()
{
    QString subject_name = ui->sunject_name->text();
    QString _str = subject_name;
    _str.remove(QRegExp("\\s"));
    if (!_str.isEmpty())
    {
        for (int i = 0; i < items.count(); i++)
        {
            if (items[i] == subject_name)
                return;
        }
        ui->subject_list->addItem(subject_name);
        ui->subject->addItem(subject_name);
        items.append(subject_name);

        ui->sunject_name->clear();
        return;
    }
    else
    {
        return;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    config["items"] = items;
    config["top"] = (int)ui->isTop->isChecked();
    config["lastchoice"] = ui->subject->currentText();
    config["sleep"] = ui->delay_value->value();
    config["transit"] = ui->transit_value->value();
    config["filetype"] = (QString)ui->file_type->currentText().toUtf8();

    QString config_str = QString(QJsonDocument(config).toJson(QJsonDocument::Compact));
    QFile file("config.json");
    file.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream file_stream(&file);
    file_stream.setCodec("utf-8");
    file_stream << config_str;
    file.close();
}
