#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class ImportDialog; }
QT_END_NAMESPACE

#include "importconfiguration.h"


class ImportDialog : public QDialog {
    Q_OBJECT

public:

    ImportDialog(QWidget *parent = nullptr);
    ~ImportDialog();

    ImportConfiguration getImportConfiguration();

public slots:

    void onGenerateNormalsCheckBoxChanged(int state);
    void onImportAccepted();

private:

    Ui::ImportDialog *ui;
    ImportConfiguration config;

};

#endif // IMPORTDIALOG_H
