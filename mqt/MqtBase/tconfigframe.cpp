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
void TConfigFrame::initialise(QWidget *p, ConfigCloseCallBack ccb, bool doAutoStart)
{
    if (!doAutoStart)
        ui->autoStartCheckBox->setVisible(false);

    closeCb = ccb;
    parent = p;

    QVBoxLayout *vbl = new QVBoxLayout(ui->scrollAreaWidgetContents);
    vbl->setMargin(1);
    ui->scrollAreaWidgetContents->setLayout(vbl);

    elementFrames.clear();
    for (int i = 0; i < MinosConfig::getMinosConfig( 0 ) ->elelist.size(); i++)
    {
        ConfigElementFrame *cef = new ConfigElementFrame();

        // set alternating background

        if (i%2)
        {
            cef->setStyleSheet("QFrame { background-color: lightBlue; }");
        }
        else
        {
            cef->setStyleSheet("QFrame { background-color: white; }");
        }

        vbl->addWidget(cef);

        QSharedPointer<TConfigElement> c = MinosConfig::getMinosConfig( 0 ) ->elelist[i];
        cef->setElement(c);
        elementFrames.append(cef);
    }
    ui->StationIdEdit->setText(MinosConfig::getMinosConfig( 0 ) ->getThisServerName());
    ui->HideCheckBox->setChecked(MinosConfig::getMinosConfig( 0 )->getHideServers());
    ui->autoStartCheckBox->setChecked(MinosConfig::getMinosConfig( 0 )->getAutoStart());

    if (doAutoStart && MinosConfig::getMinosConfig( 0 )->getAutoStart())
    {
        connect(&startTimer, SIGNAL(timeout()), this, SLOT(startTimer_Timeout()));
        startTimer.start(100);
    }
    ui->HideCheckBox->setChecked(MinosConfig::getMinosConfig( 0 )->getHideServers());
}
void TConfigFrame::setup(bool started)
{
    ui->StartButton->setEnabled(!started);
    ui->StopButton->setVisible(false);
}
void TConfigFrame::startTimer_Timeout()
{
    startTimer.stop();
    start();
}

void TConfigFrame::start()
{
    MinosConfig::getMinosConfig( 0 ) ->start();
}

void TConfigFrame::on_StartButton_clicked()
{
    saveAll();
    start();
}

void TConfigFrame::on_StopButton_clicked()
{
    MinosConfig::getMinosConfig( 0 ) ->stop();
}

void TConfigFrame::on_HideCheckBox_clicked()
{
    MinosConfig::getMinosConfig( 0 ) ->setHideServers(ui->HideCheckBox->isChecked());
    // Make this active - need a hide/show event that is signalled for show
    // and then all "server" apps need to honour this.
    setShowServers(!ui->HideCheckBox->isChecked());
}
void TConfigFrame::on_autoStartCheckBox_clicked()
{
    MinosConfig::getMinosConfig( 0 ) ->setAutoStart(ui->autoStartCheckBox->isChecked());
}

void TConfigFrame::on_SetButton_clicked()
{
    QString coh = ui->StationIdEdit->text();
    MinosConfig::getMinosConfig( 0 ) ->setThisServerName( coh );
}

void TConfigFrame::saveAll()
{
    for (int i = 0; i < elementFrames.size(); i++)
    {
        elementFrames[i]->saveElement();
    }

    on_SetButton_clicked();
    on_autoStartCheckBox_clicked();
    MinosConfig::getMinosConfig( 0 )->saveAll();    // which clears the config file before saving
}

void TConfigFrame::on_OKButton_clicked()
{
    saveAll();
    closeCb(parent);
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

    int i = elementFrames.size() - 1;
    if (i%2)
    {
        cef->setStyleSheet("QFrame { background-color: lightBlue; }");
    }
    else
    {
        cef->setStyleSheet("QFrame { background-color: white; }");
    }

    ui->scrollAreaWidgetContents->layout()->addWidget(cef);

    MinosConfig::getMinosConfig( 0 ) ->elelist.append(QSharedPointer<TConfigElement> (new TConfigElement) );
    QSharedPointer<TConfigElement> c = MinosConfig::getMinosConfig( 0 ) ->elelist[i + 1];
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
