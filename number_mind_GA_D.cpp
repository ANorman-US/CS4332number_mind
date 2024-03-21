//Alexander Norman

#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>

#include <fstream>
#include <string>
#include <unordered_set>
#include <climits>
#include <chrono>
using namespace std;

struct individual{
    int num[12] = {0};//12 digit
    int f;//number of correct digits or fitness score
};

void readInput(vector<individual>&);//read input
void possibleNumbers(vector<vector<int>> &, const vector<individual> &);//create a vector of possible numbers by removing the digits of 0 score guesses
int calculateFitness(const int*, const vector<individual>&);
long long int arrayToDecimal(const int*);
void initPop(const int, const vector<individual>&, const vector<vector<int>>&, vector<individual>&);
void geneticAlgorithm(const int&, vector<individual>&, const vector<individual>&, const vector<vector<int>>&, const double mutationRate);

int main()
{
    /*auto start = chrono::high_resolution_clock::now();*/

    vector<individual> guesses;
    readInput(guesses);
    int populationSize = 30;
    double mutationRate = 0.025;
    int maxGenerations = 1000;//not used
    vector<vector<int>> pN (12, vector<int> (10));
    possibleNumbers(pN, guesses);
    vector<individual> population;
    initPop(populationSize, guesses, pN, population);//population generation
    geneticAlgorithm(maxGenerations, population, guesses, pN, mutationRate);

    /*auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << endl << duration.count();*/
}

void geneticAlgorithm(const int& maxGenerations, vector<individual>& population, const vector<individual>& guesses, const vector<vector<int>>& pN, const double mutationRate)
{
    int generationNum = 0;

    individual bestChild;
    bestChild.f = INT_MAX;

    //stagnation ignore for now
    //int sameGen = 0;
    while(bestChild.f != 0)
    {   
        double totalFitness = 0;
        //negative score good
        for(int i=0;i<population.size();i++)//total fitness (fitness is "worse" as the score increases)
            totalFitness += 1.0 / population[i].f;
        vector<double>individualProbability(population.size());//roulette table
        individualProbability[0] = (1 /(double)population[0].f) / totalFitness;
        for(int i=1;i<population.size();i++)
            individualProbability[i] = ((1 / (double)population[i].f ) / totalFitness) + individualProbability[i-1];
        
        //positive score good. was testing different fitness metrics
        /*
        for(int i=0;i<population.size();i++)
            totalFitness+= population[i].f;
        vector<double>individualProbability(population.size());
        individualProbability[0] = ((double)population[0].f / totalFitness);
        for(int i=1;i<population.size();i++)
            individualProbability[i] = ((double)population[i].f / totalFitness) + individualProbability[i-1];
        */

        //picking parents now
        vector<individual> children;
        for(int i=0;i<population.size();i++)//generate populationsize num of children
        {
            double randomDouble = (double) rand() / RAND_MAX;
            int parentOne[12];
            int parentTwo[12];

            //first parent
            for(int j=0;j<population.size();j++)
            {
                if(randomDouble < individualProbability[j])
                {
                    copy(begin(population[0].num), end(population[0].num), begin(parentOne));
                    break;
                }
            }
            //second parent
            bool parentTwoFound = false;//try to ensure second parent is different from first
            while(!parentTwoFound)
            {
                randomDouble = (double) rand() / RAND_MAX;
                
                for(int j=0;j<population.size();j++)
                {
                    if(randomDouble < individualProbability[j])
                    {
                        copy(begin(population[j].num), end(population[j].num), begin(parentTwo));
                        parentTwoFound = true;
                        break;
                    }
                }
                if(equal(begin(parentOne), end(parentOne), parentTwo))
                    parentTwoFound=false;
            }
            //crossover
            int randomInt = (rand()%11)+1;//crossover index
            individual child;
            if( (rand() % 2) == 0)//which parents goes first
            {
                for(int index = 0; index < randomInt; index++)
                    child.num[index] = parentOne[index];
                for(int index = randomInt; index < 12;index++)
                    child.num[index] = parentTwo[index];
            }
            else
            {
                for(int index = 0; index < randomInt; index++)
                    child.num[index] = parentTwo[index];
                for(int index = randomInt; index < 12;index++)
                    child.num[index] = parentOne[index];
            }
            //mutation 5-10%
            for(int j=0;j<12;j++)
            {
                randomDouble = (double) rand() / RAND_MAX;
                if(randomDouble < mutationRate)
                    child.num[j] = pN[j][rand() % pN[j].size()];//not guaranteed to change though
            }


            child.f = calculateFitness(child.num, guesses);

            children.push_back(child);

            if(child.f == 0)//if solution found, break
            {
                bestChild = child;
                break;
            }    
            //cout << arrayToDecimal(child.num) << " " << child.f << endl;

        }
        population = children;

    }
    for(int i=0;i<12;i++)
        cout << bestChild.num[i];
    //cout << " " << bestChild.f << endl;
}

void initPop(const int populationSize, const vector<individual>& guesses, const vector<vector<int>>& pN, vector<individual>& population)
{
    unordered_set<long long int> numbersAdded;
    for(int i=0;i<populationSize;i++)
    {
        individual ind;

        for(int j=0;j<12;j++)//adding random nums (according to permissible digits) to maximize fitness of first generation
            ind.num[j] = pN[j][rand() % pN[j].size()];

        if(numbersAdded.count(arrayToDecimal(ind.num)) == 0)//prevention of duplicates
        {
            numbersAdded.insert(arrayToDecimal(ind.num));
            ind.f = calculateFitness(ind.num, guesses);
            population.push_back(ind);

        }
        else
            i--;
    }
}

long long int arrayToDecimal(const int*num)
{
    long long int total = 0;
    for(int i=0;i<12;i++)
        total = 10 * total + num[i];
    return total;
}

int calculateFitness(const int* num, const vector<individual>& guesses)
{
    int fitness = 0;
    for(const auto& guess : guesses)
    {
        int count = 0;
        for(int j=0;j<12;j++)
        {
            if(guess.num[j] == num[j])
                count++;
        } 
        if(count > guess.f)//higher fitness score is worse. Increases with number of digits we are off by
            fitness+= count - guess.f;
        else
            fitness += guess.f - count;

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

    for(const auto& guess : guesses)//begin removal
    {
        if(guess.f == 0)
        {
            for(int j=0;j<12;j++)
            {
                int n = guess.num[j];
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