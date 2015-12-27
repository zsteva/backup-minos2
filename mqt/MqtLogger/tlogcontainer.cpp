#include "logger_pch.h"

#include <QFontDialog>
#include <QFileDialog>

#include "tlogcontainer.h"
#include "ui_tlogcontainer.h"

#include "taboutbox.h"
#include "CalendarList.h"
#include "contestdetails.h"
#include "tmanagelistsdlg.h"

TLogContainer *LogContainer = 0;

TLogContainer::TLogContainer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TLogContainer)
{
    ui->setupUi(this);

    LogContainer = this;

    // we may need to delay this to get the container fully constructed
    TContestApp::getContestApp(); // initialise all the infrastructure

    // make the tab control fill the window
    ui->centralWidget->layout()->setContentsMargins(0,0,0,0);;

    setWindowTitle("MinosQt Contest Logger");

    setupMenus();

    ui->ContestPageControl->setContextMenuPolicy( Qt::CustomContextMenu );

    QSettings settings;
    QByteArray geometry = settings.value("geometry").toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);

    sblabel0 = new QLabel( "" );
    statusBar() ->addWidget( sblabel0, 6 );
    sblabel1 = new QLabel( "" );
    statusBar() ->addWidget( sblabel1, 1 );
    sblabel2 = new QLabel( "" );
    statusBar() ->addWidget( sblabel2, 2 );
}

TLogContainer::~TLogContainer()
{
    delete ui;
}
bool TLogContainer::show(int argc, char *argv[])
{
     bool mlpa = isScrollingContestTabs();
    ScrollingContestTabsAction->setChecked(mlpa);
    ui->ContestPageControl->setUsesScrollButtons(!mlpa);

    bool so = isShowOperators();
    ShowOperatorsAction->setChecked(so);

    bool autoFill;
    TContestApp::getContestApp() ->loggerBundle.getBoolProfile( elpAutoFill, autoFill );
    ReportAutofillAction->setChecked(autoFill);
    TContestApp::getContestApp() ->loggerBundle.flushProfile();

    TimerUpdateQSOTimer.start(1000);
    connect(&TimerUpdateQSOTimer, SIGNAL(timeout()), this, SLOT(on_TimeDisplayTimer()));

    connect(&MinosLoggerEvents::mle, SIGNAL(ReportOverstrike(bool , BaseContestLog * )), this, SLOT(on_ReportOverstrike(bool , BaseContestLog * )));

//    SendDM = new TSendDM( this );
    QMainWindow::show();
    if ( TAboutBox::ShowAboutBox( this, true ) == false )
    {
       close();
       return false;
    }

    if ( contestAppLoadFiles() )
    {
       // here need to pre-open the contest list
       QString conarg;
       if ( argc > 1 )
       {
          conarg = argv[1];
       }
       preloadFiles( conarg );
       enableActions();
    }
    return true;
}
void TLogContainer::on_TimeDisplayTimer( )
{

   if ( TContestApp::getContestApp() )
   {
       QString disp = QDateTime::currentDateTimeUtc().toString( "dd/MM/yyyy hh:mm:ss" ) + " UTC       ";

       sblabel2 ->setText(disp);

       MinosLoggerEvents::SendTimerDistribution();

       QString statbuf;
      BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
      if ( ct )
      {
         ct->setScore( statbuf );
      }
      sblabel0->setText( statbuf );
   }

}
void TLogContainer::on_ReportOverstrike(bool overstrike, BaseContestLog *econtest )
{
   BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
   if (ct == econtest)
   {
      sblabel1->setText(overstrike ? "Overwrite" : "Insert");
   }
}

void TLogContainer::closeEvent(QCloseEvent *event)
{
    TimerUpdateQSOTimer.stop();
    closeContestApp();
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    QWidget::closeEvent(event);
}
void TLogContainer::resizeEvent(QResizeEvent * event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    QWidget::resizeEvent(event);
}

