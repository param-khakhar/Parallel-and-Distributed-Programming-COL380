#include <bits/stdc++.h>
using namespace std;

int main(){
    int n = 50;
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            // if(i == j)
            // {
            //     float a = i+1;
            //     // cout<<a<<setprecision(12)<<" ";
            //     printf("%0.12f ", a);
            // }
            // else
            // {
            //     float b = 0.0;
            //     // cout<<b<<setprecision(12)<<" ";
            //     printf("%0.12f ",b);
            // }
            printf("%0.12f ",float(rand())/float((RAND_MAX)) * 20.0);
        }
        cout<<"\n";
    }
    return 0;
}