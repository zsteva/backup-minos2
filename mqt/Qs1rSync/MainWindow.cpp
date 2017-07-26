//#include "XMPP_pch.h"

#include "OmniWrapper.h"

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "qs_defines.h"

#ifdef RUBBISH
//---------------------------------------------------------------------------

#include "XMPP_pch.h"
#pragma hdrstop
#include "inifiles.hpp"
#include "SyncMain.h"
#include "qs_defines.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "OmniRig_OCX"
#pragma resource "*.dfm"

TQRigSyncMain *QRigSyncMain;

/*
   // IF I work as the example with the TLB rather than the OCX
   // does omnirig need to be "installed"? Or can I run up the
   // omnirig.exe as and when I require it, and carry both
   // exe, rigs, and omnirig.ini with MY app?

   // Lets get it working first...
*/
//---------------------------------------------------------------------------
__fastcall TQRigSyncMain::TQRigSyncMain(TComponent* Owner)
   : TForm(Owner)
{
         lastF = "OK\r\n"
         "fHz=28123456\r\n"
         "tf=-123456\r\n";

         int fOffset = lastF.Pos("fHz=");
         int tfOffset = lastF.Pos("tf=");
         if (fOffset > 0 && tfOffset > 0)
         {
            String temp = lastF.SubString(fOffset + 4, tfOffset - fOffset - 4);
            int l = temp.Length();
            while ((temp[l] == '\r') || (temp[l] == '\n'))
            {
               temp = temp.SubString(1, l - 1);
               l = temp.Length();
            }
            fCentre = temp.ToIntDef(0);
            temp = lastF.SubString(tfOffset + 3, 100);
            l = temp.Length();
            while ((temp[l] == '\r') || (temp[l] == '\n'))
            {
               temp = temp.SubString(1, l - 1);
               l = temp.Length();
            }
            ftf = temp.ToIntDef(0);
         }
         double f = (fCentre + ftf);
         lastF = "fCentre " + String(fCentre) + "\r\n tf " + String(ftf) + " freq " + FormatFloat("#0,000,000", f);

   Timer1->Enabled = true;
}
__fastcall TQRigSyncMain::~TQRigSyncMain()
{
}
//---------------------------------------------------------------------------
void __fastcall TQRigSyncMain::CloseButtonClick(TObject *Sender)
{
   Close();
}
//---------------------------------------------------------------------------
void __fastcall TQRigSyncMain::Timer1Timer(TObject *Sender)
{
   static bool inTimer = false;
   if (inTimer)
   {
      return;
   }

   inTimer = true;

   long freq = 0;

   freq = OmniRig->Rig1->FreqA;
   QF1Label->Caption = String(freq);

   QS1RFLabel->Caption = lastF;

   inTimer = false;
}
//---------------------------------------------------------------------------


void __fastcall TQRigSyncMain::RigSelectButtonClick(TObject *Sender)
{
   if (!OmniRig->DialogVisible)
   {
      OmniRig->DialogVisible = true;
   }
}
//---------------------------------------------------------------------------

void __fastcall TQRigSyncMain::FormShow(TObject *Sender)
{
   Top = Screen->Height - Height;
   Left = (Screen->Width - Width)/2;

   OmniRig->Connect();
   Rig1Label->Caption = OmniRig->Rig1->RigType;
}
//---------------------------------------------------------------------------

void __fastcall TQRigSyncMain::FormClose(TObject *Sender, TCloseAction &Action)
{
   //
}
//---------------------------------------------------------------------------

void __fastcall TQRigSyncMain::Transfer12ButtonClick(TObject *Sender)
{
   long freq = 0;

   freq = OmniRig->Rig1->FreqA;
//">UpdateRxFreq\n?fHz\n?tf\n" - may need to set tf (local oscillator) as well
   String mess = ">fHz " + String(fCentre) + "\n";
   mess += ">tf " + String(freq - fCentre) + "\n";
   ClientSocket1->Socket->SendBuf( mess.c_str(), mess.Length() );
}
//---------------------------------------------------------------------------

void __fastcall TQRigSyncMain::Transfer21ButtonClick(TObject *Sender)
{
   long freq = fCentre + ftf;

   OmniRig->Rig1->FreqA = freq;
}
//---------------------------------------------------------------------------
void __fastcall TQRigSyncMain::OmniRigRigTypeChange(TObject *Sender, long RigNumber)
{
   Rig1Label->Caption = OmniRig->Rig1->RigType;
}
//---------------------------------------------------------------------------

void __fastcall TQRigSyncMain::OmniRigStatusChange(TObject *Sender, long RigNumber)
{
//
}
//---------------------------------------------------------------------------

