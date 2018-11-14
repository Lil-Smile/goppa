#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <ctime>
#include <algorithm>
#include "galois.hpp"
#include "matrix.hpp"
#include "crypto.hpp"
#include "bch.hpp"
#include "goppa.hpp"
#include "mceliece.hpp"
#include "simple_construction.hpp"
#include "diagonal_construction.hpp"


using namespace std;
using namespace galois;
using namespace matrix;
using namespace crypto;
using namespace bch;
using namespace goppa;
using namespace mceliece;
using namespace simple_construction;
using namespace diagonal_construction;


typedef Galois gal;
typedef Matrix<gal> mgal;

void init(int& n) {
	ifstream inconfig(".config");
	string outFile;
	int randSeed;
	inconfig >> outFile >> n >> randSeed;
	inconfig.close();

	ofstream oflog(".log");

	if (randSeed < 0) {
		randSeed = time(0);
	}
	srand(randSeed);
	oflog << "srand = " << randSeed << "\n";

	if (outFile[0] != '-') {
		freopen(outFile.c_str(), "w", stdout);
	}

	oflog.close();
}

mgal str_to_mgal(const string &s) {
	int k = (int)s.size() * 8;
	mgal x(1, k);
	for (int i = 0; i < k; i++) {
		x(0, i) = Galois(((s[i / 8] >> (i % 8)) & 1), 3);
	}
	return x;
}

string mgal_to_str(const mgal &x) {
	stringstream ss;
	char c = 0;
	int k = x.size(1);
	for (int i = 0; i < k; ++i) {
		c |= (x(0, i).isZero() ? 0 : 1) << (i % 8);
		if ((i + 1) % 8 == 0) {
			ss << c;
			c = 0;
		}
	}
	if (k % 8) {
		ss << c;
	}
	return ss.str();
}

void test_goppa(string s) {
	int n;
	init(n);
	Goppa goppa(".goppa01");
	Goppa_public* public_key;
	Goppa_private* private_key;
	goppa.generate_pair((Crypto_public_key**)&public_key, (Crypto_private_key**)&private_key);

	//	Galois::set_binary_mode(false);

	//string s = "Hello, World!";

	mgal beforeEncode = str_to_mgal(s);
	mgal y = public_key->encode(beforeEncode);
	cout << "Encoded:     " << y;
	y = public_key->add_errors(y);
	delete public_key;
	cout << "With errors: " << y;

	mgal x = private_key->decode(y);
	cout << "Decoded: " << x;// << endl;
	delete private_key;

	cout << "Converted: " << mgal_to_str(x) << "\n";
}


int main() {
	string s;
	cout << "Enter the string: ";
	cin >> s;
	test_goppa(s);
	system("pause");
	return 0;
};

