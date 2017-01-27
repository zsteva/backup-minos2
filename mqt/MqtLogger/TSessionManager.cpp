#include "logger_pch.h"

#include "tlogcontainer.h"

#include "TSessionManager.h"
#include "ui_TSessionManager.h"

TSessionManager::TSessionManager(TLogContainer *parent) :
    QDialog(parent), tlc(parent),
    ui(new Ui::TSessionManager)
{
    ui->setupUi(this);
    QSettings settings;
    QByteArray geometry = settings.value("TSessionManager/geometry").toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);

    parseSessions();
    showSessions();
    showSession(sessionList.currentSession);
}
TSessionManager::~TSessionManager()
{
    delete ui;
}


void TSessionManager::enableButtons()
{

}
void TSessionManager::parseSessions()
{
    TContestApp *app = TContestApp::getContestApp();
    QStringList sessions = tlc->getSessions();
    SettingsBundle &preloadBundle = app ->logsPreloadBundle;

    for ( int i = 0; i < sessions.size(); i++ )
    {
        QString sessName = sessions[i];
       if ( sessName == app->currSession )
          sessionList.currentSession = i;
       Session sess;
       sess.sessionName = sessName;
       sessionList.sessions.push_back(sess);

       preloadBundle.openSection(sessName);
       QStringList slotlst = preloadBundle.getProfileEntries();

       if (slotlst.count())
       {
           for ( int j = 0; j < slotlst.count(); j++ )
           {
               QString ent;
               preloadBundle.getStringProfile( slotlst[j], ent, "" );

               if (slotlst[j] == "CurrentLog")
               {
                   sessionList.sessions[i].currentEntry = ent.toInt();
                   continue;
               }
               // get each value
               sessionList.sessions[i].entries.push_back(ent);
           }
       }
    }
}
void TSessionManager::writeSessions()
{
    TContestApp *app = TContestApp::getContestApp();
    SettingsBundle &preloadBundle = app ->logsPreloadBundle;
    QStringList sessionlst = app ->logsPreloadBundle.getSections();

    for (int i = 0; i < sessionlst.count(); i++)
    {
        preloadBundle.openSection(sessionlst[i]);
        preloadBundle.clearProfileSection(true);
    }
    preloadBundle.flushProfile();
    preloadBundle.openSection(app->preloadsect);
    preloadBundle.setStringProfile(eppSession, sessionList.sessions[sessionList.currentSession].sessionName);
    preloadBundle.setStringProfile(eppDefSession, app ->defaultSession );

    for (int i = 0; i < sessionList.sessions.count(); i++)
    {
        preloadBundle.openSection(sessionList.sessions[i].sessionName);
        for (int j = 0; j < sessionList.sessions[i].entries.count(); j++)
        {
            preloadBundle.setStringProfile(QString::number(j), sessionList.sessions[i].entries[j]);
        }
        preloadBundle.setIntProfile(eppCurrent, sessionList.sessions[i].currentEntry);
    }
    preloadBundle.flushProfile();
}

void TSessionManager::showSessions()
{
    ui->sessionsList->clear();
    ui->sessionEntries->clear();

    for ( int i = 0; i < sessionList.sessions.count(); i++ )
    {
       ui->sessionsList->addItem( sessionList.sessions[ i ].sessionName );
    }
    ui->sessionsList->setCurrentRow(sessionList.currentSession);
}

void TSessionManager::showSession(int sess)
{
    ui->sessionEntries->clear();

    int rowCount = sessionList.sessions[sess].entries.count();
    ui->sessionEntries->setColumnCount(1);
    ui->sessionEntries->setRowCount(rowCount);

    for ( int i = 0; i < rowCount; i++ )
    {
        ui->sessionEntries->setItem(i, 0, new QTableWidgetItem(sessionList.sessions[sess].entries[i]));
    }
    ui->sessionEntries->selectRow(sessionList.sessions[sess].currentEntry);

    sessionList.currentSession = sess;
}

void TSessionManager::on_OKButton_clicked()
{
    // need to get rewrite the set of sessions
    writeSessions();
    // and go back and select the new current one
    accept();
}

void TSessionManager::on_cancelButton_clicked()
{
    // ignore it all... just let our structures die.
    reject();
}
void TSessionManager::doCloseEvent()
{
    QSettings settings;
    settings.setValue("TSessionManager/geometry", saveGeometry());
}
void TSessionManager::reject()
{
    doCloseEvent();
    QDialog::reject();
}
void TSessionManager::accept()
{
    doCloseEvent();
    QDialog::accept();
}


void TSessionManager::on_NewSessionButton_clicked()
{
    // create and enter a new session
}

void TSessionManager::on_CloneSessionButton_clicked()
{
    // copy the current session with  new name, enter it
}

void TSessionManager::on_DeleteSessionButton_clicked()
{
    // delete the current session, move on one
}

void TSessionManager::on_AddEntryButton_clicked()
{
    // add an entry to the current session beow the current entry
    //- need to put up a file selection dialog
}

void TSessionManager::on_DeleteEntryButton_clicked()
{
    // delete the current entry from the current session
}

void TSessionManager::on_MoveEntryUpButton_clicked()
{
    // shift the current entry up one
}

void TSessionManager::on_MoveEntryDownButton_clicked()
{
    // shift the current entry down one
}

void TSessionManager::on_RenameSessionButton_clicked()
{
    // change the session name
}

void TSessionManager::on_sessionsList_currentRowChanged(int currentRow)
{
    showSession(currentRow);
}

void TSessionManager::on_sessionEntries_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem */*previous*/)
{
    if (current)
    {
        sessionList.sessions[sessionList.currentSession].currentEntry = current->row();
    }
}
