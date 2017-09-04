/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#ifndef BASE_PCH_H
#define BASE_PCH_H
#include "XMPP_pch.h"

#include <QApplication>
#include <QMainWindow>

#include <QDialog>
#include <QTimer>

#include <QLineEdit>
#include <QMessageBox>
#include <QStyledItemDelegate>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QStyle>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QSortFilterProxyModel>


template <class itemtype >inline bool operator < (const MapWrapper<itemtype> &key1, const MapWrapper<itemtype> &key2)
{
    itemtype *i1 = key1.wt.data();
    itemtype *i2 = key2.wt.data();
    return *i1 < *i2;
}
template <class itemtype >inline bool operator == (const MapWrapper<itemtype> &key1, const MapWrapper<itemtype> &key2)
{
    return key1.wt.data() == key2.wt.data();
}


#include <math.h>
#include <time.h>

#include "fileutils.h"
#include "MinosParameters.h"
#include "ProfileEnums.h"
#include "mwin.h"
#include "cutils.h"
#include "latlong.h"
#include "calcs.h"

#include "mults.h"
#include "contest.h"
#include "list.h"
#include "contacts.h"
#include "ScreenContact.h"
#include "MatchContact.h"
#include "ListContact.h"
#include "MatchCollection.h"
#include "MinosLoggerEvents.h"
#include "validators.h"

#include "rigcontrolcommonconstants.h"
#include "MinosTestImport.h"

#include "MShowMessageDlg.h"
#include "MMessageDialog.h"

#include "TreeUtils.h"
#include "DisplayContestContact.h"

#include "waitcursor.h"

#endif
