//
//  main.cpp
//  KeyBinding
//
//  Created by Eryun Liu on 14/12/29.
//  Copyright (c) 2014年 Eryun Liu. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <NTL/GF2EX.h>
#include <NTL/GF2XFactoring.h>
#include <NTL/GF2EXFactoring.h>
using namespace std;
NTL_CLIENT


bool load_vec_GF2E(const char* file, vec_GF2E& x, vector<int>& index)
{
    fstream fs(file);
    if (!fs.is_open()) {
        return false;
    }
    vector<GF2E> tmp;
    while (1) {
        int k;
        fs>>k;
        GF2E a;
        fs>>a;
        if (a==0) break;
        index.push_back(k);
        tmp.push_back(a);
    }
    
    x.SetLength(tmp.size());
    for (int k=0; k<x.length(); k++) {
        x[k] = tmp[k];
    }
    fs.close();
    return true;
}

bool save_vec_GF2E(const char* file, const vec_GF2E& x, const vector<int>& index)
{
    fstream fs(file, ios_base::out);
    if(!fs.is_open()) return false;
    for (int k=0; k<index.size(); k++) {
        fs<<index[k]<<" ";
        fs<<x[k]<<endl;
    }
    fs.close();
    return true;
}

bool enroll(const char* file_X, const char* file_Y, const char* file_poly, int poly_degree, int L)
{
    GF2X minipoly = BuildIrred_GF2X(L);
    GF2E::init(minipoly);

    vec_GF2E X;
    vector<int> index;
    bool rt = load_vec_GF2E(file_X, X, index);
    if(!rt){
        printf("Fail to load %s\n", file_X);
        return false;
    }
    
    GF2EX poly = random_GF2EX(poly_degree);
    vec_GF2E Y;
    Y.SetLength(X.length());
    for (int k=0; k<X.length(); k++) {
        Y[k] = eval(poly, X[k]);
    }
    
    // save message
    rt = save_vec_GF2E(file_Y, Y, index);
    if(!rt){
        printf("Fail to save %s\n", file_Y);
        return false;
    }

    fstream fs(file_poly, ios_base::out);
    if(!fs)
    {
        printf ("Fail to save %s\n", file_poly);
        return false;
    }
    fs<<poly;
    fs.close();
    
    return true;
}

bool reconstruct(const char* file_X, const char* file_Y, const char* file_poly, int poly_degree, int L)
{
    GF2X minipoly = BuildIrred_GF2X(L);
    GF2E::init(minipoly);
    
    vec_GF2E X;
    vector<int> indexX;
    bool rt = load_vec_GF2E(file_X, X, indexX);
    if(!rt){
        printf("Fail to load %s\n", file_X);
        return false;
    }
    
    vec_GF2E Y;
    vector<int> indexY;
    rt = load_vec_GF2E(file_Y, Y, indexY);
    if(!rt){
        printf("Fail to load %s\n", file_Y);
        return false;
    };
    
    if (X.length()<poly_degree){
        cout << "No enough recovered messages"<<endl;
        return false;
    };
    
    
    GF2EX poly;
    vec_GF2E Y2;
    Y2.SetLength(X.length());
    for (int k=0; k<X.length(); k++) {
        Y2[k] = Y[indexX[k]];
    }
    interpolate(poly, X, Y2);
    
    fstream fs(file_poly, ios_base::out);
    if(!fs)
    {
        printf ("Fail to save %s\n", file_poly);
        return false;
    }
    fs<<poly;
    fs.close();

    return true;
}


// usage
void usage()
{
    cout << "\tKeyBind enroll file_x_coordinate file_y_coordinates file_poly poly_degree L"<<endl;
    cout << "or"<<endl;
    cout << "\tKeyBind recon file_x_coordinate file_y_coordinates file_poly poly_degree L"<<endl;
}

int main(int argc, const char * argv[])
{
    if(argc!=7){
        usage();
        return 1;
    }
    
    int poly_degree,L;
    poly_degree = atoi(argv[5]);
    L = atoi(argv[6]);
    
    bool rt;
    if (strcmp(argv[1], "enroll")==0) {
        rt = enroll(argv[2], argv[3], argv[4], poly_degree, L);
        if (!rt) return 1;
    }else if (strcmp(argv[1], "recon")==0)
    {
        rt = reconstruct(argv[2], argv[3], argv[4], poly_degree, L);
        if (!rt) return 1;
    }else{
        cout << "Unknown argument"<<endl;
        return 1;
    }
    
    return 0;
}
