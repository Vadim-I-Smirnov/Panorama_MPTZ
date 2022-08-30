#include <iostream>
#include <tchar.h>
#include <io.h>
#include <fstream>
#include <cstdlib>
#include <windows.h>
#include <filesystem>
namespace fs = std::filesystem;
using namespace std;

struct SmallInfo
{
	unsigned int info[4];
};

struct MPTZSmallHdr
{
	unsigned int MPTZ;
	unsigned int hdr;
	unsigned long long size;
};


void STD_tstringInTchar(_TCHAR *tstr, string sstr)
{
    #ifdef _UNICODE
	wcscpy(tstr, sstr.c_str());
    #else
    strcpy(tstr, sstr.c_str());
    #endif
}

bool Fileexists(const string& name)
{
if (FILE *file = fopen(name.c_str(), "r"))
	{
		fclose(file);
		return true;
	} else {
		return false;
	}
}

void Extractfilename(const string& name, string& filepath, string& filename, string& fileext)
{
   int i;
   filepath = "";
   filename = name;
   fileext ="";

   std::size_t found = name.find_last_of("/\\");
   if (found!=std::string::npos) {
	  filepath = name.substr(0,found+1);
	  filename = name.substr(found+1);
   }

   found = filename.find_last_of(".");
   if (found!=std::string::npos) {
	  fileext = filename.substr(found+1);
	  filename = filename.substr(0,found);
   }

}

string Elementstr(const string& text, unsigned short int element, string separator)
{
	string maintext=text+separator;
	string stext=text;
	unsigned short int index=1;
	std::size_t found_f=0;

	if (element<1) {
	   element=1;
	}

	if (separator.length()>0) {
	   std::size_t found_l=maintext.find(separator);
	   while (index<element) {
		 found_f=found_l+separator.length();
		 found_l=maintext.find(separator,found_f);
		 if (found_l==std::string::npos) {
			index=element;
			found_l=found_f;
		 }
		 index++;
	   }
	   stext=maintext.substr(found_f,found_l-found_f);
	}

	return stext;
}

void Savetofile(ifstream& f, string& filename, unsigned long long size)
{

   string ffiledir, ffilename, ffileext;

   Extractfilename(filename, ffiledir, ffilename, ffileext);
   char* buffer[10000];

   fs::create_directories(ffiledir);

   ofstream outf(filename, ios::binary);


   unsigned long long ssize=size / sizeof(buffer);
   int asize=size % sizeof(buffer);

   for (unsigned long int i = 0; i < ssize; i++) {
	  f.read((char*)&buffer,sizeof(buffer));
	  outf.write((char*)&buffer,sizeof(buffer));
   }

   f.read((char*)&buffer,asize);
   outf.write((char*)&buffer,asize);

   outf.close();

}

int _tmain(int argc, _TCHAR* argv[])
{

	_TCHAR* fname;
	string ffilename="";
	string ffilepath="";
	string ffileext="";
	string ffiledir="";
	string tempfilename="";


	//setlocale(LC_ALL, "Russian");
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);

	cout << "MPTZ unpack v0.2 (20220810) smirnov.vi@ntcexp.ru " << endl;


	if (argc>1) {
	  string fullfilename = argv[1];
	  if (Fileexists(fullfilename)) {
		  cout << "Unpack file " <<  argv[1] <<endl;


		  MPTZSmallHdr mptz_hdr, mptz_s_hdr;
		  int i;

		  //open file
		  ifstream fmptz(argv[1], ios::binary);
		  if (fmptz) {

			//Extract
			Extractfilename(fullfilename, ffilepath, ffilename, ffileext);
			if (ffileext.length()==0) {
				 ffiledir="_";
			}

			if (ffilepath.length()>0) {
				 ffiledir=ffilepath+ffiledir+ffilename;
			}  else {ffiledir=ffiledir+ffilename;}

			//STD_tstringInTchar(fname,ffilename);

			cout << "Create directory " << ffiledir;
			fs::create_directory(ffiledir);
			cout << " Ok."  <<endl;

			fmptz.read((char*)&mptz_hdr,sizeof(mptz_hdr));

			SmallInfo *mptz_info = new SmallInfo [mptz_hdr.size];
			for (i = 0; i < mptz_hdr.size; i++) {
				fmptz.read((char*)&mptz_info[i],16);
			}

			fmptz.read((char*)&mptz_s_hdr,sizeof(mptz_s_hdr));
			tempfilename=ffiledir+"\\"+ffilename+".mpt";
			cout << "Create MPT file " << tempfilename;
			Savetofile(fmptz,tempfilename, mptz_s_hdr.size);

			//open mpt file
			ifstream fmpt(tempfilename, ios::in);
			if (fmpt) {

			   cout << " Ok."  <<endl;
			   string *path_info = new string [mptz_hdr.size];
			   unsigned long long findex=0;
			   string temptext, path_text;
			   std::size_t found;

			   for (i = 0; i < mptz_hdr.size; i++) {
				  temptext = to_string(i);
				  path_info[i]=ffilename+temptext;
			   }

			   while (getline(fmpt,temptext)&&(findex<mptz_hdr.size))
			   {
				   std::transform(temptext.begin(), temptext.end(), temptext.begin(), ::tolower);
				   found=temptext.find("path");
				   if (found!=std::string::npos)
				   {
					   path_text=Elementstr(temptext,2,"=");
					   if (path_text.length()>0)
					   {
						  path_info[findex]=path_text;
					   }
					   findex++;
				   }
			   }

			   fmpt.close();


			   for (i = 0; i < mptz_hdr.size; i++) {
					fmptz.read((char*)&mptz_s_hdr,sizeof(mptz_s_hdr));
					tempfilename=ffiledir+"\\"+path_info[i];
					cout << "Create file " << tempfilename;
					Savetofile(fmptz,tempfilename, mptz_s_hdr.size);
					cout << " Ok."  <<endl;
			   }

			   delete [] path_info;


			} else {cout << " error."  <<endl;}

			delete [] mptz_info;

			fmptz.close();
			cout << "Ok."  <<endl;
		  } else {  cout <<  " error." <<endl;}
	  }  else {
		  cout << "File " <<  argv[1] << " is not exists!" <<endl;
	  }




	} else {
		cout << "Example:     mptz2 filename.mptz" <<endl;
	}
}
