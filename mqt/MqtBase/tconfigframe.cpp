#include "base_pch.h"
#include <QScrollBar>

#include "tconfigframe.h"
#include "ui_tconfigframe.h"

#include "ConfigFile.h"
#include "ConfigElementFrame.h"

TConfigFrame::TConfigFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::TConfigFrame)
{
    ui->setupUi(this);
}

TConfigFrame::~TConfigFrame()
{
    delete ui;
}
void TConfigFrame::initialise(QWidget *p, ConfigCloseCallBack ccb, bool showAutoStart)
{
    ui->autoStartCheckBox->setVisible(showAutoStart);

    closeCb = ccb;
    parent = p;

    QVBoxLayout *vbl = new QVBoxLayout(ui->scrollAreaWidgetContents);
    vbl->setMargin(1);
    ui->scrollAreaWidgetContents->setLayout(vbl);

    elementFrames.clear();
    MinosConfig *minosConfig = MinosConfig::getMinosConfig();

    int offset = 0;
    for (int i = 0; i <  minosConfig->elelist.size(); i++)
    {
        QSharedPointer<RunConfigElement> c = minosConfig->elelist[i];
        if (c->name.compare("<Deleted>", Qt::CaseInsensitive) == 0)
            continue;

        ConfigElementFrame *cef = new ConfigElementFrame();

        // set alternating background

        if (offset++%2)
        {
            cef->setStyleSheet("QFrame { background-color: lightBlue; }");
        }
        else
        {
            cef->setStyleSheet("QFrame { background-color: white; }");
        }

        vbl->addWidget(cef);

        cef->setElement(c);
        elementFrames.append(cef);
    }
    ui->StationIdEdit->setText(minosConfig->getThisServerName());
    ui->HideCheckBox->setChecked(minosConfig->getHideServers());
    ui->autoStartCheckBox->setChecked(minosConfig->getAutoStart());

    QString reqErrs = MinosConfig::getMinosConfig() ->checkConfig();

    if (!reqErrs.isEmpty())
    {
        mShowMessage(reqErrs, p);
    }
}
void TConfigFrame::setup(bool started)
{
    ui->StartButton->setEnabled(!started);
    ui->StopButton->setVisible(false);
}
void TConfigFrame::on_StartButton_clicked()
{
    saveAll();
    MinosConfig::getMinosConfig() ->start();
}

void TConfigFrame::on_StopButton_clicked()
{
    MinosConfig::getMinosConfig() ->stop();
}

void TConfigFrame::on_HideCheckBox_clicked()
{
    MinosConfig::getMinosConfig() ->setHideServers(ui->HideCheckBox->isChecked());
    // Make this active - need a hide/show event that is signalled for show
    // and then all "server" apps need to honour this.
    setShowServers(!ui->HideCheckBox->isChecked());
}
void TConfigFrame::on_autoStartCheckBox_clicked()
{
    MinosConfig::getMinosConfig() ->setAutoStart(ui->autoStartCheckBox->isChecked());
}

void TConfigFrame::on_SetButton_clicked()
{
    QString coh = ui->StationIdEdit->text();
    MinosConfig::getMinosConfig() ->setThisServerName( coh );
}

void TConfigFrame::saveAll()
{
    for (int i = 0; i < elementFrames.size(); i++)
    {
        elementFrames[i]->saveElement();
    }

    on_SetButton_clicked();
    on_autoStartCheckBox_clicked();
    MinosConfig::getMinosConfig()->saveAll();    // which clears the config file before saving
}

void TConfigFrame::on_OKButton_clicked()
{
    saveAll();

    QString reqErrs = MinosConfig::getMinosConfig() ->checkConfig();

    if (reqErrs.isEmpty())
        closeCb(parent);
    else
        mShowMessage(reqErrs, parent);

}
void TConfigFrame::on_CancelButton_clicked()
{
    closeCb(parent);
}

void TConfigFrame::on_newElementButton_clicked()
{
    // Create new element
    ConfigElementFrame *cef = new ConfigElementFrame();

    // set alternating background

    int j = elementFrames.size() - 1;   // will be position of frame in list
    if (j%2)
    {
        cef->setStyleSheet("QFrame { background-color: lightBlue; }");
    }
    else
    {
        cef->setStyleSheet("QFrame { background-color: white; }");
    }

    ui->scrollAreaWidgetContents->layout()->addWidget(cef);

    MinosConfig::getMinosConfig() ->elelist.append(QSharedPointer<RunConfigElement> (new RunConfigElement) );

    int i = MinosConfig::getMinosConfig() ->elelist.size() - 1; // position of element in list
    QSharedPointer<RunConfigElement> c = MinosConfig::getMinosConfig() ->elelist[i];
    c->runType = RunLocal;
    c->appType = "None";
    c->enabled = true;

    cef->setElement(c);
    elementFrames.append(cef);

    repaint();

    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);

    connect(timer, &QTimer::timeout, [=]()
    {
        // NB a lambda function
        ui->elementScrollArea->verticalScrollBar()->setValue(ui->elementScrollArea->verticalScrollBar()->maximum());
        timer->deleteLater();
    }
    );

    timer->start(100);
    cef->setNameFocus();
}
