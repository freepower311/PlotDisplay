#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QInputDialog>
#include "qcustomplot.h"
#include "dialogaddpoint.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void mouseWheel();
    void contextMenuRequest(QPoint pos);
    void addRandomGraph();
    void addPointToGraphDialog();
    void removeGraph();
    void addGraphFromFile();
    void selectionChanged();
};

#endif // MAINWINDOW_H
