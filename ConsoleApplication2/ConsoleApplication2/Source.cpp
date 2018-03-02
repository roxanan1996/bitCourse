#include <iostream>
#include <map>
#include <list>
#include <string>

int main() {

	std::map<std::string, std::list<int> > m;

	int n;

	std::cin >> n;

	for (int i = 0; i < n; ++i) {
		int input;
		std::cin >> input;
		std::string s;
		if (input % 2) {
			s = "impar";
		}
		else {
			s = "par";
		}

		if (m.find(s) == m.end()) {
			std::list<int> l;
			l.push_back(input);
			m.insert(std::pair<std::string, std::list<int> >(s, l));
		}
		else {
			m[s].push_back(input);
		}
	}

	for (std::list<int>::iterator it = m["par"].begin(); it != m["par"].end(); it++) {
		std::cout << *it << "\n";
	}

	for(std::list<int>::iterator it = m["impar"].begin(); it != m["impar"].end(); it++) {
		std::cout << *it << "\n";
	}

	return 0;

}