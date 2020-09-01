#include <bits/stdc++.h>
#include <fstream>
using namespace std;
struct Item
{
    int vl;
    int wt;
};

int knapSack(int W, Item *it, int n)
{
    int i, w;
    int K[n+1][W+1];
    for (i = 0; i <= n; i++)
    {
        for (w = 0; w <= W; w++)
        {
            if (i==0 || w==0)
                K[i][w] = 0;
            else if (it[i-1].wt <= w)
                K[i][w] = max(it[i-1].vl + K[i-1][w-it[i-1].wt],  K[i-1][w]);
            else
                K[i][w] = K[i-1][w];
        }
    }

    return K[n][W];
}
struct Individual
{
    int fitness;
    vector<int> streambits;
};
float RandomFloat(float a, float b)
{
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}
void createRandomPopulation(int n,vector<Individual>&population,int N)
{
    for(int i=0; i<N; i++)
    {
        Individual IV;
        IV.fitness = -1;
        for(int j=0; j<n; j++)
        {
            int bit = rand()%2;
            IV.streambits.push_back(bit);
        }
        population.push_back(IV);
    }
}
void evaluateFitness(int n,int W,vector<Individual>&population,Item *it,int N)
{
    int mx1=0,mx2=0;
    int *is;
    is = new int[2];
    is[0]=0;
    is[1]=0;
    for(int i=0; i<N; i++)
    {
        int totalVL=0,totalWT=0;
        for(int j=0; j<n; j++)
        {
            if (population[i].streambits[j])
            {
                totalVL += it[j].vl;
                totalWT += it[j].wt;
            }
        }
        if (totalWT<=W)
            population[i].fitness = totalVL;
        else
            population[i].fitness = 0;
    }
    for(int i=0; i<N; i++)
    {
        if (mx1<population[i].fitness)
            mx1=max(mx1,population[i].fitness),is[0]=i;
    }
    for(int i=0; i<N; i++)
    {
        if (mx2<population[i].fitness &&i!=is[0])
        {
            mx2=max(mx2,population[i].fitness),is[1]=i;
        }
    }
    swap(population[N-1],population[is[0]]);
    swap(population[N-2],population[is[1]]);
}
void crossOver(int singlepoint,Individual &iv1,Individual &iv2,int n)
{
    Individual newIV1,newIV2;
    newIV1.fitness = iv1.fitness;
    newIV2.fitness = iv2.fitness;
    for (int i=0; i<n; i++)
    {
        if (i<singlepoint)
        {
            newIV1.streambits.push_back(iv1.streambits[i]);
            newIV2.streambits.push_back(iv2.streambits[i]);
        }
        else
        {
            newIV1.streambits.push_back(iv2.streambits[i]);
            newIV2.streambits.push_back(iv1.streambits[i]);
        }
    }
    iv1 = newIV1;
    iv2 = newIV2;
}
void crossOverThenMutation(vector<Individual>&population,int n,int W,Item *it,int N)
{
    int sp = rand()%n;
    vector<int> indexs;
    for (int i=0; i<N-2; i++)
        indexs.push_back(i);
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle (indexs.begin(), indexs.end(), std::default_random_engine(seed));
    shuffle (indexs.begin(), indexs.end(), std::default_random_engine(seed));
    shuffle (indexs.begin(), indexs.end(), std::default_random_engine(seed));
    shuffle (indexs.begin(), indexs.end(), std::default_random_engine(seed));
    shuffle (indexs.begin(), indexs.end(), std::default_random_engine(seed));
    for (int i=0; i<N-3; i+=2)
    {
        crossOver(sp,population[indexs[i]],population[indexs[i]+1],n);
    }
    float pm = 0.1;
    for (int i=0; i<N-2; i++)
    {
        for (int j=0; i<n; i++)
        {
            float r=RandomFloat(0,1.0);
            if (r<pm)
            {
                if ( population[i].streambits[j] == 0)
                    population[i].streambits[j]=1;
                else
                    population[i].streambits[j]=0;
            }
        }
    }
    evaluateFitness(n,W,population,it,N);
}
void printGeneration(int n,vector<Individual>population)
{
    for(int j=0; j<(int)population.size(); j++)
    {
        cout<<j<<" - ";
        for(int k=0; k<n; k++)
            cout<<population[j].streambits[k]<<" ";
        cout<<"   ----->  "<<population[j].fitness<<endl;
    }
}
int main()
{
    srand(time(NULL));
    fstream f;
    f.open("input_example.txt");
    int c;
    f>>c;
    float eff=0.0;
    for (int i=0; i<c; i++)
    {
        int n,W;
        f>>n>>W;
        Item items[n];
        for (int j=0; j<n; j++)
            f>>items[j].wt>>items[j].vl;
        int N;
        if (n<=5)
            N=n*n*n;
        else if (n<=30)
            N=n*n*2;
        else if (n<=50)
            N=n*n;
        else
            N= n*50;
        if (N%2)
            N++;
        cout<<"======================================================================================="<<endl;
        cout<<"Number of Items = "<<n<<endl<<"Weight of Knapsack = "<<W<<endl<<"Number of Population = "<<N<<endl;
        int ks = knapSack(W,items,n);
        vector<Individual>population;
        createRandomPopulation(n,population,N);
        evaluateFitness(n,W,population,items,N);
        int generations=1;
        while (generations++ && population[N-1].fitness!=ks && generations!=500)
        {
            crossOverThenMutation(population,n,W,items,N);
        }
        cout<<"best solution with normal knapsack = "<<ks<<endl;
        cout<<"best solution found for Case "<<i+1<<" = "<<population[N-1].fitness<<" In Generation No."<<generations<<endl<<"------------"<<endl;
        float x = (float(min(ks,population[N-1].fitness))/float(max(ks,population[N-1].fitness)))*100.0;
        eff += x;
        cout<<"Efficiency for this case = "<<x<<" %"<<endl;
        cout<<"======================================================================================="<<endl;
    }
    cout<<"total Efficiency  = "<<eff/float(c)<<" %"<<endl;
}
