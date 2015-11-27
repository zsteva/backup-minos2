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
#include "XMPPClient_pch.h"

#include <QLineEdit>
#include <QMessageBox>

#include <math.h>
#include <time.h>
#include "boost\format.hpp"
#include "boost\shared_ptr.hpp"

//#define _MSC_VER 2300
#include "sorted_vector.h"

#include "MinosLoggerEvents.h"

#include "validators.h"

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
#include "screencontact.h"
#include "matchcontact.h"
#include "ListContact.h"

#include "MinosTestImport.h"
#include "RunApp.h"

//#include "BandInfo.h"

#include "MShowMessageDlg.h"
#include "MMessageDialog.h"

#include "TreeUtils.h"
#include "DisplayContestContact.h"

#endif
