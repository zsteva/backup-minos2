/*====================================================================================
    This file is part of AdjQt, the QT based version of the RSGB
    contest adjudication software.
    
    AdjQt and its predecessor AdjSQL are Copyright 1992 - 2016 Mike Goodey G0GJV 
 
    AdjQt is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AdjQt is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with AdjQt in file gpl.txt.  If not, see <http://www.gnu.org/licenses/>.
    
======================================================================================*/

#include "logger_pch.h"

#include "enqdlg.h" 
//---------------------------------------------------------------------------
// dialog for simple enquiries that have a simple question/answer format
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool enquireDialog (QWidget *owner, const QString &prompt, QString &Value )
{
    bool ok;
    QString text = QInputDialog::getText( owner, "Please supply value",
                                          prompt, QLineEdit::Normal,
                                          Value, &ok );
    if ( ok && !text.isEmpty() )
        Value = text;

    return ok;
}

bool enquireDialog (QWidget *owner, const QString &prompt, int &Value, int minval, int maxval )
{
    bool ok;
    int val = QInputDialog::getInt(owner, "Please supply value",
                                          prompt, Value, minval, maxval,
                                           1, &ok );
    if ( ok )
        Value = val;

    return ok;
}

