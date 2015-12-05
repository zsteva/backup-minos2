#ifndef TBUNDLEFRAME_H
#define TBUNDLEFRAME_H

#include "logger_pch.h"
#include <QFrame>

namespace Ui {
class TBundleFrame;
}

class SettingsBundle;

class TBundleFrame : public QFrame
{
    Q_OBJECT

public:
    explicit TBundleFrame(QWidget *parent = 0);
    ~TBundleFrame();

private:
    Ui::TBundleFrame *ui;

   SettingsBundle *bundle;
   MinosItem<QString> *bname;

public:  		// User declarations
   void initialise();
   void initialise( const QString &cap, SettingsBundle *bundle, MinosItem<QString> *name );
   bool doEdit( );
   void enableBundle( bool );

   void BundleSectionChange(  );
   void BundleEditClick( );

};

#endif // TBUNDLEFRAME_H
