/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
#include "XMPP_pch.h"
#pragma hdrstop

#pragma package(smart_init)

//---------------------------------------------------------------------------
__fastcall TMyRCVersion::TMyRCVersion()
      : FhInstance( 0 ), FInfoFrom( ifEXE ), FVData( 0 )
{}
//---------------------------------------------------------------------------
__fastcall TMyRCVersion::~TMyRCVersion()
{
   if ( FVData )
      delete[] FVData;
   FVData = 0;
}
//---------------------------------------------------------------------------
void __fastcall TMyRCVersion::setValue( String )
{
   ::MessageBeep( 0 );
}
//---------------------------------------------------------------------------
void __fastcall TMyRCVersion::SetInstance( HINSTANCE n )
{
   FhInstance = n;
   if ( FhInstance )
      FInfoFrom = ifDLL;
   else
      FInfoFrom = ifEXE;
   initialise();
}
//---------------------------------------------------------------------------
void __fastcall TMyRCVersion::initialise( void )
{
   delete[] FVData;
   FVData = 0;

   DWORD fvHandle;
   unsigned int vSize;
#if defined (_UNICODE)
   wchar_t appFName[ 255 ];
   wchar_t * subBlockName = new wchar_t[ 255 ];
#else
   char appFName[ 255 ];
   char * subBlockName = new char[ 255 ];
#endif
   HINSTANCE hInstance = 0; // default to application
   switch ( FInfoFrom )
   {
      case ifDLL:
         hInstance = FhInstance;
         break;
      case ifPackage:
         hInstance = HInstance; // DLL Module Instance for package
         break;
   }
   int nLen = ::GetModuleFileName( hInstance, appFName, 255 );
   appFName[ nLen ] = '\0';
#if !defined (_UNICODE)
   OemToChar( appFName, appFName );
#endif
   DWORD dwSize = ::GetFileVersionInfoSize( appFName, &fvHandle );
   if ( dwSize )
   {
      FVData = ( void * ) new char[ dwSize ];
      if ( ::GetFileVersionInfo( appFName, fvHandle, dwSize, FVData ) )
      {
         // Copy string to buffer so if the -dc compiler switch(Put constant strings in code segments)
         // is on VerQueryValue will work under Win16.  This works around a problem in Microsoft's ver.dll
         // which writes to the string pointed to by subBlockName.
         //
         #if defined (_UNICODE)
            wcscpy(subBlockName, _T("\\VarFileInfo\\Translation"));
         #else
            strcpy(subBlockName, _T("\\VarFileInfo\\Translation"));
         #endif
         if ( !::VerQueryValue( FVData, subBlockName, ( void * * ) & TransBlock, &vSize ) )
         {
            delete[] FVData;
            FVData = 0;
         }
         else
            // Swap the words so sprintf will print the lang-charset in the correct format.
            //
            *TransBlock = MAKELONG( HIWORD( *TransBlock ), LOWORD( *TransBlock ) );
      }
   }
   delete [] subBlockName;
}
//---------------------------------------------------------------------------
void __fastcall TMyRCVersion::SetInfoFrom( eInfoFrom e )
{
   if ( FInfoFrom == e )
      return ;

   FInfoFrom = e;
   initialise();
}
//---------------------------------------------------------------------------
#if defined(_UNICODE)
String __fastcall TMyRCVersion::getValue(const wchar_t * itemName)
#else
String __fastcall TMyRCVersion::getValue(const char * itemName)
#endif
{
  String itemValue;
  if (FVData)
  {
    #if defined(_UNICODE)
#warning do we need the parts newed as for ascii?


      unsigned int vSize;
      wchar_t subBlockName[255];
      wchar_t * ItemValue = 0;
      swprintf(subBlockName, _T("\\StringFileInfo\\%08lx\\%s"), TransBlock,(LPTSTR)itemName);
      if ( ::VerQueryValue(FVData, subBlockName,(void * *)&ItemValue, &vSize) )
      {
         wchar_t *temp = new wchar_t[vSize + 1];
         wcsncpy(temp, ItemValue, vSize);
         temp[vSize] = 0;
         itemValue = WideString(temp);
         delete [] temp;
      }
    #else
      unsigned int vSize;
      char * subBlockName = new char[ 255 ];
      char * ItemValue = new char[ 2000 ];
      char *p1 = &ItemValue[ 0 ];

      ItemValue[ 0 ] = '\0';
      if ( FVData )
      {
         sprintf( subBlockName, "\\StringFileInfo\\%08lx\\%s", *TransBlock, ( LPSTR ) itemName );
         if ( ::VerQueryValue( FVData, subBlockName, ( void * * ) & ItemValue, &vSize ) )
            itemValue = ItemValue;
      }
      delete [] subBlockName;
      delete [] p1;
    #endif
  }

  return itemValue;
}
//---------------------------------------------------------------------------
String __fastcall TMyRCVersion::GetLegalCopyright( void )
{
   return getValue( _T("LegalCopyright") );
}
//---------------------------------------------------------------------------
String __fastcall TMyRCVersion::GetCompanyName( void )
{
   return getValue( _T("CompanyName") );
}
//---------------------------------------------------------------------------
String __fastcall TMyRCVersion::GetFileDescription( void )
{
   return getValue( _T("FileDescription") );
}
//---------------------------------------------------------------------------
String __fastcall TMyRCVersion::GetFileVersion( void )
{
   return getValue( _T("FileVersion") );
}
//---------------------------------------------------------------------------
String __fastcall TMyRCVersion::GetInternalName( void )
{
   return getValue( _T("InternalName") );
}
//---------------------------------------------------------------------------
String __fastcall TMyRCVersion::GetLegalTrademarks( void )
{
   return getValue( _T("LegalTrademarks") );
}
//---------------------------------------------------------------------------
String __fastcall TMyRCVersion::GetOriginalFilename( void )
{
   return getValue( _T("OriginalFilename") );
}
//---------------------------------------------------------------------------
String __fastcall TMyRCVersion::GetProductName( void )
{
   return getValue( _T("ProductName") );
}
//---------------------------------------------------------------------------
String __fastcall TMyRCVersion::GetProductVersion( void )
{
   return getValue( _T("ProductVersion") );
}
//---------------------------------------------------------------------------
String __fastcall TMyRCVersion::GetComments( void )
{
   return getValue( _T("Comments") );
}
//---------------------------------------------------------------------------
String __fastcall TMyRCVersion::GetSpecialBuild( void )
{
   return getValue( _T("SpecialBuild") );
}
//---------------------------------------------------------------------------
String __fastcall TMyRCVersion::GetMajorVersion( void )
{
   String s = GetFileVersion();
   // s = major.minor.release.build
   int n = s.Pos( '.' );
   if ( n )
      return s.SubString( 1, n - 1 );
   return s;
}
//---------------------------------------------------------------------------
String __fastcall TMyRCVersion::GetMinorVersion( void )
{
   String s = GetFileVersion();
   int nStart = s.Pos( '.' );
   if ( !nStart )
      return s;
   s[ nStart ] = ' ';
   int nEnd = s.Pos( '.' );
   return s.SubString( nStart + 1, nEnd - 1 - nStart );
}
//---------------------------------------------------------------------------
String __fastcall TMyRCVersion::GetRelease( void )
{
   String s = GetFileVersion();
   int nStart = s.Pos( '.' );
   if ( !nStart )
      return s;
   s[ nStart ] = ' ';
   nStart = s.Pos( '.' );
   if ( !nStart )
      return s;
   s[ nStart ] = ' ';
   int nEnd = s.Pos( '.' );
   return s.SubString( nStart + 1, nEnd - 1 - nStart );
}
//---------------------------------------------------------------------------
int TMyRCVersion::getOSVersion( std::string &sos)
{
    OSVERSIONINFOEX v;
    char os[80];

    os[0] = '\0';

	// Try GetVersionEx with size of OSVERSIONINFOEX
    // NOTE: OSVERSIONINFOEX structure used here is not compatible
    // with SDK headers from Visual C++ 6 -- headers from later
    // versions of Visual Studio are OK, as are those from Feb 2003
    // platform SDK (which is compatible with VC6)
    memset(&v, 0, sizeof(OSVERSIONINFOEX));
    v.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    if( GetVersionEx((OSVERSIONINFO*)(&v))==0 )
	{
		// If that failed, try again with size of OSVERSIONINFO
		v.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx((OSVERSIONINFO*)(&v));
	}

	// All versions of Windows that support Minos should report
	// VER_PLATFORM_WIN32_NT for their PlatformId .. anything else
	// is not (currently) documented. (December 2012)
	//
	// These are the documented Server and Workstation versions of
	// Windows (since 2000 ... NT4 and Win9x not listed)
	//
	// major   minor    S/W       Version
	//   6       2       W        Windows 8
	//   6       2       S        Windows Server 2012
	//   6       1       W        Windows 7
	//   6       1       S        Windows Server 2008 R2
	//   6       0       S        Windows Server 2008
	//   6       0       W        Windows Vista
	//   5       2       S        Windows Server 2003 (R1 and R2)
	//   5       2       W        Windows XP 64-Bit Edition
	//   5       1       W        Windows XP
	//   5       0       W        Windows 2000
	//   5       0       S        Windows 2000 Server

    int ios = v.dwMajorVersion * 1000 + v.dwMinorVersion;

    if (v.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS)
    {
        if (v.dwMajorVersion==4)
		{
			if (v.dwMinorVersion==0)
				sprintf(os, "Win95 %s", v.szCSDVersion );
			else if (v.dwMinorVersion==10)
				sprintf(os, "Win98 %s", v.szCSDVersion);
			else if (v.dwMinorVersion==90)
				sprintf(os, "WinMe %s", v.szCSDVersion);
			else
				sprintf(os, "Win95+ %s minor version %d", v.szCSDVersion, v.dwMinorVersion);
		}
    }
    else if (v.dwPlatformId==VER_PLATFORM_WIN32_NT)
    {
        if (v.dwMajorVersion==5)
		{
			if (v.dwMinorVersion==0)
			{
				if( v.wProductType==VER_NT_WORKSTATION )
				{
					sprintf( os, "Windows 2000 Workstation %s build %ld", v.szCSDVersion, v.dwBuildNumber );
				}
				else
				{
					sprintf( os, "Windows 2000 Server %s build %ld", v.szCSDVersion, v.dwBuildNumber );
				}
			}
			else if (v.dwMinorVersion==1)
				sprintf(os, "Windows XP %s build %ld", v.szCSDVersion, v.dwBuildNumber);
			else if (v.dwMinorVersion==2)
			{
				// NT 5.2 is either XP x64 or Server 2003
				if( v.wProductType==VER_NT_WORKSTATION )
				{
					sprintf(os, "Windows XP x64 %s build %ld", v.szCSDVersion, v.dwBuildNumber);
				}
				else
				{
					// Let's NOT call it "Windows .NET server" -- nobody else does!
					sprintf(os, "Windows Server 2003 %s build %ld", v.szCSDVersion, v.dwBuildNumber);
				}
			}
			else
				sprintf(os, "Windows 2000 or later %s minor version %d build %ld", v.szCSDVersion, v.dwMinorVersion, v.dwBuildNumber);
		}
		else if( v.dwMajorVersion==6 )
		{
			if( v.dwMinorVersion==0 )
			{
				if( v.wProductType==VER_NT_WORKSTATION )
				{
					sprintf(os, "Windows Vista %s build %ld", v.szCSDVersion, v.dwBuildNumber);
				}
				else
				{
					sprintf(os, "Windows 2008 server %s build %ld", v.szCSDVersion, v.dwBuildNumber);
				}
			}
			else if( v.dwMinorVersion==1 )
			{
				if( v.wProductType==VER_NT_WORKSTATION )
				{
					sprintf(os, "Windows 7 %s build %ld", v.szCSDVersion, v.dwBuildNumber);
				}
				else
				{
					sprintf(os, "Windows 2008 server R2 %s build %ld", v.szCSDVersion, v.dwBuildNumber);
				}
			}
			else if( v.dwMinorVersion==2 )
			{
				if( v.wProductType==VER_NT_WORKSTATION )
				{
					sprintf(os, "Windows 8 %s build %ld", v.szCSDVersion, v.dwBuildNumber);
				}
				else
				{
					sprintf(os, "Windows 2012 server %s build %ld", v.szCSDVersion, v.dwBuildNumber);
				}
			}
			else
			{
				sprintf( os, "Windows NT 6.%d %s %s build %ld",
					v.dwMinorVersion,
					(v.wProductType==VER_NT_WORKSTATION) ? "Workstation " : "Server " ,
					v.szCSDVersion,
					v.dwBuildNumber );
			}
		}
        else
		{
			sprintf( os, "Windows NT %d.%d %s %s build %ld",
				v.dwMajorVersion,
				v.dwMinorVersion,
				(v.wProductType==VER_NT_WORKSTATION) ? "Workstation " : "Server " ,
				v.szCSDVersion,
				v.dwBuildNumber );
		}
    }
    else
	{
		// PlatformId is neither WIN32_WINDOWS nor WIN32_NT
        sprintf(os, "Windows build %ld", v.dwBuildNumber);
	}
   sos = std::string(os);
   return ios;
}
