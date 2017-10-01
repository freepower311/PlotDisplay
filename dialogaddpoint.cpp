#include "dialogaddpoint.h"
#include "ui_dialogaddpoint.h"

DialogAddPoint::DialogAddPoint(QWidget *parent,QCustomPlot * plot) :
    QDialog(parent),
    ui(new Ui::DialogAddPoint)
{
    ui->setupUi(this);
    curPlot = plot;
}

DialogAddPoint::~DialogAddPoint()
{
    delete ui;
}


void DialogAddPoint::on_buttonBox_clicked(QAbstractButton *button)
{
    if((QPushButton *)button== ui->buttonBox->button(QDialogButtonBox::Ok)
            && curPlot != 0
            && curPlot->graphCount() > 0){
        QVector <double> x(1),y(1);
        x[0] = ui->lineEdit->text().toDouble();
        y[0] = ui->lineEdit_2->text().toDouble();
        curPlot->graph()->addData(x,y);
        curPlot->replot();
    }
}
