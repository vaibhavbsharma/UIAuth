#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <unistd.h>
#include <cstdlib>
using namespace std;

std::vector<std::string> &split(const std::string &s, char delim, vector<string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

double string_to_double( const std::string& s )
{
  std::istringstream i(s);
  double x;
  if (!(i >> x))
    return 0;
  return x;
}

int string_to_int( const std::string& s )
{
  std::istringstream i(s);
  int x;
  if (!(i >> x))
    return 0;
  return x;
}

double distanceCalculate(double x1, double y1, double x2, double y2)
{
  double x = x1 - x2;
  double y = y1 - y2;
  double dist;
  dist = pow(x,2)+pow(y,2);           //calculating distance by euclidean formula
  dist = sqrt(dist);                  //sqrt is function in math.h
  return dist;
}

struct TapdownEvent {
  double down_x,down_y,down_p,down_s;
  int down_t;
  double fling_x,fling_y,fling_p,fling_s;
  int fling_t;
};

//argv[0] - program name
//argv[1] - search string e.g. next_button
//argv[2] - search-filename e.g. app1.txt
//argv[3] - userID-<input_control> e.g. user1-button
int main(int argc,char *argv[]) {
  string input_file_name( string("user-all.txt"));
  string search_string(string("grep \"")+argv[1]+"\" "+argv[2]+" >"+input_file_name);
  system(search_string.c_str());
  //system("grep next_button * >user-button-all.txt");
  ifstream fin(input_file_name.c_str());
  //ifstream fin("user-button-all.txt");
  string output_file_name(string(argv[3])+"-downup.log");
  ofstream fout(output_file_name.c_str(),fstream::app);
  string s1,s2,s3,s4,line;
  int found_down=0;
  vector<TapdownEvent> event_info;
  TapdownEvent e;
  bool picker=false;
  while(getline(fin,line)) {
    if(line.find("Down")!=string::npos) {
      s1="";
      s2="";
      s3="";
      s4="";
      found_down=1;
      getline(fin,s1);
      getline(fin,s2);
      //cout<<line<<endl;
      //cout<<s1<<endl;
      //cout<<s2<<endl;
      continue;
    }
    if(line.find("Fling")!=string::npos) {
      found_down=0;
      string tmpstr;
      getline(fin,tmpstr);
      getline(fin,tmpstr);
      getline(fin,s3);
      getline(fin,s4);
      vector<string> v_s1,v_s2,v_s3,v_s4;
      v_s1=split(s1,' ',v_s1);
      int v_s1_sz = v_s1.size();
      ////cout<<tmpv[tmpv.size()-1]<<endl;
      v_s2=split(s2,' ',v_s2);
      int v_s2_sz=v_s2.size();
      ////cout<<tmpv[sz-5]<<" "<<tmpv[sz-4]<<" "<<tmpv[sz-3]<<" "<<tmpv[sz-1]<<endl;

      //cout<<line<<endl;
      //cout<<s3<<endl<<s4<<endl<<"****"<<endl;
      v_s3=split(s3,' ',v_s3);
      int v_s3_sz=v_s3.size();
      v_s4=split(s4,' ',v_s4);
      int v_s4_sz=v_s4.size();
      
	  
      e.down_x=string_to_double(v_s2[v_s2_sz-5].c_str());
      e.down_y=string_to_double(v_s2[v_s2_sz-4].c_str());
      e.down_p=string_to_double(v_s2[v_s2_sz-3].c_str());
      e.down_s=string_to_double(v_s2[v_s2_sz-1].c_str());
      e.down_t=string_to_int(v_s1[v_s1_sz-1].c_str());

      e.fling_x=string_to_double(v_s4[v_s4_sz-5].c_str());
      e.fling_y=string_to_double(v_s4[v_s4_sz-4].c_str());
      e.fling_p=string_to_double(v_s4[v_s4_sz-3].c_str());
      e.fling_s=string_to_double(v_s4[v_s4_sz-1].c_str());
      e.fling_t=string_to_int(v_s3[v_s3_sz-1].c_str());

      double normalized_down_x=e.down_x;
      //If this is a switch feature collection run
	  //normalize the x coordinate
	  /*int switch_size[]={300, 460, 523, 516, 471,\
	    486, 269, 407, 427, 334, 440, 391, 351, 490, \
		320, 304, 432, 466, 449, 600};
	  if(string(argv[1]).find("switch")!=string::npos) {
		  int switch_no=0;
		  string s(argv[1]);
		  int len=s.length();
		  if(s[len-3] == 'r') {//Single-digit
			  switch_no=s[len-2]-'0';
		  }
		  else {
			  switch_no=s[len-3]-'0';
			  switch_no*=10;
			  switch_no+=(s[len-2]-'0');
		  }
		  //cout<<"Found switch with no = "<<switch_no<<endl;
		  normalized_down_x = (e.down_x/switch_size[switch_no-1])*100;
	  }*/

	  if(string(argv[1]).find("picker")!=string::npos) {
		  picker=true;
	  }
      double direction = atan2(e.fling_y-e.down_y,e.fling_x-e.down_x);
      fout<<normalized_down_x<<" "<<e.down_y;
	  fout<<" "<<e.down_p<<" "<<e.down_s<<" ";
      fout<<(e.fling_x-e.down_x)<<" "<<(e.fling_y-e.down_y)<<" ";
      fout<<(e.fling_p-e.down_p)<<" "<<(e.fling_s-e.down_s)<<" ";
      fout<<distanceCalculate(e.down_x,e.down_y,e.fling_x,e.fling_y)<<" ";
      fout<<direction<<" "<<(e.fling_t - e.down_t)<<endl;
       
      //cout<<e.down_t<<" "<<e.down_x<<" "<<e.down_y<<" "<<e.down_p<<" "<<e.down_s<<endl;
      //cout<<e.fling_t<<" "<<e.fling_x<<" "<<e.fling_y<<" "<<e.fling_p<<" "<<e.fling_s<<endl;
      //cout<<"-- -- -- --"<<endl;

      s1="";
      s2="";
      s3="";
      s4="";
	  if(picker==true) break;	
    }
  }
  system(string("rm "+input_file_name).c_str());
}
