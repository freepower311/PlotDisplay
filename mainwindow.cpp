#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    srand(QDateTime::currentDateTime().toTime_t());
    ui->setupUi(this);
    ui->plotDisplay->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);
    ui->plotDisplay->axisRect()->setupFullAxesBox();
    ui->plotDisplay->xAxis->setLabel("x Axis");
    ui->plotDisplay->yAxis->setLabel("y Axis");
    ui->plotDisplay->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    ui->plotDisplay->legend->setFont(legendFont);
    ui->plotDisplay->legend->setSelectedFont(legendFont);
    ui->plotDisplay->legend->setSelectableParts(QCPLegend::spItems);

    // connect slot that ties some axis selections together (especially opposite axes):
    connect(ui->plotDisplay, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    connect(ui->plotDisplay, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(ui->plotDisplay, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    // make bottom and left axes transfer their ranges to top and right axes:
    connect(ui->plotDisplay->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->plotDisplay->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->plotDisplay->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->plotDisplay->yAxis2, SLOT(setRange(QCPRange)));

    // setup policy and connect slot for context menu popup:
    ui->plotDisplay->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->plotDisplay, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
}

void MainWindow::selectionChanged()
{
  /*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.

   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.

   Further, we want to synchronize the selection of the graphs with the selection state of the respective
   legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
   or on its legend item.
  */

  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->plotDisplay->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->plotDisplay->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->plotDisplay->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->plotDisplay->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->plotDisplay->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->plotDisplay->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->plotDisplay->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->plotDisplay->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->plotDisplay->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->plotDisplay->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->plotDisplay->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->plotDisplay->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }

  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i<ui->plotDisplay->graphCount(); ++i)
  {
    QCPGraph *graph = ui->plotDisplay->graph(i);
    QCPPlottableLegendItem *item = ui->plotDisplay->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      item->setSelected(true);
      graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
    }
  }
}

void MainWindow::mouseWheel()
{
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed

    if (ui->plotDisplay->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->plotDisplay->axisRect()->setRangeZoom(ui->plotDisplay->xAxis->orientation());
    else if (ui->plotDisplay->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->plotDisplay->axisRect()->setRangeZoom(ui->plotDisplay->yAxis->orientation());
    else
        ui->plotDisplay->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::contextMenuRequest(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->addAction("Add random graph", this, SLOT(addRandomGraph()));
    menu->addAction("Add graph from file", this, SLOT(addGraphFromFile()));
    if (ui->plotDisplay->graphCount() > 0)
    {
        menu->addAction("Remove graph", this, SLOT(removeGraph()));
        menu->addAction("Add point", this, SLOT(addPointToGraphDialog()));
    }
    menu->popup(ui->plotDisplay->mapToGlobal(pos));
}

void MainWindow::addRandomGraph()
{
    removeGraph();
    int n = 50; // number of points in graph
    double xScale = (rand()/(double)RAND_MAX + 0.5)*2;
    double yScale = (rand()/(double)RAND_MAX + 0.5)*2;
    double xOffset = (rand()/(double)RAND_MAX - 0.5)*4;
    double yOffset = (rand()/(double)RAND_MAX - 0.5)*10;
    double r1 = (rand()/(double)RAND_MAX - 0.5)*2;
    double r2 = (rand()/(double)RAND_MAX - 0.5)*2;
    double r3 = (rand()/(double)RAND_MAX - 0.5)*2;
    double r4 = (rand()/(double)RAND_MAX - 0.5)*2;
    QVector<double> x(n), y(n);
    for (int i=0; i<n; i++)
    {
        x[i] = (i/(double)n-0.5)*10.0*xScale + xOffset;
        y[i] = (qSin(x[i]*r1*5)*qSin(qCos(x[i]*r2)*r4*3)+r3*qCos(qSin(x[i])*r4*2))*yScale + yOffset;
    }

    ui->plotDisplay->addGraph();
    ui->plotDisplay->graph()->setName(QString("New graph %1").arg(ui->plotDisplay->graphCount()-1));
    ui->plotDisplay->graph()->setData(x, y);
    ui->plotDisplay->graph()->setLineStyle((QCPGraph::LineStyle)(1));
    ui->plotDisplay->graph()->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)(1)));
    QPen graphPen;
    graphPen.setColor(QColor(0, 0, 0));
    graphPen.setWidthF(1);
    ui->plotDisplay->graph()->setPen(graphPen);
    ui->plotDisplay->replot();
}

void MainWindow::addGraphFromFile()
{
    QMessageBox::about(this,"TODO","Not available in this version");
    return;
    //Read
    //Parse
    //Visualize
}

void MainWindow::addPointToGraphDialog()
{
    DialogAddPoint *dialog = new DialogAddPoint(this,ui->plotDisplay);
    dialog->show();
}

void MainWindow::removeGraph()
{
  ui->plotDisplay->clearGraphs();
  ui->plotDisplay->replot();
}

MainWindow::~MainWindow()
{
    delete ui;
}
