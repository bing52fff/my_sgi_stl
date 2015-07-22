#include <iostream>
#include "list"
using namespace std;
using ip::list;

int main()
{
	list<int> i;
	i.push_back(1);
	i.push_back(2);
	i.push_back(3);
	i.push_back(4);
	list<int> j;
	j.push_back(2);
	j.push_back(4);
	i.reverse();
	i.sort();
	list<int>::iterator it;
	for (it = i.begin(); it != i.end(); ++ it)
		cout << *it << " ";
	cout << endl;
	return 0;
}
