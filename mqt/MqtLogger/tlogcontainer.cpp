#include "logger_pch.h"

#include <QFontDialog>
#include <QFileDialog>

#include "tlogcontainer.h"
#include "ui_tlogcontainer.h"

#include "VHFList.h"
#include "contestdetails.h"

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

}

TLogContainer::~TLogContainer()
{
    delete ui;
}

void TLogContainer::closeEvent(QCloseEvent *event)
{
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
    CloseAllAction = newAction("Close all Contest", &TabPopup, SLOT(CloseAllActionExecute()));
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
    ShiftTabLeftAction = newAction("Shift Tab Left", &TabPopup, SLOT(ShiftTabLeftActionExecute()));
    ShiftTabRightAction = newAction("Shift Tab Right", &TabPopup, SLOT(ShiftTabRightActionExecute()));
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
           addSlot( &pced, FileName, false, -1 );
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
    QString InitialDir = getDefaultDirectory( false );
    // generate a default filename for a new contest
    QString nfileName( "C");

    dtg d( true );      // get time now
    nfileName += d.getDate( DTGLOG );

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

    QString initName = InitialDir + "/" + nfileName + letter + ".minos";
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
                          suggestedfName,
                          "Minos contest files (*.Minos)",
                          0,
                          QFileDialog::DontConfirmOverwrite
                                                      );
       if ( !fileName.isEmpty() )
       {
          suggestedfName = fileName;
          QDir r;
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
    if ( !fname.isEmpty() )
    {
        ContestDetails pced(this );
        addSlot( &pced, fname, false, -1 );   // not automatically read only
    }
}

void TLogContainer::ListOpenActionExecute()
{

}
void TLogContainer::ManageListsActionExecute()
{

}

void TLogContainer::ContestDetailsActionExecute()
{

}

void TLogContainer::FileCloseActionExecute()
{

}

void TLogContainer::CloseAllActionExecute()
{

}

void TLogContainer::CloseAllButActionExecute()
{

}

void TLogContainer::ExitActionExecute()
{

}
void TLogContainer::MakeEntryActionExecute()
{

}

void TLogContainer::ShiftTabLeftActionExecute()
{

}

void TLogContainer::ShiftTabRightActionExecute()
{

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

void TLogContainer::on_ContestPageControl_currentChanged(int /*index*/)
{
    //
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
         if ( contest->needsExport() )      // imported from an alien format (e.g. .log)
         {
            QString expName = f->makeEntry( true );
            if ( expName.size() )
            {
               closeSlot(tno, true );
               addSlot( 0, expName, false, -1 );
            }
         }
         removeCurrentFile( fname );
      }
   }
   TContestApp::getContestApp() ->writeContestList();
   enableActions();
   return contest;
}
void TLogContainer::closeSlot(int t, bool addToMRU )
{
   if ( t )
   {
      TSingleLogFrame * f = findLogFrame(t);
      if ( addToMRU )
      {
         BaseContestLog * contest = f->getContest();
         QString curPath = contest->cfileName;
         setCurrentFile( curPath );
      }
      f->closeContest();    // which should close the contest
      ui->ContestPageControl->removeTab(t);
//      ContestPageControlChange( this );
      enableActions();
   }
}
TSingleLogFrame *TLogContainer::findLogFrame(int t)
{
   // we need to find the embedded frame...
   // now ONLY used in closeSlot!
   if ( t >= 0 )
      return 0;
   TSingleLogFrame * f = dynamic_cast<TSingleLogFrame *>( ui->ContestPageControl->currentWidget() );
   return f;
}
