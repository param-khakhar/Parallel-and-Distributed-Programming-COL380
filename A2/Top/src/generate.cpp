#include <bits/stdc++.h>
using namespace std;

int main(){
    int n = 1000;
    for(int i=0;i<1000;i++){
        for(int j=0;j<1000;j++){
            if(i == j)
            {
                float a = i+1;
                // cout<<a<<setprecision(12)<<" ";
                printf("%0.12f ", a);
            }
            else
            {
                float b = 0.0;
                // cout<<b<<setprecision(12)<<" ";
                printf("%0.12f ",b);
            }
        }
        cout<<"\n";
    }
    return 0;
}