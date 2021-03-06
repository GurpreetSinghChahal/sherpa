#include "AMEGIC++/String/String_Library.H"
#include "ATOOLS/Org/Run_Parameter.H"
#include <fstream>
#include "ATOOLS/Org/Shell_Tools.H"
#include "ATOOLS/Org/Message.H"
#include <stdio.h>

using namespace AMEGIC;
using namespace ATOOLS;
using namespace std;


String_Library::String_Library(int mode):m_mode(mode)
{

}

void String_Library::UpdateConfigure(std::string pathID)
{
  msg_Debugging()<<"String_Library::UpdateConfigure("<<pathID<<") called :"<<std::endl;

  string cnf("/configure.in");
  string mkam("/Makefile.am");
  unsigned int hit=pathID.find("/");
  string base=pathID.substr(0,hit);
  string subdirname=pathID.substr(hit+1);
  string name=rpa->gen.Variable("SHERPA_CPP_PATH")+string("/Process/Amegic/")+base+cnf;
  if (!IsFile(name)) {
    msg_Tracking()<<"   file "<<name<<" does not exist, create it."<<endl;


    ofstream file(name.c_str());

    file<<"dnl Process this file with autoconf to produce a configure script."<<endl;
    file<<"AC_INIT("<<base<<",1.0)"<<endl; 
    file<<"AM_INIT_AUTOMAKE"<<endl;
    file<<"AM_DISABLE_STATIC"<<endl;
    file<<"AC_PREFIX_DEFAULT("<<ATOOLS::rpa->gen.Variable("SHERPA_CPP_PATH")
	<<"/Process/Amegic)"<<endl;
    file<<"m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])"<<endl;
    file<<"dnl Checks for programs."<<endl;
    file<<"AC_PROG_INSTALL"<<endl;
    file<<"AC_PROG_MAKE_SET"<<endl;
    file<<"AC_PROG_CXX"<<endl;
    file<<"AM_PROG_LIBTOOL"<<endl;
    file<<"AC_OUTPUT( "<<'\\'<<endl;
    file<<"\t"<<subdirname<<"/Makefile "<<'\\'<<endl;
    file<<"\tMakefile )"<<endl;

    CreateExtraFiles(rpa->gen.Variable("SHERPA_CPP_PATH")+string("/Process/Amegic/")+base);
  } 
  else {
    ifstream from(name.c_str());
    ofstream to((name+string(".tmp")).c_str());  

    string buffer;
    for (;from;) {
      getline(from,buffer);
      if (buffer.find("\tMakefile )")!=string::npos) {
	to<<"\t"<<subdirname<<"/Makefile "<<'\\'<<endl;
      }
      to<<buffer<<endl;
    }
    from.close();
    to.close();

    Move(name+".tmp",name);
  }
  
  name=rpa->gen.Variable("SHERPA_CPP_PATH")+string("/Process/Amegic/")+base+mkam;
  if (!IsFile(name)) {
    msg_Tracking()<<"   file "<<name<<" does not exist, create it."<<endl;

    ofstream file(name.c_str());

    file<<"MAKE = make "<<endl;
    file<<"SUBDIRS = "<<'\\'<<endl;
    file<<"\t"<<subdirname<<endl;
  }
  else {
    ifstream from(name.c_str());
    ofstream to((name+string(".tmp")).c_str());  

    string buffer;
    for (;from;) {
      getline(from,buffer);
      to<<buffer<<endl;
      if (buffer.find("SUBDIRS")!=string::npos) {
	to<<"\t"<<subdirname<<" "<<'\\'<<endl;
      }
    }
    from.close();
    to.close();

    Move(name+".tmp",name);
  }

}

void String_Library::CreateExtraFiles(std::string path) 
{
  const int ln=4;
  string names[ln];
  names[0]=path+string("/NEWS");
  names[1]=path+string("/README");
  names[2]=path+string("/AUTHORS");
  names[3]=path+string("/ChangeLog");
  for (int i=0;i<ln;++i) {
    ofstream file(names[i].c_str());
    file.close();
  }

}

