#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFutureWatcher>

#include "importer.h"
#include "amdmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class QLabel;
class QCloseEvent;
class EntityTreeModel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    virtual void closeEvent(QCloseEvent *event) override;

private slots:

    void onOpen();
    void onTaskFinished();

private:

    Ui::MainWindow *ui;
    QString openedFilename;
    QLabel* statusLabel;
    QFutureWatcher<AMDModel> watcher;

    EntityTreeModel* entityTree;
    Importer importer;
    AMDModel model;

};
#endif // MAINWINDOW_H