/*

ContestPageControl
    OnChange = ContestPageControlChange
    OnDrawTab = ContestPageControlDrawTab
    OnMouseDown = ContestPageControlMouseDown
    OnMouseMove = ContestPageControlMouseMove

StatusBar1
    Hint = 'Click to correct date/time'
    Action = CorrectDateTimeAction
    OnDblClick = CorrectDateTimeActionExecute
*/
QAction *TLogContainer::newAction( const QString &text, QMenu *m, const char *atype )
{
    QAction * newAct = new QAction( text, this );
    m->addAction( newAct );
    connect( newAct, SIGNAL( triggered() ), this, atype );
    return newAct;
}
QAction *TLogContainer::newCheckableAction( const QString &text, QMenu *m, const char *atype )
{
    QAction * newAct = new QAction( text, this );
    newAct->setCheckable( true );
    m->addAction( newAct );
    connect( newAct, SIGNAL( triggered( bool ) ), this, atype );
    return newAct;
}

/*

private slots:

*/
void TLogContainer::setupMenus()
{
    FileOpenAction = newAction("&Open Contest...", ui->menuFile, SLOT(FileOpenActionExecute()));
    recentFilesMenu = ui->menuFile->addMenu("Reopen Contest");

    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
        recentFilesMenu->addAction(recentFileActs[i]);
    }
    updateRecentFileActions();

    FileNewAction = newAction("&New Contest...", ui->menuFile, SLOT(FileNewActionExecute()));
    FileCloseAction = newAction("Close Contest", ui->menuFile, SLOT(FileCloseActionExecute()));
    ui->menuFile->addSeparator();
    ContestDetailsAction = newAction("Contest Details...", ui->menuFile, SLOT(ContestDetailsActionExecute()));
    MakeEntryAction = newAction("Produce Entry/Export File...", ui->menuFile, SLOT(MakeEntryActionExecute()));
    ui->menuFile->addSeparator();
    ListOpenAction = newAction("Open &Archive List...", ui->menuFile, SLOT(ListOpenActionExecute()));
    ManageListsAction = newAction("&Manage Archive Lists...", ui->menuFile, SLOT(ManageListsActionExecute()));
    ui->menuFile->addSeparator();
    OptionsAction = newAction("Options...", ui->menuFile, SLOT(OptionsActionExecute()));
    ExitAction = newAction("E&xit MinosQt", ui->menuFile, SLOT(ExitActionExecute()));

    GoToSerialAction = newAction("&Go To Contact Serial...", ui->menuSearch, SLOT(GoToSerialActionExecute()));
    NextUnfilledAction = newAction("Goto First Unfilled Contact", ui->menuSearch, SLOT(NextUnfilledActionExecute()));

    LocCalcAction = newAction("Locator Calculator", ui->menuTools, SLOT(LocCalcActionExecute()));
    AnalyseMinosLogAction = newAction("Analyse Minos Log", ui->menuTools, SLOT(AnalyseMinosLogActionExecute()));
    ui->menuFile->addSeparator();
    FontEditAcceptAction = newAction("Select &Font...", ui->menuTools, SLOT(FontEditAcceptActionExecute()));
    ReportAutofillAction = newCheckableAction("Signal Report AutoFill", ui->menuTools, SLOT(ReportAutofillActionExecute()));


    TabPopup.addAction(FileCloseAction);
    CloseAllAction = newAction("Close all Contests", &TabPopup, SLOT(CloseAllActionExecute()));
    CloseAllButAction = newAction("Close all but this Contest", &TabPopup, SLOT(CloseAllButActionExecute()));
    TabPopup.addSeparator();
    TabPopup.addAction(ContestDetailsAction);
    TabPopup.addAction(MakeEntryAction);
    TabPopup.addSeparator();
    TabPopup.addAction(GoToSerialAction);
    TabPopup.addAction(NextUnfilledAction);
    TabPopup.addSeparator();
    NextContactDetailsOnLeftAction = newAction("&Next Contact Details On Left", &TabPopup, SLOT(NextContactDetailsOnLeftActionExecute()));
    ScrollingContestTabsAction = newAction("Scrolling contest tabs", &TabPopup, SLOT(ScrollingContestTabsActionExecute()));
    ShowOperatorsAction = newAction("Show Operators", &TabPopup, SLOT(ShowOperatorsActionExecute()));
    TabPopup.addSeparator();
    ShiftTabLeftAction = newAction("Shift Active Tab Left", &TabPopup, SLOT(ShiftTabLeftActionExecute()));
    ShiftTabRightAction = newAction("Shift Active Tab Right", &TabPopup, SLOT(ShiftTabRightActionExecute()));
    CorrectDateTimeAction = newAction("Correct Date/Time", &TabPopup, SLOT(CorrectDateTimeActionExecute()));
    TabPopup.addSeparator();
    TabPopup.addAction(AnalyseMinosLogAction);
    newAction( "Cancel", &TabPopup, SLOT( CancelClick() ) );

    HelpAboutAction = newAction("About...", ui->menuHelp, SLOT(HelpAboutActionExecute()));
}
void TLogContainer::enableActions()
{
   bool f = ( ui->ContestPageControl->currentIndex() >= 0 );

   LocCalcAction->setEnabled(true);
   FileNewAction->setEnabled(true);
   HelpAboutAction->setEnabled(true);

   FileCloseAction->setEnabled(f);
   CloseAllAction->setEnabled(f);
   CloseAllButAction->setEnabled(f);

   NextContactDetailsOnLeftAction->setEnabled(true);

   ContestDetailsAction->setEnabled(f);
   GoToSerialAction->setEnabled(f);
   NextUnfilledAction->setEnabled(f);
   MakeEntryAction->setEnabled(f);

   ManageListsAction->setEnabled( TContestApp::getContestApp() ->getOccupiedListSlotCount() > 0 );

   NextContactDetailsOnLeftAction->setChecked(isNextContactDetailsOnLeft());

   if ( ui->ContestPageControl->currentIndex() >= 0 )
   {
      int tno = ui->ContestPageControl->currentIndex();
      ShiftTabLeftAction->setEnabled( tno > 0 );
      ShiftTabRightAction->setEnabled( tno < ui->ContestPageControl->count() - 1 );
   }
   else
   {
      ShiftTabLeftAction->setEnabled(false);
      ShiftTabRightAction->setEnabled(false);
   }

}
bool TLogContainer::isNextContactDetailsOnLeft()
{
   bool ncdol;
   TContestApp::getContestApp() ->displayBundle.getBoolProfile( edpNextContactDetailsOnLeft, ncdol );
   return ncdol;
}