void String_Library::AddToMakefileAM(string makefilename,string pathID,string fileID)
{
  msg_Debugging()<<"String_Library::AddToMakefileAM("<<makefilename<<","
			<<pathID<<","<<fileID<<")"<<endl;

  unsigned int hit=pathID.find("/");
  string base=pathID.substr(0,hit);
  string subdirname=pathID.substr(hit+1);

  if (!IsFile(makefilename)) {
    ofstream file(makefilename.c_str());

    file<<"lib_LTLIBRARIES = libProc_"<<subdirname<<".la"<<endl;
    file<<"libProc_"<<subdirname<<"_la_SOURCES = "<<'\\'<<endl;
    file<<"\t"<<fileID<<".C"<<endl;
    file<<"CURRENT_SHERPASYS ?= "<<ATOOLS::rpa->gen.Variable("SHERPA_INC_PATH")<<endl;
    file<<"INCLUDES = -I$(CURRENT_SHERPASYS)"<<endl;
    file<<"DEFS     = "<<endl;
    file<<"noinst_HEADERS = V.H"<<endl;
  }
  else {
    ifstream from(makefilename.c_str());
    ofstream to((makefilename+string(".tmp")).c_str());  

    string buffer;
    string key=string("libProc_"+subdirname+"_la_SOURCES");
    for (;from;) {
      getline(from,buffer);
      to<<buffer<<endl;
      if (buffer.find(key)!=string::npos) {
	to<<"\t"<<fileID<<".C"<<'\\'<<endl;
      }
    }
    from.close();
    to.close();

    Move(makefilename+".tmp",makefilename);
  }
}



void String_Library::InitMakefile(string pathID)
{
  UpdateConfigure(pathID);
  return;
}

void String_Library::Replace(string& buffer,const string& search,const string& replace)
{
  int minpos=0;
  while (SingleReplace(buffer,search,replace,minpos));
}

int String_Library::SingleReplace(string& buffer,
				  const string& search,const string& replace,int& minpos)
{
  int pos= buffer.find(search,minpos);
  if (pos==-1) return 0;
  minpos=pos+replace.length();
  buffer = buffer.substr(0,pos)+replace+buffer.substr(pos+search.length());
  return 1;
}

void String_Library::Copy(string sfrom,string sto)
{
  ifstream from;
  ofstream to;
  
  from.open(sfrom.c_str());
  to.open(sto.c_str()); 

  char ch;
  while(from.get(ch)) to.put(ch);
  from.close();
  to.close();  

  //kill tmp
  remove(sfrom.c_str());
}

int String_Library::IsFile(string &filename)
{
  ifstream from;
  from.open(filename.c_str());
  if (from) return 1;
  return 0;
}

int String_Library::Search(string file,string search)
{  

  ifstream from;
  //search name  
  from.open(file.c_str());

  char buffer[buffersize];

  for(;from;) {
    from.getline(buffer,buffersize);    
    if (string(buffer).find(string(search))!=string::npos) {
      from.close();
      return 1;
    }
  }
  from.close();
  return 0;
}

void String_Library::AddToMakefile(string makefilename,string pathID,string fileID)
{
  // add also to makefile am!
  AddToMakefileAM(makefilename+string(".am"),pathID,fileID);

  return;
  if (IsFile(makefilename)==0) {
    cerr<<makefilename.c_str()<<" is not available !"<<endl;
    return;
  }

  if (Search(makefilename,string(fileID)+string(".C"))) return;

  ofstream to;  
  ifstream from;


  from.open(makefilename.c_str()); 
  to.open((makefilename+string(".tmp")).c_str());

  char buffer[buffersize];

  string pID;
  pID=pathID;
  for (short int i=pathID.length()-1;i>=0;i--) {
    if (pathID[i]=='/') {
      pID    = pathID.substr(i+1);
      break;
    }
  }

  string lib = string("libProc_")+pID;

  for(;from;) {
    from.getline(buffer,buffersize);
    if (string(buffer).find(lib+string("_la_SOURCES"))==0) {
      if (string(buffer).find(string("\\"))==string::npos) {
	//no backslash
	to<<buffer<<"\\"<<endl;
	to<<"\t"<<(fileID+string(".C")).c_str()<<endl; 
      }
      else {
	to<<buffer<<endl;
	to<<"\t"<<(fileID+string(".C")).c_str()<<" \\"<<endl; 
      }
    }
    else {
      if (string(buffer).find(lib+string("_la_OBJECTS"))==0) {
	if (string(buffer).find(string("\\"))==string::npos) {
	  //no backslash
	  to<<buffer<<"\\"<<endl;
	  to<<"\t"<<(fileID+string(".lo")).c_str()<<endl; 
	}
	else {
	  to<<buffer<<endl;
	  to<<"\t"<<(fileID+string(".lo"))<<" \\"<<endl; 
	}
      }
      else to<<buffer<<endl;
    }
  }
  from.close();
  to.close();

  //copy back
  this->Copy(makefilename+string(".tmp"),makefilename);
}
