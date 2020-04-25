// C++ program to print words in a sentence 
#include <bits/stdc++.h> 
#include <map>
using namespace std; 
  
void removeDupWord(string str) 
{ 
   string word = ""; 
   vector<vector<string>> instructions;
   for (auto x : str) 
   { 
       if (x == ' ') 
       { 
           cout << word << endl;
           word = ""; 
       } 
       else
       { 
           word = word + x; 
       } 
   }  
} 

string testMe()
{
    string myString = "test me";
    return myString;
}

void testVector()
{
    vector<int> myVec;
    for (int i=0; i<5; i++)
    {
        myVec.push_back(i);
        cout << myVec[i] << endl;
    }
    // time to delete
    cout << "Time to delete myVec[2] \n";
    myVec.erase(myVec.begin()+2);
    for (int j=0; j <myVec.size(); j++)
    {
        cout << myVec[j] << endl;
    }
}

string myFullBin(string myBin)
{
    string compBin = "empty";
    for (int ii=0; ii<32; ii++)
    {
        // do something
    }
}

// Driver function 
int main() 
{ 
    string str = "Geeks for Geeks"; 
    removeDupWord(str); 
    cout << "did I get here?\n";
    string bin = bitset<8>(6).to_string();
    cout << bin << endl;
    string myString = testMe();
    cout << myString << endl;
    testVector();

    // string binary bitwise OR
    string myBin = "000000001010101110111";
    int numZeros = 32 - myBin.size();
    string strZeros(numZeros, '0');
    string myFinalBin = myBin + strZeros;
    cout << myFinalBin << endl;
    cout << numZeros << endl;
    cout << "my first bit : " << myBin[0] << " second bit : " << myBin[1] << endl; 
    cout << "This is testbin : " << '1' - '0' << endl;
    return 0; 
} 
