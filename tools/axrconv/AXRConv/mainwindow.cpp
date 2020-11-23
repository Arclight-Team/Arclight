#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "importdialog.h"
#include "entitytreemodel.h"

#include <QtConcurrent/QtConcurrent>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){

    ui->setupUi(this);

    statusLabel = new QLabel(this);
    statusLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    statusLabel->setText(tr("Ready"));

    entityTree = new EntityTreeModel();

    ui->entityTreeView->setModel(entityTree);
    ui->statusbar->addWidget(statusLabel);

    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(onOpen()));
    connect(&watcher, &QFutureWatcher<void>::finished, this, &MainWindow::onTaskFinished);

}




void MainWindow::onOpen(){

    openedFilename = QFileDialog::getOpenFileName(this, tr("Open 3D model file"), QString(), tr("Supported files (*.fbx *.dae *.obj);;All files (*.*)"));

    if(!openedFilename.isEmpty()){

        ImportDialog dialog(this);
        int result = dialog.exec();

        if(result == QDialog::Accepted){

            ImportConfiguration config = dialog.getImportConfiguration();

            watcher.setFuture(QtConcurrent::run([this, &config](){
                return importer.import(openedFilename, config);
            }));

        }

    }

}



void MainWindow::onTaskFinished(){

    model = watcher.result();

    QFileInfo info(openedFilename);
    QString filename = info.fileName();

    if(importer.validImport()){

        statusLabel->setText(tr("Imported ") + filename);
        entityTree->setEntityTree(model);
        ui->renderWidget->setModel(&model);

    }else{

        statusLabel->setText(tr("Import failed for ") + filename);
        QMessageBox::critical(this, tr("Import failed"), tr("Failed to import file ") + filename + ".\n" + importer.getErrorString(), QMessageBox::Ok);

    }

}



void MainWindow::closeEvent(QCloseEvent *event){

    watcher.waitForFinished();
    ui->renderWidget->reset();
    QMainWindow::closeEvent(event);

}



MainWindow::~MainWindow(){
    delete ui;
    delete entityTree;
}