bool TLogContainer::isScrollingContestTabs()
{
   bool ncdol;
   TContestApp::getContestApp() ->displayBundle.getBoolProfile( edpScrollingContestTabs, ncdol );
   return ncdol;
}

bool TLogContainer::isShowOperators()
{
   bool ncdol;
   TContestApp::getContestApp() ->displayBundle.getBoolProfile( edpShowOperators, ncdol );
   return ncdol;
}

void TLogContainer::CancelClick()
{
    // do nothing...
}

void TLogContainer::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString FileName = action->data().toString();

        if ( FileExists( FileName ) )
        {
           setCurrentFile(FileName);
           ContestDetails pced( this );
           BaseContestLog *ct = addSlot( &pced, FileName, false, -1 );
           if (ct)
           {
               selectContest(ct, 0);
           }
        }
        else
        {
            removeCurrentFile( FileName );
        }
    }
}

void TLogContainer::setCurrentFile(const QString &fileName)
{
    QSettings settings;
    QStringList files = settings.value("dbmru").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("dbmru", files);

    updateRecentFileActions();

}
void TLogContainer::removeCurrentFile(const QString &fileName)
{
    QSettings settings;
    QStringList files = settings.value("dbmru").toStringList();
    files.removeAll(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("dbmru", files);

    updateRecentFileActions();

}
QString TLogContainer::getCurrentFile()
{
    QSettings settings;
    QStringList files = settings.value("dbmru").toStringList();
    if (files.size())
        return files[0];
    return QString();
}
void TLogContainer::updateRecentFileActions()
{
    QSettings settings;
    QStringList files = settings.value("dbmru").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);

    recentFilesMenu->setEnabled(numRecentFiles > 0);
}

QString TLogContainer::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}


