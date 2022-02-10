#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QUrl>
#include <QFile>
#include <QDebug>
#include <QSound>
#include <QScreen>
#include <QLibrary>
#include <QProcess>
#include <QDateTime>
#include <QIODevice>
#include <windows.h>
#include <QArrayData>
#include <QJsonArray>
#include <QJsonValue>
#include <QFileDialog>
#include <QCloseEvent>
#include <QStringList>
#include <QMessageBox>
#include <QJsonObject>
#include <QTextStream>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDesktopServices>
#include <qxtglobalshortcut.h>
#include "pch.h"

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
        working_path = QDir::currentPath() + "/Picture";
    }
    //设置初始化
    config = document.object();

    work_path_list = config["work_path_list"].toArray();
    for (int j = 0; j < work_path_list.count(); j++)
    {
        if (work_path_list[j] != "")
        {
            ui->select_work_dir->addItem(work_path_list[j].toString());
        }
    }
    working_path = config["working_dir"].toString();
    ui->select_work_dir->setCurrentText(working_path);

    if (working_path == "<Default>" || working_path.isEmpty())
    {
        working_path = QDir::currentPath() + "/Picture";
        old_working_path = working_path;
    }

    items = config["items"].toArray();
    for (int i = 0; i < items.count(); i++)
    {
        QString subject_ =items[i].toString();
        QDir dir;
        QString path = working_path + "/" + subject_;
        if (!dir.exists(path))
        {
            dir.mkpath(path);
        }
        QFile sub_file(path + "/" + MARK);
        if (!sub_file.exists())
        {
            QFile file(path + "/" + MARK);
            file.open(QIODevice::WriteOnly|QIODevice::Text);
            file.close();
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
    ui->statusBar->showMessage("欢迎", 5000);

    ui->statusBar->setStyleSheet("QStatusBar{background-color:#3a9aeb;}"); //状态栏颜色
    old_working_path = working_path;
    //全局快捷键
    QxtGlobalShortcut *shortcut = new QxtGlobalShortcut(this);
    shortcut->setShortcut(QKeySequence("Alt+S"));
    //槽函数链接
    connect(shortcut, &QxtGlobalShortcut::activated, this, &MainWindow::Shot);
    connect(ui->shot, &QPushButton::clicked, this, &MainWindow::Shot);
    connect(ui->menu_shot, &QAction::triggered, this, &MainWindow::Shot);
    connect(ui->open_dir, &QAction::triggered, this, &MainWindow::OpenWorkPath);
    connect(ui->subject_dir, &QAction::triggered, this, &MainWindow::OpenSubjectPath);
    connect(ui->isTop, &QCheckBox::clicked, this, &MainWindow::SetWindowTop);
    connect(ui->add_item, &QPushButton::clicked, this, &MainWindow::AddSubject);
    connect(ui->subject, &QComboBox::currentTextChanged, this, &MainWindow::ShowDirSize);
    connect(ui->show_size, &QPushButton::clicked, this, &MainWindow::ShowDirSize);
    connect(ui->open_new, &QAction::triggered, this, &MainWindow::OpenNewOne);
    connect(ui->select_work_dir, &QComboBox::currentTextChanged, this, &MainWindow::SwitchWorkingPath);
    connect(ui->add_work_dir, &QPushButton::clicked, this, &MainWindow::AddWorkPath);
    connect(ui->move_file, &QPushButton::clicked, this, &MainWindow::MoveDirBtn);
    connect(ui->edit_cfg_file, &QPushButton::clicked, this, &MainWindow::EditCfgFile);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Shot()
{
    if (working_path.isEmpty())
    {
        working_path = QDir::currentPath() + "/Picture";
    }
    if (ui->subject->currentText().isEmpty())
    {
        QMessageBox::critical(this, "错误", "没有可用的项目, 请先新建一个项目。");
        return;
    }
    float delay = (ui->delay_value->value() + ui->transit_value->value()) * 1000;
    QString file_type = ui->file_type->currentText();
    if (delay != 0)
    {
        this->hide();
        Sleep(delay);
    }
    QScreen *screen = QGuiApplication::primaryScreen();
    QString filePathName = working_path + "/" + ui->subject->currentText() + "/";
    QDir dir;
    if (!dir.exists(filePathName))
    {
         dir.mkpath(filePathName);
    }
    QFileInfo sub_file(filePathName + MARK);
    if (!sub_file.exists())
    {
        QFile file(filePathName + MARK);
        file.open(QIODevice::WriteOnly|QIODevice::Text);
        file.close();
    }
    filePathName += QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    filePathName += ".";
    filePathName += file_type;
    new_one = filePathName;
    QString notice = ""; //用于进行状态栏提醒的字符串
    if (!screen->grabWindow(0).save(filePathName))
    {
        notice += "无法保存截图";
        ui->statusBar->showMessage(notice, 5000);
        if (delay != 0)
        {
            this->show();
        }
        return;
    }
    else
    {
        QApplication::alert(this, 1000);
        notice += "成功保存到 \"" + filePathName + "\"";
        ui->statusBar->showMessage(notice, 5000);
        if (delay != 0)
        {
            this->show();
        }
        QSound::play(":/audio.wav");
    }
    return;
}

void MainWindow::OpenWorkPath()
{
    QString path = "file:";
    path += working_path;
    QDesktopServices::openUrl(QUrl::fromLocalFile(working_path));
    return;
}

void MainWindow::OpenSubjectPath()
{
    if (ui->subject->currentText().isEmpty())
    {
        QMessageBox::critical(this, "错误", "没有可用的项目, 请先新建一个项目。");
        return;
    }
    QString path = "file:";
    path += working_path + "/" + ui->subject->currentText() + "/";
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
    return;
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
    delete popMenu;
    delete deleteSeed;
    return;
}

void MainWindow::DeleteSubject()
{
    int ch = QMessageBox::warning(this, "警告", "您确定删除这一项吗?", QMessageBox::Yes|QMessageBox::No);
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
    return;
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
        ui->subject_list->scrollToBottom();
        ui->sunject_name->clear();
        return;
    }
    else
    {
        return;
    }
    return;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    config["items"] = items;
    config["top"] = (int)ui->isTop->isChecked();
    config["lastchoice"] = ui->subject->currentText();
    config["sleep"] = ui->delay_value->value();
    config["transit"] = ui->transit_value->value();
    config["filetype"] = (QString)ui->file_type->currentText().toUtf8();
    //work_path_list.removeAt(0); //移除“<Default>”
    for (int i = 0; i < work_path_list.count(); i++)
    {
        if (work_path_list[i] == "")
        {
            work_path_list.removeAt(i);
        }
    }
    config["work_path_list"] = work_path_list;

    if (ui->select_work_dir->currentText() == "<Default>")
    {
        config["working_dir"] = QDir::currentPath() + "/Picture";
    }
    else
    {
        config["working_dir"] = working_path;
    }

    QString config_str = QString(QJsonDocument(config).toJson(QJsonDocument::Indented));
    QFile file("config.json");
    file.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream file_stream(&file);
    file_stream.setCodec("utf-8");
    file_stream << config_str;
    file.close();
    return;
}

float MainWindow::dirFileSize(const QString &path)
{
    QDir dir(path);
    float size = 0;
    foreach(QFileInfo fileInfo, dir.entryInfoList(QDir::Files))
    {
    //计算文件大小
        size += fileInfo.size();
    }
    foreach(QString subDir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
    //若存在子目录，则递归调用dirFileSize()函数
        size += dirFileSize(path + QDir::separator() + subDir);
    }
    return size;
}

void MainWindow::ShowDirSize()
{
    float size = dirFileSize(working_path + "/" + ui->subject->currentText());
    QString str_size = QString::asprintf("%.2f", size / pow(1024, 2));
    QString notice = "\"" + ui->subject->currentText() + "\"" + "文件夹大小为" + str_size + "MB";
    ui->statusBar->showMessage(notice, 5000);
    return;
}

void MainWindow::OpenNewOne()
{
    if (new_one == "")
    {
        ui->statusBar->showMessage("还没有新的截图", 2000);
        return;
    }
    QString path = "file:";
    path += new_one;
    QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
    return;
}

void MainWindow::AddWorkPath()
{
    QString new_path = QFileDialog::getExistingDirectory(this, "选择新的工作目录", working_path);
    if (!new_path.isEmpty())
    {
        work_path_list.append(new_path);
        ui->select_work_dir->addItem(new_path);
        ui->statusBar->showMessage("添加了工作目录:“ " + new_path + " ”", 2000);
    }
    else
    {
        ui->statusBar->showMessage("用户取消了操作", 2000);
    }
    return;
}

void MainWindow::SwitchWorkingPath()
{
    working_path = ui->select_work_dir->currentText();
    if (working_path == "<Default>")
    {
        working_path = QDir::currentPath() + "/Picture";
        ui->statusBar->showMessage("新的截图将保存到:“" + working_path + "”", 5000);
    }
    if (old_working_path != working_path)
    {
        AskForMove(old_working_path, working_path);
        old_working_path = working_path;
    }

    return;
}

void MainWindow::MoveDir(QString src_dir, QString dst_dir)
{
    QByteArray tmp_src = src_dir.toLocal8Bit();
    QByteArray tmp_dst = dst_dir.toLocal8Bit();
    QStringList sub_list;
    QDir dir(src_dir);
    dir.setFilter(QDir::Dirs); //只遍历文件夹
    QFileInfoList temp_sub_list = dir.entryInfoList();
    ui->statusBar->showMessage("正在迁移文件,请耐心等待......");
    for (int i = 0; i < temp_sub_list.count(); i++)
    {
        QFileInfo curr_info = temp_sub_list.at(i);
        if (curr_info.fileName() == "." || curr_info.fileName() == "..")
        {
            continue;
        }
        else
        {
            QString sub_dir_path = curr_info.absoluteFilePath();
            QFileInfo sub_file(sub_dir_path + "/" + MARK);
            if (sub_file.exists())
            {
                QString src = sub_dir_path;
                QString dst = sub_dir_path.replace(src_dir, dst_dir);
                MoveDirectory(src.toLocal8Bit().data(), dst.toLocal8Bit().data());
            }
        }
    }
    ui->statusBar->showMessage("移动完成", 2000);
    return;
}

void MainWindow::AskForMove(QString old_dir, QString new_dir)
{
    QString tips = "图像保存目录从:\n\"%1\"\n切换到了:\n\"%2\"\n是否将旧目录中的文件迁移到新的目录?";
    tips = tips.arg(old_dir, new_dir);
    QMessageBox::StandardButton ans = QMessageBox::question(this, "移动文件", tips);

    if (ans == QMessageBox::Yes)
    {
        MoveDir(old_dir, new_dir);
        return;
    }
    else
    {
        return;
    }

}

void MainWindow::MoveDirBtn()
{
    QString src = QFileDialog::getExistingDirectory(this,
                                                    "选择源工作目录",
                                                    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    if (src.isEmpty()) return;
    QString dst = QFileDialog::getExistingDirectory(this,
                                                    "选择目标工作目录",
                                                    ui->select_work_dir->currentText());
    if (dst.isEmpty()) return;
    if (src == "" || dst == "")
    {
        return;
    }
    else
    {
        QString tips = "确定从:\n\"%1\"\n移动到:\n\"%2\" 吗?";
        tips = tips.arg(src, dst);
        QMessageBox::StandardButton ans = QMessageBox::question(this, "移动文件", tips);

        if (ans == QMessageBox::Yes)
        {
            MoveDir(src, dst);
            return;
        }
        else
        {
            return;
        }
    }
}

void MainWindow::on_subject_list_itemDoubleClicked(QListWidgetItem *item)
{
    if( item == NULL )
        return;
    QString select_item_path = working_path + "/" + item->text();
    QDir dir = select_item_path;
    if (!dir.exists())
    {
        dir.mkdir(select_item_path);
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(select_item_path));
    return;
}

void MainWindow::EditCfgFile()
{
    QMessageBox::StandardButton ans = QMessageBox::question(this, "需要关闭", "编辑配置文件需要退出程序,是否继续?");
    if (ans == QMessageBox::Yes)
    {
        QString cfg_path = QDir::currentPath() + "/config.json";
        QStringList arg = { cfg_path };
        QProcess::startDetached("C:\\Windows\\system32\\notepad.exe", arg);
        this->close();
    }
    else
    {
        return;
    }
}
