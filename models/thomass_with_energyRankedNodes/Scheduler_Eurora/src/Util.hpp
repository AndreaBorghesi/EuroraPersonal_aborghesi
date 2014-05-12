#ifndef Def_Util
#define Def_Util
#include "Job.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <ctime>
#include <math.h>
#include <sstream>
#include <time.h> 
#include <stdlib.h>
//#include "chrono.hpp"
//#include "boost/date_time/local_time/local_time.hpp"

using namespace std;

class Util
{
private:
	static time_t strTimeToTime(string str)
	{
		time_t t=0;
		vector<string> data=split(str,":");
		if(data.size()>3 || data.size()==0)
		{
			std::cout<<"ERROR Util::strHHMMToTime( "<<str<<" ) wrong format: Format accepted HH:MM"<<std::endl;
		} 
		else
		{
			//t+=std::atoi(data[0].c_str())*60*60+std::atoi(data[0].c_str())*60;
			for(int i=data.size()-1;i>=0;i--)
			{
				//t+=std::atoi(data[i].c_str())*pow(60,data.size()-1-i);
				t+=atoi(data[i].c_str())*pow(60,data.size()-1-i);
			}
		}
		return t;
	}
public:
	
	static int min(int a,int b)
	{
		return (a<b?a:b);
	}
	
	static int max(int a,int b)
	{
		return (a>b?a:b);
	}
	static vector<string> split(string str, string del)
	{
		vector<string> res;
		int end=0;
		while((end=str.find(del,0))>=0)
		{
			res.push_back(str.substr(0,end));
			str=str.substr(end+del.size(),str.size());
		}
		if(str.size()>0)
			res.push_back(str);
		return res;
	}
	
	static string timeHHMMToStr(time_t t)
	{
		stringstream ret("");
		int h=t/(60*60);
		int m=(t-h*60*60)/60;
		ret<<h<<":"<<m;
		return ret.str();
		
	}
	
	static string timeToStr(time_t t)
	{
		std::tm * ptm = std::localtime(&t);
		char buffer[32];
		
		std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);  
		string res(buffer);
		return res;
		/*stringstream ret("");
		int s=((int)t%60);
		//std::cout<<s<<std::endl;
		t=(int)t/60;
		int m=((int)t%60);
		t=(int)t/60;
		if(t<24)
		{
		if(t>0)
		ret<<t<<":"<<m<<":"<<s;
		else if(m>0)
		ret<<m<<":"<<s;
		else
		ret<<s;
		return ret.str();
		}	 
		int h=(int)t%24;
		t=(int)t/24;
		int y=1970;
		for(int i=0;t>0;i++)
		{
		t-=365;
		y++;
		if((i%4==0||i%100==0||i%400==0)&&t>0)
		{
		t-=1;
		}
			
		}
		t+=365;
		y--;
		int tPrec=t;
		int mesi=1;
		for(int i=0;t>30;i++)
		{
		mesi++;
		tPrec=t;
		if(i<=7)
		{
		if(i==2)
		t-=28;
		else if(i%2==0)
		t-=30;
		else
		t-=31;
		}
		else
		{
		if(i%2==0)
		t-=31;
		else
		t-=30;
		}
		}
		if(t<=0)
		t=tPrec;
		ret<<y<<"-"<<mesi<<"-"<<t<<" "<<h<<":"<<m<<":"<<s; 
		return ret.str();*/
		
	}
	static time_t strToTime(string str) 
	{
		//time_t now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		struct tm t;
		strptime(str.c_str(), "%Y-%m-%d %H:%M:%S", &t);
		t.tm_isdst = -1;
		return mktime(&t) ;
		//return now;
		/*if(str.compare("--")==0)
		return 0;
		vector<string> info=split(str," ");
		int s=strTimeToTime(info[1]);
		vector<string> data=split(info[0],"-");
		int anno=std::atoi(data[0].c_str());
		int r=0;
		for(int i=1970;i<anno;i++)
		{
		r+=365;
		if(i%4==0||i%100==0||i%400==0)
		{
		r+=1;
		}
		}
		int m=std::atoi(data[1].c_str());
		for(int i=1;i<m;i++)
		{
		if(i<=7)
		{
		if(i==2) r+=28;
		else if(i%2==0)r+=30;
		else r+=31;
		}
		else
		{
		if(i%2==0)r+=31;
		else r+=30;
		}	
		}
		int d=std::atoi(data[2].c_str())-1;
		r+=d;
		s+=r*24*60*60;
		return s;*/
	}
	
	static time_t strHHMMToTime(string str)
	{
		time_t t=0;
		vector<string> data=split(str,":");
		if(data.size()!=2)
		{
			std::cout<<"ERROR Util::strHHMMToTime( "<<str<<" ) wrong format: Format accepted HH:MM"<<std::endl;
		} 
		else
		{
			//t+=std::atoi(data[0].c_str())*60*60+std::atoi(data[1].c_str())*60;
			t+=atoi(data[0].c_str())*60*60+atoi(data[1].c_str())*60;
			/*for(int i=data.size()-1;i>=0;i--)
			{
			t+=std::atoi(data[i].c_str())*pow(60,data.size()-1-i);
			}*/
		}
		return t;
	}
};
#endif
