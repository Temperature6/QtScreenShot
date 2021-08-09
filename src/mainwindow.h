#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QJsonObject>
#include <QJsonArray>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QJsonObject config;
    QJsonArray items;
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
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_subject_list_customContextMenuRequested(const QPoint &pos);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
