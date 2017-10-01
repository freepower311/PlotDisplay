#ifndef DIALOGADDPOINT_H
#define DIALOGADDPOINT_H

#include <QDialog>
#include "qcustomplot.h"

namespace Ui {
class DialogAddPoint;
}

class DialogAddPoint : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddPoint(QWidget *parent = 0,QCustomPlot * plot = 0);
    ~DialogAddPoint();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::DialogAddPoint *ui;
    QCustomPlot * curPlot;
};

#endif // DIALOGADDPOINT_H