void TLogContainer::HelpAboutActionExecute()
{
    TAboutBox::ShowAboutBox(this, false);
}
QString TLogContainer::getDefaultDirectory( bool IsList )
{
   QString fileName;
   if ( IsList )
   {
      TContestApp::getContestApp() ->loggerBundle.getStringProfile( elpListDirectory, fileName );

   }
   else
   {
      TContestApp::getContestApp() ->loggerBundle.getStringProfile( elpLogDirectory, fileName );
   }

   // we default to "./logs" or "./lists. On Vista this won't be relative to
   // the executable, but to the user data directory (I think). This is why Minos
   // invents a very strange default directory which you then cannot find.

   if ( fileName.size() && DirectoryExists( fileName ) )
   {
      // need to check possible validity of directory string
      if ( fileName[ fileName.size() ] != '/' )
         fileName += "/";
      if ( fileName[ fileName.size() ] == '/' )
      {
         fileName = fileName.left( fileName.size() - 1 );
      }
   }
   else
      fileName = "";

   return fileName;
}
void TLogContainer::FileNewActionExecute()
{
    QString InitialDir = GetCurrentDir() + "/Logs/";
    // generate a default filename for a new contest
    QString nfileName( "C");

    QDate d = QDate::currentDate();      // get time now
    nfileName += d.toString("ddMMyy");

    char letter = 'A';
    while ( letter < 'Z' )      // the A of A.Minos
    {
       QString fileNameBuff = InitialDir + "/" + nfileName + letter + ".minos";

       if (FileExists(fileNameBuff))
       {
           letter ++;
       }
       else
          break;
    }

    QString initName = InitialDir + nfileName + letter + ".minos";
    ContestDetails pced( this );
    BaseContestLog * c = addSlot( &pced, initName, true, -1 );

    bool repeatDialog = (c != 0);
    while ( repeatDialog )
    {
       QString suggestedfName;
       c->mycall.validate();
       suggestedfName = ( c->mycall.prefix + c->mycall.number + c->mycall.body );
       suggestedfName += '_';
       if ( c->DTGStart.getValue().size() )
       {
          suggestedfName += CanonicalToTDT( c->DTGStart.getValue() ).toString( "yyyyMMMdd" );
       }
       else
       {
          suggestedfName += QDate::currentDate().toString( "yyyyMMMdd" );
       }
       QString band = c->band.getValue();
       if ( band.size() )
       {
          suggestedfName += '_';
          suggestedfName += band;
       }
       suggestedfName += ".minos";

       closeSlot(ui->ContestPageControl->currentIndex(), false );

       QString fileName = QFileDialog::getSaveFileName( this,
                          "Save new contest as",
                          "./Logs/" + suggestedfName,
                          "Minos contest files (*.Minos)",
                          0,
                          QFileDialog::DontConfirmOverwrite
                                                      );
       if ( !fileName.isEmpty() )
       {
          suggestedfName = fileName;
          QDir r(GetCurrentDir() + "Logs");
          if ( !r.rename( initName, suggestedfName ) )
          {
             MinosParameters::getMinosParameters() ->mshowMessage( QString( "Failed to rename " ) + initName + " as " + suggestedfName );
             suggestedfName = initName;
          }

          // we want to (re)open it WITHOUT using the dialog!
          addSlot( 0, suggestedfName, false, -1 );
          repeatDialog = false;
       }
       else
       {
             ContestDetails pced( this);
             c = addSlot( &pced, initName, false, -1 );
             repeatDialog = (c != 0);
       }
    }
    if (!c)
    {
       if ( !QFile::remove( initName ) )
       {
          MinosParameters::getMinosParameters() ->mshowMessage( QString( "Failed to delete " ) + initName );
       }
    }
    else
    {
        selectContest(c, 0);
    }
}
void TLogContainer::FileOpenActionExecute()
{
    // first choose file
//"Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"

    QString Filter = "Minos contest files (*.minos);;"
                     "Reg1Test Files (*.edi);;"
                     "GJV contest files (*.gjv);;"
                     "RSGB Log Files (*.log);;"
                     "ADIF Files (*.adi);;"
                     "All Files (*.*)" ;

    QString fname = QFileDialog::getOpenFileName( this,
                       "Open contest",
                       "",
                       Filter
                       );
    BaseContestLog *ct = 0;
    if ( !fname.isEmpty() )
    {
        ContestDetails pced(this );
        ct = addSlot( &pced, fname, false, -1 );   // not automatically read only
        if (ct)
        {
            selectContest(ct, 0);
        }
    }
}

