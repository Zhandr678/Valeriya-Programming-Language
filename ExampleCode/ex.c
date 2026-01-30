#include <stdio.h>
#include <stdlib.h>

/*

property prop
{
	opt1: { int a; string s; }
	opt2: { int b; string ss; }
	opt3: { int c; string sss; }
	opt4: { prop rep; }
}

*/

enum prop_tag
{
	opt1_tag, opt2_tag, opt3_tag, opt4_tag
};

struct opt1;
struct opt2;
struct opt3;
struct opt4;

union prop
{
	prop_tag tag;

	opt1 _o1;
	opt2 _o2;
	opt3 _o3;
	opt4 _o4;
};

struct opt1
{
	int a;
	char* s;
};

struct opt2
{
	int b;
	char* ss;
};

struct opt3
{
	int c;
	char* sss;
};

struct opt4
{
	prop rep;
};

int main()
{
	


}