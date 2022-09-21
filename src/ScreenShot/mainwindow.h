#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidgetItem>
#include <QMainWindow>
#include <QCloseEvent>
#include <QJsonObject>
#include <QJsonArray>
#include <QSystemTrayIcon>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum CloseAction
    {
        Ac_Exit = 0,
        Ac_Hide = 1
    };
    enum RemindType
    {
        Re_Audio = 0,
        Re_Message = 1
    };
    int argc = 0;
    QString argv[10];
    QString MARK = "item";
    QJsonObject config;
    QJsonArray items;
    QJsonArray work_path_list;
    QString new_one = ""; //新的文件的路径
    QString old_working_path;
    QString working_path = "\\Picture\\";
    int close_action = 0;
    QSystemTrayIcon* tray = new QSystemTrayIcon(this);
    bool remind = true;
    int notice_type_sel = 0;
    QString audio_path = ".\\audio.wav";
    bool background_run = false;
    bool error_start = false;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void Shot();
    void OpenWorkPath();
    void OpenSubjectPath();
    void EditOpenPath();
    void SetWindowTop();
    void DeleteSubject();
    void AddSubject();
    float dirFileSize(const QString &path);
    void ShowDirSize();
    void OpenNewOne();
    void AddWorkPath();
    void SwitchWorkingPath();
    void MoveDir(QString src_dir, QString dst_dir);
    void AskForMove(QString old_dir, QString new_dir);
    void MoveDirBtn();
    void EditCfgFile();
    void Restart();
    void Exit();
    void TrayActived(QSystemTrayIcon::ActivationReason Ac_code);
    void TrayClick();
    void TrayRightClick();
    void SwitchCloseAction();
    void ShowWindow();
    void SwitchRemindType();
    void SwitchNoticeRBtnState();
    void RenameItem();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_subject_list_customContextMenuRequested(const QPoint &pos);

    void on_subject_list_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
