// C++ program to print words in a sentence
#include <bits/stdc++.h>
using namespace std;


void removeDupWord(string str,vector<string>&use)
{
    string word = "";
    for (auto x : str)
    {
        if (x == ' ')
        {
            use.push_back(word);
            word = "";
        }
        else
        {
            word = word + x;
        }
    }
            use.push_back(word);
}

// Driver code
int main()
{
    string str;
    while (1)
    {
        vector<string>use;

        cin >> str;
        removeDupWord(str,use);

        for(auto it:use){
            cout<<it<<endl;
        }
    }
    return 0;
}