void TLogContainer::ContestDetailsActionExecute()
{
    QWidget *tw = ui->ContestPageControl->currentWidget();
    TSingleLogFrame *f = dynamic_cast<TSingleLogFrame *>( tw );

    if (f)
    {
        LoggerContestLog *ct = dynamic_cast<LoggerContestLog *>( f->getContest());


        if (ct)
        {
            ContestDetails pced( this );


            pced.setDetails( ct );
            if ( pced.exec() == QDialog::Accepted )
            {
                // and we need to do some re-init on the display
                //updateQSODisplay();
                ct->scanContest();
                f->refreshMults();
            }
        }
    }
}
//---------------------------------------------------------------------------

void TLogContainer::FileCloseActionExecute()
{
   int t = ui->ContestPageControl->currentIndex();
   closeSlot(t, true );
}

//---------------------------------------------------------------------------

void TLogContainer::CloseAllActionExecute()
{
   while ( ui->ContestPageControl->count())
   {
      // Keep closing the current (and hence visible) contest
      closeSlot(0, true);
   }
   on_ContestPageControl_currentChanged(0);
   enableActions();
}
//---------------------------------------------------------------------------

void TLogContainer::CloseAllButActionExecute()
{
   QWidget *thisContest = ui->ContestPageControl->currentWidget();
   while ( ui->ContestPageControl->count() > 1)
   {
       int t = ui->ContestPageControl->count() - 1;
      QWidget *ctab = ui->ContestPageControl->widget(t);
      if (ctab == thisContest)
      {
         t -= 1;
      }
      closeSlot(t, true);
   }
   on_ContestPageControl_currentChanged(0);
   enableActions();
}
//---------------------------------------------------------------------------

void TLogContainer::ExitActionExecute()
{
    close();
}
void TLogContainer::MakeEntryActionExecute()
{
    BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
    MinosLoggerEvents::SendMakeEntry(ct);
}

void TLogContainer::LocCalcActionExecute()
{

}

void TLogContainer::AnalyseMinosLogActionExecute()
{

}

void TLogContainer::CorrectDateTimeActionExecute()
{

}

void TLogContainer::ScrollingContestTabsActionExecute()
{

}

void TLogContainer::ShowOperatorsActionExecute()
{

}

void TLogContainer::OptionsActionExecute()
{

}

void TLogContainer::FontEditAcceptActionExecute()
{
    QApplication::setFont( QFontDialog::getFont( 0, font() ) );

    foreach ( QWidget * widget, QApplication::allWidgets() )
    widget->update();

    QSettings settings;
    settings.setValue( "font", font() );
}

void TLogContainer::ReportAutofillActionExecute()
{
    bool autoFill = ReportAutofillAction->isChecked();
    TContestApp::getContestApp() ->loggerBundle.setBoolProfile( elpAutoFill, autoFill );
    TContestApp::getContestApp() ->loggerBundle.flushProfile();

}

void TLogContainer::GoToSerialActionExecute()
{

}

void TLogContainer::NextUnfilledActionExecute()
{

}

void TLogContainer::NextContactDetailsOnLeftActionExecute()
{

}

void TLogContainer::on_ContestPageControl_currentChanged(int index)
{
    enableActions();

    MinosLoggerEvents::SendContestPageChanged();

    TContestApp::getContestApp() ->writeContestList();
    enableActions();
}

void TLogContainer::on_ContestPageControl_tabBarDoubleClicked(int /*index*/)
{
    //
}

