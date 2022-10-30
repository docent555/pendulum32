#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <QMainWindow>
#include <QString>

class MyWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MyWindow(QWidget* parent = nullptr);

private:
    void closeEvent(QCloseEvent*);

signals:
    void writeRes(QString*);

public slots:
};

#endif // MYWINDOW_H
