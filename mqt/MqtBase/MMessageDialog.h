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

#ifndef MMessageDialogH
#define MMessageDialogH 
//---------------------------------------------------------------------------

bool mShowYesNoMessage( QWidget *Owner, const std::string &mess );
bool mShowYesNoMessage( QWidget *Owner, const QString &mess );
bool mShowOKCancelMessage(QWidget *Owner, const std::string &mess );
bool mShowOKCancelMessage(QWidget *Owner, const QString &mess );
void mShowOKMessage( QWidget *Owner, const std::string &mess );
void mShowOKMessage( QWidget *Owner, const QString &mess );

bool sShowYesNoMessage( QWidget *Owner, const std::string &mess );
bool sShowYesNoMessage( QWidget *Owner, const QString &mess );
bool sShowOKCancelMessage( QWidget *Owner, const std::string &mess );
bool sShowOKCancelMessage( QWidget *Owner, const std::string &mess );
void sShowOKMessage( QWidget *Owner, const std::string &mess );
void sShowOKMessage( QWidget *Owner, const QString &mess );

//---------------------------------------------------------------------------
#endif