void TLogContainer::on_ContestPageControl_customContextMenuRequested(const QPoint &pos)
{
    QPoint globalPos = ui->ContestPageControl->mapToGlobal( pos );

    TabPopup.popup( globalPos );
}
BaseContestLog * TLogContainer::addSlot(ContestDetails *ced, const QString &fname, bool newfile, int slotno )
{
   static int namegen = 0;
   // openFile ends up calling ContestLog::initialise which then
   // calls TContestApp::insertContest

   LoggerContestLog * contest = TContestApp::getContestApp() ->openFile( fname, newfile, slotno );

   if ( contest )
   {
      bool show = false;
      if ( ced )
      {

         ced->setDetails( contest );

         {
            if ( ced->exec() == QDialog::Accepted )
            {
               contest->scanContest();
               show = true;
            }
            else
            {
               TContestApp::getContestApp() ->closeFile( contest );
               contest = 0;
            }
         }
      }
      else
      {
         contest->scanContest();
         show = true;
      }

      if ( show )
      {
         TContestApp::getContestApp() ->setCurrentContest( contest );
         QString baseFName = ExtractFileName( contest->cfileName );
         TSingleLogFrame *f = new TSingleLogFrame( this, contest );
         f->setObjectName( QString( "LogFrame" ) + QString::number(namegen++));


         int tno = ui->ContestPageControl->addTab(f, baseFName);
         ui->ContestPageControl->setCurrentWidget(ui->ContestPageControl->widget(tno));

         f->logColumnsChanged = true;  // also causes show QSOs
         f->splittersChanged = true;

         on_ContestPageControl_currentChanged(tno);

         if ( contest->needsExport() )      // imported from an alien format (e.g. .log)
         {
            QString expName = f->makeEntry( true );
            if ( expName.size() )
            {
               closeSlot(tno, true );
               addSlot( 0, expName, false, -1 );
            }
         }
         ui->ContestPageControl->tabBar()->setTabTextColor(tno,Qt::blue);
        // ui->ContestPageControl->tabBar()->setTabBackgroundColor(tno,Qt::red); Doesn't exist :(
         removeCurrentFile( fname );
      }
   }
   TContestApp::getContestApp() ->writeContestList();
   enableActions();
   return contest;
}
TSingleLogFrame *TLogContainer::getCurrentLogFrame()
{
    QWidget *w = ui->ContestPageControl->currentWidget();
    TSingleLogFrame *f = dynamic_cast<TSingleLogFrame *>(w);
    return f;
}

void TLogContainer::closeSlot(int t, bool addToMRU )
{
   if ( t >= 0 )
   {
      TSingleLogFrame * f = findLogFrame(t);

      if (f)
      {
          if ( addToMRU )
          {
             BaseContestLog * contest = f->getContest();
             QString curPath = contest->cfileName;
             setCurrentFile( curPath );
          }
          f->closeContest();    // which should close the contest
          ui->ContestPageControl->removeTab(t);
          on_ContestPageControl_currentChanged(0);
      }
      enableActions();
   }
}
TSingleLogFrame *TLogContainer::findLogFrame(int t)
{
    // we need to find the embedded frame...
    // now ONLY used in closeSlot!
    if ( t < 0 )
        return 0;
    QWidget *tw = ui->ContestPageControl->widget(t);
    if ( TSingleLogFrame * f = dynamic_cast<TSingleLogFrame *>( tw ))
    {
        return f;
    }
    return 0;
}
void TLogContainer::preloadFiles( const QString &conarg )
{
   // and here we want to pre-load lists and contests from the INI file
   // based on what was last open

// getProfileEntries gets the Current entry as well... not good
   QStringList slotlst = TContestApp::getContestApp() ->preloadBundle.getProfileEntries();
   QStringList pathlst;
   for ( int i = 0; i < slotlst.count(); i++ )
   {
      QString ent;
      TContestApp::getContestApp() ->preloadBundle.getStringProfile( slotlst[i], ent, "" );
      pathlst.append( ent );
   }
   int curSlot = 0;
   TContestApp::getContestApp() ->preloadBundle.getIntProfile( eppCurrent, curSlot );
   for ( int i = 0; i < slotlst.size(); i++ )
   {
      QString s = slotlst[ i ].left( 4 );
      if ( s == "List" )
      {
         int slotno = slotlst[ i ].mid( 4, 2 ).toInt() - 1; // even a 2 char number is a BIT excessive
         if ( slotno >= 0 )
         {
            addListSlot( pathlst[ i ], slotno, true );
         }
      }
      else
      {
         int slotno = slotlst[ i ].toInt() - 1;
         if ( slotno >= 0 )
         {
            addSlot( 0, pathlst[ i ], false, slotno );
         }
      }
   }
   BaseContestLog *ct = 0;

   if ( TContestApp::getContestApp() ->getContestSlotCount() )
   {
      if ( curSlot > 0 )
      {
         ct = TContestApp::getContestApp() ->contestSlotList[ curSlot - 1 ] ->slot;
      }
   }

   TContestApp::getContestApp() ->writeContestList();	// to clear the unopened and changed ones

   if ( conarg.size() )
   {
      // open the "argument" one last - which will make it current
      ct = addSlot( 0, conarg, false, -1 );
      TContestApp::getContestApp() ->writeContestList();	// or this one will not get included
   }

   if ( ct )
   {
      selectContest( ct, 0 );
   }
}
void TLogContainer::addListSlot( const QString &fname, int slotno, bool preload )
{
   // Is this the correct return type, or do we have an even more basic one? Or even a useful interface...

   // openFile ends up calling ContactList::initialise which then
   // calls TContestApp::insertList

   ContactList * list = TContestApp::getContestApp() ->openListFile( fname, slotno );
   if ( list && !preload )
   {

       if (!mShowOKCancelMessage(this, "Open List " + list->name + "?") )
       {
           TContestApp::getContestApp() ->closeListFile( list );
           list = 0;
       }
   }

   TContestApp::getContestApp() ->writeContestList();
   enableActions();
}

