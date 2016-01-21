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

#ifndef BandListH
#define BandListH

//---------------------------------------------------------------------------

class BandInfo
{
        QString type;
    public:
        double flow;
        double fhigh;
        QString wlen;
        QString uk;
        QString cabrillo;
        QString adif;
        QString reg1test;

        void setType ( const QString &t );
        QString getType();
};
class TiXmlElement;
class BandList
{
        bool parseBand ( TiXmlElement * e );
    public:
        BandList();
        ~BandList();
        std::vector<BandInfo> bandList;
        bool parseFile ( const QString &bandFile );
        bool findBand ( const QString &freq, BandInfo & );
        bool findBand ( int freq, BandInfo & );

        static BandList &getBandList();

};
#endif
