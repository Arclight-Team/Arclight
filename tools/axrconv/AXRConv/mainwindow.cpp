#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "importdialog.h"
#include "entitytreemodel.h"

#include <QtConcurrent/QtConcurrent>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){

    ui->setupUi(this);

    statusLabel = new QLabel(this);
    statusLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    statusLabel->setText(tr("Ready"));

    entityTree = new EntityTreeModel(this);

    ui->entityTreeView->setModel(entityTree);
    ui->statusbar->addWidget(statusLabel);

    renderTimer = new QTimer(this);

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpen);
    connect(&watcher, &QFutureWatcher<void>::finished, this, &MainWindow::onTaskFinished);
    connect(renderTimer, SIGNAL(timeout()), ui->renderWidget, SLOT(update()));

    renderTimer->start(17);

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

        ui->renderWidget->loadModel(&model);

        if(ui->renderWidget->errorOccured()){
            showRenderError();
        }

    }else{

        statusLabel->setText(tr("Import failed for ") + filename);
        QMessageBox::critical(this, tr("Import failed"), tr("Failed to import file ") + filename + ".\n" + importer.getErrorString(), QMessageBox::Ok);

    }

}



void MainWindow::showRenderError(){
    QMessageBox::warning(this, tr("Render preview error"), ui->renderWidget->getErrorMessage());
}



void MainWindow::closeEvent(QCloseEvent *event){

    watcher.waitForFinished();
    QMainWindow::closeEvent(event);

}



MainWindow::~MainWindow(){
    delete ui;
}

