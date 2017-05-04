#include "logger_pch.h"
#include <QDesktopServices>

#include "taboutbox.h"
#include "ui_taboutbox.h"

bool started = false;

QString MinosText =
   "The Inferno, Dante, (trans Dorothy L Sayers, Penguin Classics); Canto 5.\r\n"
   "\r\n"
   "\"From the first circle thus I came descending\r\n"
   "To the second, which, in narrower compass turning,\r\n"
   "Holds greater woe, with outcry loud and rending.\"\r\n"
   "\r\n"
   "\"There in the threshold, horrible and giming,\r\n"
   "Grim Minos sits, holding his ghastly session,\r\n"
   "And, as he girds him, sentencing and spurning;\"\r\n"
   "\r\n"
   "\"For when the ill soul faces him, confession\r\n"
   "Pours out of it till nothing's left to tell;\r\n"
   "Whereon that connoisseur of all transgression\"\r\n"
   "\r\n"
   "\"Assigns it to its proper place in hell,\r\n"
   "As many grades as he would have it fall,\r\n"
   "So oft he belts him round with his own tail.\"\r\n"
   "\r\n"
   "\"Before him stands a throng continual;\r\n"
   "Each comes in turn to abye the fell arraign;\r\n"
   "They speak - they hear - they're whirled down one and all.\"\r\n"
   "\r\n"
   "So when I was thinking a name for a messaging server, it amused me to think of it belting messages round a field with its tail!\r\n"
   "\r\n"
   "Of course another quote from Dante is most apt for anyone thinking of becoming involved in this kind of project…\r\n"
   "\r\n"
   "“Abandon Hope, all ye who enter here”…"
   ;

/*static*/bool TAboutBox::ShowAboutBox(QWidget *Owner,  bool onStartup )
{
   TAboutBox aboutBox( Owner, onStartup );


   int ret = aboutBox.exec();

   return ret == QDialog::Accepted;
}

int TAboutBox::exec()
{
    int ret = QDialog::exec();
    if (ret == QDialog::Rejected)
    {
        doStartup = false;
    }
    if ( !started && doStartup )
    {
       // auto start on first run, but only if we gave that option
       ui->ConfigFrame->start();
    }
    return ret;
}
void closeCallback(QWidget *w)
{
    TAboutBox *tab = dynamic_cast<TAboutBox *>(w);
    tab->reject();
}
TAboutBox::TAboutBox(QWidget *parent, bool onStartup) :
    QDialog(parent),
    ui(new Ui::TAboutBox),
    doStartup(onStartup)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QSettings settings;
    QByteArray geometry = settings.value("MinosAbout/geometry").toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);

    ui->ConfigFrame->initialise(this, &::closeCallback);
    ui->PageControl1->setCurrentWidget(ui->AboutTabSheet);

    ui->AboutMemo->setText(QString("<h1>Welcome to Minos 2 Version ") + VERSION + " Beta" + "</h1><br><a href=\"http://minos.sourceforge.net/\">http://minos.sourceforge.net</a>");
    ui->AboutMemo->setTextFormat(Qt::RichText);
    ui->AboutMemo->setTextInteractionFlags(Qt::TextBrowserInteraction);

    ui->MinosMemo->setText(MinosText);

    if ( !FileExists( "./Configuration/MinosConfig.ini" ) /*|| !onStartup || checkServerReady()*/ )
    {
       ui->AutoStartTabSheet->setVisible(false);
       ui->LoggerOnlyButton->setVisible(false);
       ui->ExitButton->setVisible(onStartup);
    }
    else
    {
        ui->ConfigFrame->setup(started);
       if (  !onStartup || checkServerReady() )
       {
          ui->LoggerOnlyButton->setVisible(false); // as we are started we cannot now be logger only
          ui->ExitButton->setVisible(false);
       }
       else
       {
          doStartup = true; // click the start button on form close
       }
    }
}

TAboutBox::~TAboutBox()
{
    delete ui;
}
void TAboutBox::doCloseEvent()
{
    QSettings settings;
    settings.setValue("MinosAbout/geometry", saveGeometry());
}
void TAboutBox::reject()
{
    doCloseEvent();
    QDialog::reject();
}
void TAboutBox::accept()
{
    doCloseEvent();
    QDialog::accept();
}
void TAboutBox::on_AboutMemo_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}

void TAboutBox::on_ExitButton_clicked()
{
    doStartup = false;
    reject();
}

void TAboutBox::on_OKButton_clicked()
{
    accept();
}

void TAboutBox::on_LoggerOnlyButton_clicked()
{
    doStartup = false;
    accept();
}

