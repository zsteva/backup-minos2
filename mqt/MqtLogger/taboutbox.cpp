#include "logger_pch.h"

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

/*static*/bool TAboutBox::ShowAboutBox( QWidget *Owner,  bool onStartup )
{
   TAboutBox aboutBox( Owner, onStartup );


   int ret = aboutBox.exec();
   /*
   if ( !started && doStartup )
   {
      // auto start on first run, but only if we gave that option
      TConfigFrame1StartButtonClick( this );
   }
    */
   return ret == QDialog::Accepted;
}

TAboutBox::TAboutBox(QWidget *parent, bool onStartup) :
    QDialog(parent),
    ui(new Ui::TAboutBox),
    doStartup(onStartup)
{
    ui->setupUi(this);
    ui->PageControl1->setCurrentWidget(ui->AboutTabSheet);
 //   TConfigFrame1->initialise();

    ui->AboutMemo->setText("Welcome to Minos 2");

    ui->MinosMemo->setText(MinosText);

    if ( !FileExists( "./Configuration/MinosConfig.ini" ) /*|| !onStartup || checkServerReady()*/ )
    {
       ui->AutoStartTabSheet->setVisible(false);
       ui->LoggerOnlyButton->setVisible(false);
       ui->ExitButton->setVisible(onStartup);
    }
    else
    {
        /*
       TConfigFrame1->StartButton->Enabled = !started;
       TConfigFrame1->StopButton->Visible = false;
       if (  !onStartup || checkServerReady() )
       {
          LoggerOnlyButton->Visible = false; // as we are started we cannot now be logger only
          ExitButton->Visible = false;
       }
       else
       {
          doStartup = true; // click the start button on form close
       }
       */
    }
}

TAboutBox::~TAboutBox()
{
    delete ui;
}

//---------------------------------------------------------------------------
/*
void __fastcall TAboutBox::WebLabelClick( )
{
   ShellExecute( Handle, "open", WebLabel->Caption.c_str(), 0, 0, SW_SHOWNORMAL );   //The home page for this program
}
*/
//---------------------------------------------------------------------------
/*
void __fastcall TAboutBox::TConfigFrame1StartButtonClick( TObject *Sender )
{
   TConfigFrame1->StartButton->Enabled = false;
   TConfigFrame1->StartButtonClick( Sender );
   TConfigFrame1->StopButton->Enabled = true;
   started = true;
}
*/
/*
void __fastcall TAboutBox::TConfigFrame1StopButtonClick( TObject *Sender )
{
   started = false;
   TConfigFrame1->StopButtonClick( Sender );
   TConfigFrame1->StartButton->Enabled = true;
   TConfigFrame1->StopButton->Enabled = false;
}
*/
//---------------------------------------------------------------------------
/*
void __fastcall TAboutBox::TConfigFrame1CancelButtonClick(  )
{
   // cancel the start up
   doStartup = false;
   ModalResult = mrCancel;
}
*/
//---------------------------------------------------------------------------

void TAboutBox::on_ExitButton_clicked()
{
    doStartup = false;
    reject();
}

void TAboutBox::on_OKButton_clicked()
{
    doStartup = false;
    accept();
}

void TAboutBox::on_LoggerOnlyButton_clicked()
{
    doStartup = false;
    accept();
}