void TLogContainer::ListOpenActionExecute()
{
    // first choose file
//"Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"

    QString Filter = "Contact list files (*.csl);;"
                     "All Files (*.*)" ;

    QString fname = QFileDialog::getOpenFileName( this,
                       "Open Archive List",
                       "",
                       Filter
                       );
    if ( !fname.isEmpty() )
    {
        addListSlot( fname, -1, false );
    }
}
void TLogContainer::ManageListsActionExecute(  )
{
   TManageListsDlg manageListsDlg(this );
   manageListsDlg.exec();
   enableActions();
}
//---------------------------------------------------------------------------

void TLogContainer::ShiftTabRightActionExecute( )
{
   // We want to reorder the tabs so that this one goes right
   if ( !ui->ContestPageControl->currentWidget() )
      return ;
   int tno = ui->ContestPageControl->currentIndex();
   if ( tno < ui->ContestPageControl->count() - 1 )
   {
      ContestSlot * cs = TContestApp::getContestApp() ->contestSlotList[ tno ];
      int s = cs->slotno;

      ContestSlot *csp1 = TContestApp::getContestApp() ->contestSlotList[ tno + 1 ];
      int sp1 = csp1->slotno;

      TContestApp::getContestApp() ->contestSlotList[ tno ] = csp1;
      csp1->slotno = s;

      TContestApp::getContestApp() ->contestSlotList[ tno + 1 ] = cs;
      cs->slotno = sp1;

      TContestApp::getContestApp() ->writeContestList();

      ui->ContestPageControl->tabBar()->moveTab(tno, tno + 1);
//      tno++;
//      ui->ContestPageControl->setCurrentIndex(tno);

      enableActions();
   }
}
//---------------------------------------------------------------------------

void TLogContainer::ShiftTabLeftActionExecute( )
{
   //
   if ( !ui->ContestPageControl->currentWidget() )
      return ;
   int tno = ui->ContestPageControl->currentIndex();
   if ( tno > 0 )
   {
      ContestSlot * cs = TContestApp::getContestApp() ->contestSlotList[ tno ];
      int s = cs->slotno;
      ContestSlot *csm1 = TContestApp::getContestApp() ->contestSlotList[ tno - 1 ];
      int sm1 = csm1->slotno;
      TContestApp::getContestApp() ->contestSlotList[ tno ] = csm1;
      csm1->slotno = s;

      TContestApp::getContestApp() ->contestSlotList[ tno - 1 ] = cs;
      cs->slotno = sm1;

      TContestApp::getContestApp() ->writeContestList();

      //      ContestPageControl->ActivePage->PageIndex = tno; // BCB - this moves the tabs
      ui->ContestPageControl->tabBar()->moveTab(tno, tno - 1);
//      tno--;
//      ui->ContestPageControl->setCurrentIndex( tno);

      enableActions();
   }
}
void TLogContainer::selectContest( BaseContestLog *pc, BaseContact *pct )
{
    // we have double clicked on a contact in "other" or "archive" trees
    // so we want to (a) switch tabs and (b) go to that contact edit


    for ( int j = 0; j < ui->ContestPageControl->count(); j++ )
    {
        QWidget *ctab = ui->ContestPageControl->widget(j);
        if ( TSingleLogFrame * f = dynamic_cast<TSingleLogFrame *>( ctab ) )
        {
            if ( f->getContest() == pc )
            {
                ui->ContestPageControl->setCurrentIndex(j);         // This doesn't call ContestPageControlChange (see TPageControl::OnChange in  help)
                on_ContestPageControl_currentChanged(0);       // so the contest gets properly switched
                f->QSOTreeSelectContact( pct );         // which triggers edit on the contact
                return ;
            }
        }
    }
}
