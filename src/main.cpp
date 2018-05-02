#include "header.hpp"
//фильтр(константный), функция print без auto
int main() {

	try {
		TokenContainer<TokenIterator, TokenIterator> t;
		/*auto k = t.end();
		std::cout << *k;
		std::cout << std::endl;;
		k = t.begin();
		std::cout << *k;
		while (k != t.end()) {
			k++;
			std::cout << *k << std::endl;
		}  */ 

		TokenSyntax(t.begin());


	} catch (TokenException& caught) {
		std::cout << caught.what() << std::endl;
	}
}
