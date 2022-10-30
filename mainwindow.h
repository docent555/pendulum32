#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include "rkn.h"
#include "widget.h"

class MainWindow : public QMainWindow
{
   Q_OBJECT
   public:
   explicit MainWindow(QWidget *parent = nullptr);
   ~MainWindow();

   protected:
   void closeEvent(QCloseEvent *) override;

   private:
   QThread *thread;
   Rkn *r;
   Widget *widget;

   signals:
   //    void freeEventLoop();

   public slots:
};

#endif // MAINWINDOW_H
