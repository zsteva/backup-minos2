#ifndef TSESSIONMANAGER_H
#define TSESSIONMANAGER_H

#include <QDialog>
#include <QTableWidget>

class TLogContainer;

namespace Ui {
class TSessionManager;
}

class Session
{
public:
    QString sessionName;
    int currentEntry;
    QVector<QString> entries;
};

class SessionVector
{
public:
    int currentSession;
    QVector<Session> sessions;
};

class TSessionManager : public QDialog
{
    Q_OBJECT

public:
    explicit TSessionManager(TLogContainer *parent);
    ~TSessionManager();
    QString currSession;

private slots:

    void on_OKButton_clicked();

    void on_cancelButton_clicked();

    void on_NewSessionButton_clicked();

    void on_DeleteSessionButton_clicked();

    void on_AddEntryButton_clicked();

    void on_DeleteEntryButton_clicked();

    void on_MoveEntryUpButton_clicked();

    void on_MoveEntryDownButton_clicked();

    void on_CloneSessionButton_clicked();

    void on_RenameSessionButton_clicked();

    void on_sessionsList_currentRowChanged(int currentRow);

    void on_sessionEntries_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

private:
    Ui::TSessionManager *ui;
    void doCloseEvent();
    void parseSessions();
    void writeSessions();
    void showSessions();
    void showSession(int sess);
    void enableButtons();

    TLogContainer *tlc;
    SessionVector sessionList;


public Q_SLOTS:
    virtual void accept() override;
    virtual void reject() override;
};

#endif // TSESSIONMANAGER_H
