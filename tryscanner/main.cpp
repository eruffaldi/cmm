#include <iostream.h>
#include "..\scan.h"
#include "..\keywords.h"

class MyScan: public CScan
{
public:
	MyScan(char *cp, KeywordEntry *k, int n)
		: CScan(cp, k, n) {}
	virtual void Error(ScanError, char * more);
};

void MyScan::Error(ScanError r, char * more)
{
	CScan::Error(r, more);
	cout << "\nERRORE: " << r << ": " << more << endl;
}

int main(int argc, char * argv[]) 
{
	cout << "Scanner Tester\n";
	char * cp;
	if(argc == 1) {
		//cout << "Required source C++ file\n";
		//return -1;
		cp = "c:\\prj\\cmm\\tryscanner\\scansrc.cpp";
	}
	else
		cp = argv[1];
	MyScan scanner(cp, KeywordList, sizeof(KeywordList)/sizeof(KeywordList[0]));

	int lineno = 0;
	while(scanner) {
		const Token & r = scanner.next();
		if(r.isEof())
			break;
		if(lineno != scanner.lineno()) {
			cout << endl;
			lineno = scanner.lineno();
		}
		cout << r << ' ';		
	}
	return 0;
}
