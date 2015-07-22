#include "deque"
using namespace ip;
using namespace std;

int main()
{
	deque<int> dq(10, 1);
	deque<int>::iterator it;
	for (it = dq.begin(); it != dq.end(); ++ it)
		cout << *it << " ";
	cout << endl;
	return 0;
}
