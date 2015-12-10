#include "taboutbox.h"
#include "ui_taboutbox.h"

TAboutBox::TAboutBox(QWidget *parent, bool onStartup) :
    QDialog(parent),
    ui(new Ui::TAboutBox),
    doStartup(onStartup)
{
    ui->setupUi(this);
}

TAboutBox::~TAboutBox()
{
    delete ui;
}
/*static*/bool TAboutBox::ShowAboutBox( QWidget *Owner,  bool onStartup )
{
   TAboutBox aboutBox( Owner, onStartup );
   int ret = aboutBox.exec();
   return ret == QDialog::Accepted;
}
#ifdef RUBBISH
bool started = false;

String MinosText =
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
//---------------------------------------------------------------------
__fastcall TAboutBox::TAboutBox( TComponent *Owner, bool onStartup )
      : TForm( Owner ), doStartup(false)
{

   TMyRCVersion RCVersion;
   RCVersion.initialise();
   ProductName->Caption = RCVersion.ProductName;
   String minor = RCVersion.MinorVersion;
#ifdef BETA_VERSION
   bool Beta = true;
#else
   bool Beta = false;
#endif
   Version->Caption = "Version " + RCVersion.FileVersion + ( Beta ? " BETA" : "" );
   Copyright->Caption = RCVersion.LegalCopyright;
   String caption = String("Git SHA1 ") + GITVER + "\r\n\r\n";
   Comments->Caption = caption + ( Beta ? "\r\nBeta version - use at your own risk!\r\n\r\n\r\n" : "" ) + RCVersion.Comments;

   if ( !FileExists( ".\\Configuration\\MinosConfig.ini" ) /*|| !onStartup || checkServerReady()*/ )
   {
      AutoStartTabSheet->TabVisible = false;
      LoggerOnlyButton->Visible = false;
      ExitButton->Visible = onStartup;
   }
   else
   {
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
   }

}
//---------------------------------------------------------------------
/*static*/bool TAboutBox::ShowAboutBox( TComponent *Owner,  bool onStartup )
{
   std::auto_ptr <TAboutBox> aboutBox( new TAboutBox( Owner, onStartup ) );
   return aboutBox->ShowModal() == mrOk;
}
//---------------------------------------------------------------------

void __fastcall TAboutBox::FormShow( TObject */*Sender*/ )
{
   MinosParameters::getMinosParameters() ->applyFontChange(this);
   PageControl1->ActivePage = AboutTabSheet;
   TConfigFrame1->initialise();
   WebLabel->Font->Color = clBlue;
   WebLabel->Font->Style = WebLabel->Font->Style << fsUnderline;

   MinosMemo->Text = MinosText;
}
//---------------------------------------------------------------------------

void __fastcall TAboutBox::WebLabelClick( TObject */*Sender*/ )
{
   ShellExecute( Handle, "open", WebLabel->Caption.c_str(), 0, 0, SW_SHOWNORMAL );   //The home page for this program
}
//---------------------------------------------------------------------------

void __fastcall TAboutBox::TConfigFrame1StartButtonClick( TObject *Sender )
{
   TConfigFrame1->StartButton->Enabled = false;
   TConfigFrame1->StartButtonClick( Sender );
   TConfigFrame1->StopButton->Enabled = true;
   started = true;
}
//---------------------------------------------------------------------------

void __fastcall TAboutBox::FormClose( TObject */*Sender*/, TCloseAction &/*Action*/ )
{
   if ( !started && doStartup )
   {
      // auto start on first run, but only if we gave that option
      TConfigFrame1StartButtonClick( this );
   }
}
//---------------------------------------------------------------------------

void __fastcall TAboutBox::TConfigFrame1StopButtonClick( TObject *Sender )
{
   started = false;
   TConfigFrame1->StopButtonClick( Sender );
   TConfigFrame1->StartButton->Enabled = true;
   TConfigFrame1->StopButton->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TAboutBox::LoggerOnlyButtonClick( TObject */*Sender*/ )
{
   doStartup = false;
   ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TAboutBox::TConfigFrame1CancelButtonClick( TObject */*Sender*/ )
{
   // cancel the start up
   doStartup = false;
   ModalResult = mrCancel;
}
//---------------------------------------------------------------------------



void __fastcall TAboutBox::ExitButtonClick(TObject */*Sender*/)
{
   // cancel the start up
   doStartup = false;
   ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
#endif

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
