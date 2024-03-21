//Alexander Norman
//works for 18-19/21 test cases

#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>

#include <fstream>
#include <string>

using namespace std;

struct individual{
    int num[12] = {0};//12 digit
    int f;//number of correct digits or fitness score
};

void readInput(vector<individual>&);//read input
void possibleNumbers(vector<vector<int>> &, const vector<individual> &);//create a vector of possible numbers by removing the digits of 0 score guesses
int calculateFitness(const int*, const vector<individual>&);
void SA(const vector<vector<int>>&, const vector<individual>&);


int main()
{
    srand(time(nullptr));
    vector<individual> guesses;
    readInput(guesses);
    vector<vector<int>> pN (12, vector<int> (10));
    possibleNumbers(pN, guesses);
    SA(pN, guesses);
}

void SA(const vector<vector<int>>& pN, const vector<individual>& guesses)
{
    int extremeOne[]={0,0,0,0,0,0,0,0,0,0,0,0};
    int extremeTwo[]={9,9,9,9,9,9,9,9,9,9,9,9};
    float T = (calculateFitness(extremeOne, guesses) + calculateFitness(extremeTwo, guesses)) / 2;
    //reduction factor
    float a = 0.01;
    int state[12];
    for(int i=0;i<12;i++)//random initial state
        state[i] = pN[i][rand() % pN[i].size()];
    
    
    while (T > 0)
    {
        int fitness = calculateFitness(state, guesses);
        if(fitness == 0)
            break;
        bool neighborFound = false;
        while(neighborFound == false)
        {   
            int digitIndex = rand() %12;
            int temp[12];
            copy(begin(state), end(state), temp);

            while(equal(begin(state), end(state), temp) == 1)
            {
                digitIndex = rand()%12;
                temp[digitIndex] = pN[digitIndex][rand() % pN[digitIndex].size()];//random shift of a random digit until something actually changes
            }


            int tempFitness = calculateFitness(temp, guesses);
            float dE = tempFitness - fitness;

            if(dE < 0)//if fitness score went down (good)
            {
                copy(begin(temp), end(temp), state);
                neighborFound = true;
            }
            else
            {
                float P = exp(-1*abs(dE/T));
                float randomNum = (float) rand() / RAND_MAX;
                if (randomNum < P)
                {
                    copy(begin(temp), end(temp), state);
                    neighborFound = true;
                }
            }
            //otherwise no neighbor found, repeat without decrementing temp
        }
        for(int i=0;i<12;i++)
            cout << state[i];
        cout << " " << fitness << endl;
        T-=a;
    }
    for(int i=0;i<12;i++)
        cout << state[i];
    cout << " " << calculateFitness(state, guesses) << endl;
}

int calculateFitness(const int* num, const vector<individual>& guesses)
{
    int fitness = 0;
    for(int i=0;i<guesses.size();i++)
    {
        int count = 0;
        for(int j=0;j<12;j++)
        {
            if(guesses[i].num[j] == num[j])
                count++;
        }

        
        if(count > guesses[i].f)//higher fitness score is worse. Increases with number of digits we are off by
            fitness+= count - guesses[i].f;
        else
            fitness += guesses[i].f - count;
        

        /*
        if(count <= guesses[i].f)
            fitness+=count;
        else
            fitness+=(guesses[i].f) - (count - guesses[i].f);
        */

        
    }
    return fitness;
}

void possibleNumbers(vector<vector<int>> &pN, const vector<individual> &guesses)//if 12 digit number has 0 correct digits, remove all from 2d vector
{
    for(int i=0;i<12;i++)//init with 0-9 in each row(digit)
    {
        for(int j=0;j<10;j++)
            pN[i][j]=j;
    }

    for(int i=0;i<guesses.size();i++)//begin removal
    {
        if(guesses[i].f == 0)
        {
            for(int j=0;j<12;j++)
            {
                int n = guesses[i].num[j];
                auto it = find(pN[j].begin(), pN[j].end(), n);//iterator, begin end and n
                if(it != pN[j].end())
                    pN[j].erase(it);
            }
        }
    }
}

void readInput(vector<individual> &guesses)
{
    int n;

    ifstream inFile;
    inFile.open("guesses.txt");
    inFile >> n;
    inFile.ignore();

    //cin >> n;
    //cin.ignore();//ignore \n
    
    individual g;
    string input;

    for(int i=0;i<n;i++)
    {
        getline(inFile, input);
        //getline(cin, input);//read whole line

        string num = input.substr(0,12);
        for(int j=0;j<12;j++)
            g.num[j] = num[j] - '0';
        g.f = stoi(input.substr(13));
        guesses.push_back(g);
    }
}