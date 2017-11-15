#ifndef MONITORMAIN_H
#define MONITORMAIN_H

#include "base_pch.h"
#include <QAction>
#include <QMenu>

#include "MonitoredLog.h"
#include "MonitoringFrame.h"


class MonitoringFrame;
class MonitoredLog;

class MonitoredStation
{
   private:

      // Plus we want the control (keyer) state, frequency, ?bandmap etc

   public:
      QString stationName;
      PublishState state;
      QVector< MonitoredLog *> slotList;

      MonitoredStation()
      {}
      ~MonitoredStation()
      {}   // need to delete slots...
};
struct MonitoredStationCmp
{
   QString cmpstr;
   MonitoredStationCmp( const QString &s ) : cmpstr( s )
   {}

   bool operator() ( MonitoredStation * &s1 ) const
   {
      return s1->stationName.compare( cmpstr, Qt::CaseInsensitive ) == 0;
   }
};
struct MonitoredLogCmp
{
   QString cmpstr;
   MonitoredLogCmp( const QString &s ) : cmpstr( s )
   {}

   bool operator() ( MonitoredLog * &s1 ) const
   {
      return s1->getPublishedName().compare(cmpstr, Qt::CaseInsensitive ) == 0;
   }
};
//=============================================================================================
enum NodeType{entRoot, entServer, entLog};
class TreeNode
{
    NodeType ntype;
protected:
    QString NodeName;
    int childNo;
    TreeNode *parentItem;
    MonitoredLog *mlog;

public:
    MonitorMain *monmain;

    TreeNode(NodeType sn, TreeNode *parent, QString name, MonitorMain *mm):
        ntype(sn), NodeName(name), parentItem(parent), monmain(mm), mlog(0)
    {
        if (parent)
            parent->nodes.push_back(this);
    }
    TreeNode(NodeType sn, TreeNode *parent, MonitoredLog *log, MonitorMain *mm):
        ntype(sn), NodeName(log->getPublishedName()), parentItem(parent), monmain(mm), mlog(log)
    {
        if (parent)
            parent->nodes.push_back(this);
    }
    virtual ~TreeNode()
    {
        clear();
    }

    virtual NodeType GetNodeType()
    {
        return ntype;
    }
    QVector<TreeNode *> nodes;
    int childNumber() const;

    int find( const TreeNode *t ) const;

    void clear();

    virtual QString Name ( void )
    {
        return NodeName;
    }
    virtual QString data( int column ) = 0;

    TreeNode *parent();
    TreeNode *child( int number );
    int childCount() const;
    MonitoredLog *getLog()
    {
        return mlog;
    }
    void setLog(MonitoredLog *l)
    {
        mlog = l;
    }

};
class RootTreeNode:public TreeNode
{
public:
    RootTreeNode(MonitorMain *mm):TreeNode(entRoot, 0, "Root", mm)
    {

    }
    virtual QString data( int column );
};
class ServerTreeNode:public TreeNode
{
public:
    ServerTreeNode(TreeNode *parent, QString name):TreeNode(entServer, parent, name, parent->monmain)
    {
    }
    virtual QString data( int column );
};
class LogTreeNode:public TreeNode
{
public:
    LogTreeNode(TreeNode *parent, MonitoredLog *log):TreeNode(entLog, parent, log, parent->monmain)
    {
    }
    virtual QString data( int column );
};
class MonitorTreeModel: public QAbstractItemModel
{
    TreeNode *rootData;
public:
    MonitorTreeModel();
    ~MonitorTreeModel();
    void setRoot(  TreeNode *root );
    void clear();

    QVariant data( const QModelIndex &index, int role ) const Q_DECL_OVERRIDE;
    QVariant headerData( int section, Qt::Orientation orientation,
                         int role ) const Q_DECL_OVERRIDE;
    QModelIndex index( int row, int column,
                       const QModelIndex &parent = QModelIndex() ) const Q_DECL_OVERRIDE;
    QModelIndex parent( const QModelIndex &index ) const Q_DECL_OVERRIDE;

    int rowCount( const QModelIndex &parent = QModelIndex() ) const Q_DECL_OVERRIDE;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const Q_DECL_OVERRIDE;
    TreeNode *getItem( const QModelIndex &index ) const;
};

namespace Ui {
class MonitorMain;
}

class MonitorMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit MonitorMain(QWidget *parent = 0);
    ~MonitorMain();

    QVector<MonitoredStation *> stationList;

    void logMessage( const QString &s );
//    void notifyCallback( bool err, MinosRPCObj *mro, const QString &from );
//    void loggerSubscribeClientCallback( bool err, MinosRPCObj *mro, const QString &from );

    void closeTab(MonitoringFrame *tab);

private slots:
    void on_closeButton_clicked();
    void on_notify(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from );
    void on_serverCall( bool err, QSharedPointer<MinosRPCObj>, const QString &from );

    void onStdInRead(QString cmd);

    void on_monitorTimeout();

    void on_monitorTree_doubleClicked(const QModelIndex &index);
    void on_monitorSplitter_splitterMoved(int /*pos*/, int /*index*/);
    void on_closeMonitoredLog();
    void on_contestPageControl_customContextMenuRequested(const QPoint &pos);
    void CancelClick();

    void on_contestPageControl_tabCloseRequested(int index);

private:
    Ui::MonitorMain *ui;

    QAction *newAction( const QString &text, QMenu *m, const char *atype );
    QMenu TabPopup;
    QAction *closeMonitoredLog;


    int splitterHandleWidth;

    virtual void closeEvent(QCloseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void moveEvent(QMoveEvent *event) override;
    virtual void changeEvent( QEvent* e ) override;

    StdInReader stdinReader;
    QString localServerName;

    MonitorTreeModel *treeModel;

    QTimer *monitorTimer;

    bool syncstat;
    void syncStations();
    void addSlot( MonitoredLog *ct );
    MonitoringFrame *findCurrentLogFrame();
    MonitoringFrame *findContestPage( BaseContestLog *ct );
};

#endif // MONITORMAIN_H
