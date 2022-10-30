#include "mainwindow.h"

#include <QCloseEvent>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
   r = new Rkn();
   widget = new Widget(r);
   thread = new QThread();

   this->setCentralWidget(widget);
   this->resize(1300, 600);

   // Запуск выполнения метода run будет осуществляться по сигналу запуска от соответствующего потока
   //   connect(thread, &QThread::started, r, &Rkn::calculate);
   connect(thread, SIGNAL(started()), r, SLOT(calculate()));
   // Остановка потока же будет выполняться по сигналу finished от соответствующего объекта в потоке
   //   connect(r, &Rkn::finished, thread, &QThread::terminate);
   connect(r, SIGNAL(finished()), thread, SLOT(terminate()));
   //    connect(r, &Rkn::finished, &thread, &QThread::deleteLater);

   //    // EventLoop для ожидания записи результата
   //    connect(this, SIGNAL(freeEventLoop()), &r->loop, SLOT(quit()));

   // Передаём объект r в поток
   r->moveToThread(thread);
   //    r->moveToThread(&thread);
   // Запускаем счет
   r->setStop(false);
   r->setCalculating(true);
   thread->start();
   //    thread.start();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
   QMessageBox::StandardButton resBtn = QMessageBox::question(this,
                                                              " ",
                                                              tr("Сохранить траектории?\n"),
                                                              QMessageBox::No | QMessageBox::Yes,
                                                              QMessageBox::No);
   if (resBtn != QMessageBox::Yes) {
      r->setCalculating(false);
      //        thread.wait(100);
      //        event->ignore();
      event->accept();
   } else {
      r->setStop(true);

      //      QString dir = QFileDialog::getExistingDirectory(this,
      //                                                      tr("Open Directory"),
      //                                                      "./",
      //                                                      QFileDialog::ShowDirsOnly
      //                                                          | QFileDialog::DontResolveSymlinks);
      QString fileName = QFileDialog::getSaveFileName(this,
                                                      tr("Save File"),
                                                      "results.dat",
                                                      tr("Data files (*.dat)"));
      double *z = r->getz();
      int ne = r->getNe();
      int nz = r->getnz();
      //      int *it = r->getit();
      double **th = r->get_theta();
      double **dth = r->get_dtheta();
      //      QString result, dir_a = dir, dir_k = dir;
      QString result;
      //      QString fileName_a = dir_a.append("\\").append("a.dat");
      QFile file_tr(fileName);

      if (file_tr.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
         for (int i = 0; i < nz; i++) {
            QTextStream(&result) << z[i] << '\t';
            for (int j = 0; j < ne; j++)
               QTextStream(&result) << th[i][j] << '\t';
            for (int j = 0; j < ne; j++)
               QTextStream(&result) << dth[i][j] << '\t';
            QTextStream(&result) << '\n';
         }
         file_tr.write(result.toUtf8());
         file_tr.close();
      } else {
         qDebug() << "\nError of file open: " << fileName.toUtf8();
      }

      file_tr.close();

      //      QString fileName_k = dir_k.append("\\").append("eff.dat");
      //      QFile file_k(fileName_k);
      //      result.clear();

      //      if (file_k.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
      //         for (int i = 0; i <= *it; i++)
      //            //            QTextStream(&result) << k[i] << '\n';
      //            file_k.write(result.toUtf8());
      //         file_k.close();
      //      } else {
      //         qDebug() << "\nError of file open: " << fileName_k.toUtf8();
      //      }

      //      file_k.close();

      qDebug() << "\nResults recorded!";

      //        emit freeEventLoop();
      r->setCalculating(false);
      //        thread.wait(100);
      event->accept();
   }
}

MainWindow::~MainWindow() {}
