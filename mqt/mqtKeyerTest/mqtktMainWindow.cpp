#include "mqtktMainWindow.h"
#include "ui_mqtktMainWindow.h"

mqtktMainWindow::mqtktMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mqtktMainWindow)
{
    ui->setupUi(this);

    QLineSeries *series = new QLineSeries();

    series->append(0, 6);
    series->append(2, 4);
    series->append(3, 8);
    series->append(7, 4);
    series->append(10, 5);

    QLineSeries *series2 = new QLineSeries();

    series2->append(0, 8);
    series2->append(2, 6);
    series2->append(3, 10);
    series2->append(7, 6);
    series2->append(10, 7);

    QChart *chart = new QChart();
        chart->legend()->hide();
        chart->addSeries(series);
        chart->addSeries(series2);
        chart->createDefaultAxes();
        chart->setTitle("Simple line chart example");

    QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

    setCentralWidget(chartView);
    resize(400, 300);
    show();
}

mqtktMainWindow::~mqtktMainWindow()
{
    delete ui;
}
