#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>

using namespace std;

std::string escapeLine ( std::string orig )
{
   string retme;
   for ( unsigned int i = 0; i < orig.size(); i++ )
   {
      switch ( orig[i] )
      {
         case ' ': // Strip out the final linefeed.
            break;
         case '\n': // Strip out the final linefeed.
            break;
         case '\r': // Strip out the final linefeed.
            break;
         default:
            retme += orig[i];
      }
   }
   return retme;
}

int main ( int argc, char ** argv )
{
   string filenamein, filenameout;

   if ( argc > 1 )
      filenamein = argv[ 1 ];
   else
   {
      // Not enough arguments
      fprintf ( stderr, "Usage: %s <file to convert.in> [ <output file name.h> ]\n", argv[0] );
      exit ( -1 );
   }

   if ( argc > 2 )
      filenameout = argv[ 2 ];
   else
   {
      string new_ending = ".h";
      filenameout = filenamein;
      std::string::size_type pos;
      pos = filenameout.find ( ".in" );
      if ( pos == std::string::npos )
         filenameout += new_ending;
      else
         filenameout.replace ( pos, new_ending.size(), new_ending );
   }

   printf ( "Converting \"%s\" to \"%s\"\n", filenamein.c_str(), filenameout.c_str() );

   ifstream filein ( filenamein.c_str(), ios::in );
   ofstream fileout ( filenameout.c_str(), ios::out );

   if ( !filein.good() )
   {
      fprintf ( stderr, "Unable to open input file %s\n", filenamein.c_str() );
      exit ( -2 );
   }
   if ( !fileout.good() )
   {
      fprintf ( stderr, "Unable to open output file %s\n", filenameout.c_str() );
      exit ( -3 );
   }

   // Write the file.
   fileout << "#define GITVER \"- \"";

   while ( filein.good() )
   {
      string buff;
      if ( getline ( filein, buff ) )
      {
         fileout << "\"" << escapeLine ( buff ) << "\"" << endl;
         break;
      }
   }

   filein.close();
   fileout.close();

   return 0;
}
