#include "logger_pch.h"

#include "tsettingseditdlg.h"
#include "ui_tsettingseditdlg.h"
#include "enqdlg.h"

TSettingsEditDlg::TSettingsEditDlg(QWidget *parent, SettingsBundle *bundle) :
    QDialog(parent),
    ui(new Ui::TSettingsEditDlg),
    bundle(bundle),
    currSectionOnly(false)

{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    initialSection = bundle->getSection();

    QSettings settings;
    QByteArray geometry = settings.value("EntrySettings/geometry").toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);
}
void TSettingsEditDlg::ShowCurrentSectionOnly()
{
   currSectionOnly = true;
}
int TSettingsEditDlg::exec()
{
    showSections();
    showDetails();

    return QDialog::exec();
}

TSettingsEditDlg::~TSettingsEditDlg()
{
    delete ui;
}

void TSettingsEditDlg::showSections()
{
   ui->SectionsList->clear();
   QStringList sections;

   if (currSectionOnly)
   {
      sections.append(initialSection);
   }
   else
   {
      sections = bundle->getSections( );
   }

   setWindowTitle(windowTitle() + " - " + bundle->getBundle() + " for " + bundle->getSection()) ;

   int offset = 0;
   for ( int i = 0; i < sections.size(); i++ )
   {
      if ( sections[ i ] == bundle->getSection() )
         offset = i;
      ui->SectionsList->addItem( sections[ i ] );
   }
   ui->SectionsList->setCurrentRow(offset);
   showSection( );
}
//---------------------------------------------------------------------------
void TSettingsEditDlg::showSection()
{
   // Select this section to display on tree
   int offset = ui->SectionsList->currentRow();
   if (currSectionOnly)
   {
      bundle->openSection( initialSection );
      ui->OptionsTable->setVisible(true);
      showDetails();
   }
   else if ( offset > 0 )
   {
      QStringList sections = bundle->getSections( );
      QString sect = sections[ offset ];
      bundle->openSection( sect );
      ui->OptionsTable->setVisible(true);
      showDetails();
   }
   else
   {
      ui->OptionsTable->setVisible(false);
      bundle->openSection( noneBundle );
   }

   ui->NewSectionButton->setEnabled(!currSectionOnly);
   ui->DeleteButton->setEnabled(currSectionOnly?false:( offset > 0 ));
   ui->CopyButton->setEnabled(currSectionOnly?false:( offset > 0 ));
}
void TSettingsEditDlg::showDetails()
{
   ui->OptionsTable->clear();
   int offset = ui->SectionsList->currentRow();
   if ( offset > 0 || currSectionOnly)
   {
       // sections are all i = aaaaaaa
       QVector<int> entries = bundle->getBundleEntries();

       QStringList hints = bundle->getBundleHints( );

       QStringList labels;
       QStringList values;

       ui->OptionsTable->setColumnCount(1);
       ui->OptionsTable->setRowCount(entries.size());

       for ( int i= 0; i < entries.size(); i++ )
       {
           labels.append(bundle->displayNameOf( entries[ i ] ));

           QString val;
           bundle->getStringProfile( entries[ i ], val );

           QTableWidgetItem *it = new QTableWidgetItem(val);


          bool RO = bundle->isReadOnly(entries[ i ]);
          if (RO)
          {
             Qt::ItemFlags fl = it->flags();
             fl &= Qt::ItemIsEditable;
             it->setFlags(fl);
          }
          ui->OptionsTable->setItem(i, 0, it);
       }
       ui->OptionsTable->setVerticalHeaderLabels(labels);
   }
}
void TSettingsEditDlg::getDetails()
{
   int offset = ui->SectionsList->currentRow();
   if (( offset > 0 || currSectionOnly) && ui->OptionsTable->rowCount())
   {
      QVector<int> entries = bundle->getBundleEntries();
      for ( int r = 0; r < entries.size(); r++ )
      {
         QString val = ui->OptionsTable->item(r, 0)->text();
         bundle->setStringProfile( entries[ r ], val );
      }
   }
}

//---------------------------------------------------------------------------

void TSettingsEditDlg::on_NewSectionButton_clicked()
{
    getDetails();  // save what is set already
    QString Value = "new section";
    if ( enquireDialog( this, "Please give a name for the new section", Value ) )
    {
       bundle->newSection( Value );
       bundle->openSection( Value );
       showSections();
       showDetails();
    }
}

void TSettingsEditDlg::on_CopyButton_clicked()
{
    getDetails();  // save what is set already

    int offset = ui->SectionsList->currentRow();
    if ( offset > 0 && !currSectionOnly)
    {
       QString Value = "new section";
       if ( enquireDialog( this, "Please give a name for the new section", Value ) )
       {
          getDetails();  // save old section

          if ( bundle->dupSection( Value ) )
          {
             bundle->openSection( Value );
             showSections();
             showDetails();
          }
          else
          {
             MinosParameters::getMinosParameters() ->mshowMessage( "Section already exists", this );
          }
       }
    }
}

void TSettingsEditDlg::on_DeleteButton_clicked()
{
    //   getDetails();  // we are about to delete it - don't save it first!

    // delete the section - don't allow "<None>" to be deleted!
    // First, put up an "Are you sure?" message

    int offset = ui->SectionsList->currentRow();
    if ( offset == 0 )
       MinosParameters::getMinosParameters() ->mshowMessage( "You cannot delete the empty section!", this );
    else
       if ( MinosParameters::getMinosParameters() ->yesNoMessage( this, "Are you sure you want to delete the current section?" ) )
       {
          bundle->clearProfileSection( true );
          showSections();
          showDetails();
       }
}

void TSettingsEditDlg::on_CancelButton_clicked()
{
    bundle->openSection( initialSection );
    reject();
}

void TSettingsEditDlg::on_OKButton_clicked()
{
    getDetails();
    bundle->flushProfile();
    accept();
}

void TSettingsEditDlg::on_SectionsList_itemSelectionChanged()
{
    getDetails();  // save what is set already
    showSection();
}
void TSettingsEditDlg::doCloseEvent()
{
    QSettings settings;
    settings.setValue("EntrySettings/geometry", saveGeometry());
}
void TSettingsEditDlg::reject()
{
    doCloseEvent();
    QDialog::reject();
}
void TSettingsEditDlg::accept()
{
    doCloseEvent();
    QDialog::accept();
}
