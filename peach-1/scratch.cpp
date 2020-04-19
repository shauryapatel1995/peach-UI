# include <iostream>
# include <fstream>
# include <limits>
# include <cmath>

//using namespace std;

using namespace std; 
int main() 
{ 
    float a = sqrt(2); 
    float b = sqrt(-2); 
      
    // Returns false as a  
    // is real number 
    isnan(a)? cout << "Its NaN" << endl: 
              cout << "Its a real number" << endl; 
      
    // Returns true as b is  NaN 
    isnan(b)? cout << "Its NaN" << endl: 
              cout << "Its a real number" << endl; 
      
    return 0;     
} 
// int main () {
// 	float myTest = sqrt(-2);
// 	if(isnan(myTest)) {
// 		cout << "This if statement works!\n";
// 	}
// 	// string x;
// 	// ifstream myFile;
// 	// myFile.open("memory.txt");
// 	// int sum = 0;
// 	// while (myFile >> x)
// 	// {
// 	// 	cout << '\n' << x << '\n'; 
// 	// 	string tagStr = x.substr(0,4);
// 	// 	cout << tagStr << '\n';
// 	// 	string indexStr = x.substr(4,4);
// 	// 	cout << indexStr << '\n';
// 	// 	string valueStr = x.substr(8,8);
// 	// 	cout << valueStr << '\n';
// 	// 	int tag = stoi(tagStr, nullptr, 2);
// 	// 	cout << tag << '\n';
// 	// 	int index = stoi(indexStr, nullptr, 2);
// 	// 	cout << index << '\n';
// 	// 	int value = stoi(valueStr, nullptr, 2);
// 	// 	cout << value << '\n';
// 	// 	cout << "\n this is my tag: " << tag << ", this is my index: " << index << ", this is my value: " << value << "\n";
// 	// 	//sum = sum + atoi(x);	
// 	//}
// 	return 0; 
// }


//cout << "this is my NAN: " << myTest << "\n";