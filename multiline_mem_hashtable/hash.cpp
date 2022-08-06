#include "hash.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <cstdio>
using namespace std;

int main(int argc, char **argv)
{
    HashTable<int> HT(10000, 6);
    HT.setList(5);

    int i = 0, j;
    double sum = 0.0, sum2 = 0.0;
    volatile unsigned long temp;
    volatile unsigned long seed;
    while (i++ < 1000)
    {
        seed = time(NULL);
        srand(i + seed);
        while (true)
        {
            temp = rand();
            if (HT.insert(temp, 0) < 0)
                break;
        }
        cout << HT.getFullRate() << "\t" << HT.getOverflowRate() << endl;
        sum += HT.getFullRate();
        sum2 += HT.getOverflowRate();
        HT.clear();
    }
    cout << "\n\n\n"
        << sum / 1000 << "\t  " << sum2 / 1000 << endl;

    return 0;
}