void __fastcall TQRigSyncMain::OmniRigParamsChange(TObject *Sender, long RigNumber,
          long Params)
{
//   bool ptt1 = OmniRig->Rig1->Tx & PM_TX;
//   bool ptt2 = OmniRig->Rig2->Tx & PM_TX;

}
//---------------------------------------------------------------------------

void __fastcall TQRigSyncMain::ConnectQS1RButtonClick(TObject *Sender)
{
// // start up the QS1R socket
   if (ClientSocket1->Active)
   {
      ClientSocket1->Close();
   }
   else
   {
      ClientSocket1->Port = RX1_CMD_SERV_TCP_PORT + 2;
      ClientSocket1->Host = "localhost";

      ClientSocket1->Open();
   }
}
//---------------------------------------------------------------------------

void __fastcall TQRigSyncMain::ClientSocket1Connect(TObject *Sender,
      TCustomWinSocket *Socket)
{
   ConnectQS1RButton->Caption = "Disconnect QS1R";
}
//---------------------------------------------------------------------------

void __fastcall TQRigSyncMain::ClientSocket1Disconnect(TObject *Sender,
      TCustomWinSocket *Socket)
{
   ConnectQS1RButton->Caption = "Connect QS1R";
}
//---------------------------------------------------------------------------

void __fastcall TQRigSyncMain::ClientSocket1Read(TObject *Sender,
      TCustomWinSocket *Socket)
{
   static char sockbuffer[ 4096 ];
   //Hz=5.02e+07
   //tf=-121700

   try
   {
      int retlen = ClientSocket1->Socket->ReceiveBuf( sockbuffer, 4095 );
      if ( retlen > 0 )
      {
         sockbuffer[ retlen ] = 0;

         lastF =  sockbuffer;
         int fOffset = lastF.Pos("fHz=");
         int tfOffset = lastF.Pos("tf=");
         if (fOffset > 0 && tfOffset > 0)
         {
            String temp = lastF.SubString(fOffset + 4, tfOffset - fOffset - 4);
            int l = temp.Length();
            while ((temp[l] == '\r') || (temp[l] == '\n'))
            {
               temp = temp.SubString(1, l - 1);
               l = temp.Length();
            }
            fCentre = temp.ToIntDef(0);
            temp = lastF.SubString(tfOffset + 3, 100);
            l = temp.Length();
            while ((temp[l] == '\r') || (temp[l] == '\n'))
            {
               temp = temp.SubString(1, l - 1);
               l = temp.Length();
            }
            ftf = temp.ToIntDef(0);
         }
         double f = (fCentre + ftf);
         lastF = "fCentre " + String(fCentre) + "\r\n tf " + String(ftf) + " freq " + FormatFloat("#0,000,000", f);
      }
   }
   catch ( Exception & e )
   {
//      Log( "**** TQRigSyncMain::ClientSocket1Read - Exception: " + e.Message );
   }
}
//---------------------------------------------------------------------------

void __fastcall TQRigSyncMain::ClientSocket1Write(TObject *Sender,
      TCustomWinSocket *Socket)
{
   // we can write...
}
//---------------------------------------------------------------------------

void __fastcall TQRigSyncMain::ApplicationEvents1Exception(TObject *Sender,
      Exception *E)
{
//
}
//---------------------------------------------------------------------------

void __fastcall TQRigSyncMain::Timer2Timer(TObject *Sender)
{
   // Poll the QS1R
   String mess = ">UpdateRxFreq\n?fHz\n?tf\n";
   ClientSocket1->Socket->SendBuf( mess.c_str(), mess.Length() );
}
//---------------------------------------------------------------------------

