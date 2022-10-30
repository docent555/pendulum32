#include "mywindow.h"

#include <QCloseEvent>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>

MyWindow::MyWindow(QWidget* parent)
    : QMainWindow(parent)
{
}

void MyWindow::closeEvent(QCloseEvent* event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question(this, "",
        tr("Закрыть?\n"),
        QMessageBox::No | QMessageBox::Yes,
        QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
        //        event->accept();
    } else {

        //        QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
        //                    "./", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        //        QString dir(".\\");

        //        emit writeRes(&dir);

        event->accept();
    }
}
