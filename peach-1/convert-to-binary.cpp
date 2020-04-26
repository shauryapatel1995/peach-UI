#include <iostream>
#include <fstream>
#include <bitset>
using namespace std; 
int main() {
    ifstream infile; 
    ofstream outfile;
    infile.open("sort-numbers");
    outfile.open("sort-large.txt");
    std::string ans;
    int count = 1;
    while(infile >> ans) {
        std::string final_string = std::bitset<16>(count).to_string();
        count++;
        final_string += std::bitset<32>(stoi(ans)).to_string();
        //cout << final_string << "\n";
        outfile << final_string << "\n";
    }

}