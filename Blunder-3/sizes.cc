#include <iostream>

using namespace std;

int main() {
    char c = 'C';
    char* cArr = "hallodkfhgbalksdjfbalskdfhbj";
    string str = cArr;

    cout << "char  : " << c << ", size: " << sizeof(c) << endl;
    cout << "char* : " << cArr << ", size: " << sizeof(cArr) << endl;
    cout << "string: " << str << ", size: " << sizeof(str) << endl; 
}