#endif
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    omni_rig.reset (new OmniRig::OmniRigX {this});

    lastF = "OK\r\n"
    "fHz=28123456\r\n"
    "tf=-123456\r\n";

    int fOffset = lastF.indexOf("fHz=");
    int tfOffset = lastF.indexOf("tf=");
    if (fOffset >= 0 && tfOffset >= 0)
    {
       QString temp = lastF.mid(fOffset + 4, tfOffset - fOffset - 4);
       int l = temp.length();
       while ((temp[l] == '\r') || (temp[l] == '\n'))
       {
          temp = temp.right(l - 1);
          l = temp.length();
       }
       fCentre = temp.toInt();
       temp = lastF.mid(tfOffset + 3, 100);
       l = temp.length();
       while ((temp[l] == '\r') || (temp[l] == '\n'))
       {
          temp = temp.right( l - 1);
          l = temp.length();
       }
       ftf = temp.toInt();
    }
    double f = (fCentre + ftf);
    lastF = "fCentre " + QString::number(fCentre) + "\r\n tf " + QString::number(ftf) + " freq " + QLocale::system().toString(f, 'f', 0);

    //omni_rig->connect();
    ui->Rig1Label->setText(omni_rig->Rig1()->RigType());

    connect(&timer1, SIGNAL(timeout()), this, SLOT(timer1Timeout()));
    connect(&timer2, SIGNAL(timeout()), this, SLOT(timer2Timeout()));

    timer1.start(1000);
    timer2.start(1000);

    connect(&ClientSocket1, SIGNAL(connected()), this, SLOT(onSocketConnect()));
    connect(&ClientSocket1, SIGNAL(disconnected()), this, SLOT(onSocketDisconnect()));
    connect(&ClientSocket1, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(&ClientSocket1, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

    connect (&*omni_rig, SIGNAL (VisibleChange ()), this, SLOT (handle_visibleChange ()));
    connect(&*omni_rig, SIGNAL(RigTypeChange(int)), this, SLOT(handle_rigTypeChange(int)));
    connect (&*omni_rig, SIGNAL (StatusChange (int)), this, SLOT (handle_statusChange (int)));
    connect (&*omni_rig, SIGNAL (ParamsChange (int, int)), this, SLOT (handle_paramsChange (int, int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::timer1Timeout()
{
    static bool inTimer = false;
    if (inTimer)
    {
       return;
    }

    inTimer = true;

    long freq = 0;

    freq = omni_rig->Rig1()->FreqA();
    ui->QF1Label->setText(QString::number(freq));

    ui->QS1RFLabel->setText(lastF);

    inTimer = false;
}

void MainWindow::timer2Timeout()
{
    // Poll the QS1R
    QString mess = ">UpdateRxFreq\n?fHz\n?tf\n";
    ClientSocket1.write( mess.toLatin1().data(), mess.length() );
}

void MainWindow::onSocketConnect()
{

}

void MainWindow::onSocketDisconnect()
{

}
void MainWindow::onError(QAbstractSocket::SocketError err)
{

}

void MainWindow::onReadyRead()
{
    static char sockbuffer[ 4096 ];
    //Hz=5.02e+07
    //tf=-121700

    while(ClientSocket1.bytesAvailable())
    {
       qint64 retlen = ClientSocket1.read( sockbuffer, 4095 );
       if ( retlen > 0 )
       {
          sockbuffer[ retlen ] = 0;

          lastF =  sockbuffer;
          int fOffset = lastF.indexOf("fHz=");
          int tfOffset = lastF.indexOf("tf=");
          if (fOffset >= 0 && tfOffset >= 0)
          {
             QString temp = lastF.mid(fOffset + 4, tfOffset - fOffset - 4);
             int l = temp.length();
             while ((temp[l] == '\r') || (temp[l] == '\n'))
             {
                temp = temp.right(l - 1);
                l = temp.length();
             }
             fCentre = temp.toInt();
             temp = lastF.mid(tfOffset + 3, 100);
             l = temp.length();
             while ((temp[l] == '\r') || (temp[l] == '\n'))
             {
                temp = temp.right( l - 1);
                l = temp.length();
             }
             ftf = temp.toInt();
          }
          double f = (fCentre + ftf);
          lastF = "fCentre " + QString::number(fCentre) + "\r\n tf " + QString::number(ftf) + " freq " + QLocale::system().toString(f, 'f', 0);
       }
    }
}

void MainWindow::on_selectButton_clicked()
{
    if (!omni_rig->DialogVisible())
    {
       omni_rig->SetDialogVisible(true);
    }
}
void MainWindow::on_connectQS1RButton_clicked()
{
    if (ClientSocket1.isOpen())
    {
        ClientSocket1.close();
    }
    else
    {
        ClientSocket1.connectToHost(QHostAddress::LocalHost, RX1_CMD_SERV_TCP_PORT + 2);
    }
}

void MainWindow::on_closeButton_clicked()
{
    close();
}

void MainWindow::on_transfer12Button_clicked()
{
    long freq = 0;

    freq = omni_rig->Rig1()->FreqA();
 //">UpdateRxFreq\n?fHz\n?tf\n" - may need to set tf (local oscillator) as well
    QString mess = ">fHz " + QString::number(fCentre) + "\n";
    mess += ">tf " + QString::number(freq - fCentre) + "\n";
    ClientSocket1.write( mess.toLatin1().data(), mess.length() );
}

void MainWindow::on_transfer21Button_clicked()
{
    long freq = fCentre + ftf;

    omni_rig->Rig1()->SetFreqA(freq);
}

void MainWindow::handle_visibleChange()
{

}

void MainWindow::handle_rigTypeChange(int r)
{
    ui->Rig1Label->setText(omni_rig->Rig1()->RigType());
}
void MainWindow::handle_statusChange(int r)
{

}

void MainWindow::handle_paramsChange(int a, int b)
{

}
