#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "feature.h"
#include "ftree.h"
#include "matrix.h"

using namespace std;


int main(int argc, char *argv[])
{
    if(argc <=1 ){
        cout << "Please specify data directory!\n"; 
        exit(1);
    }
    
    string directory(argv[1]);

    Ftree t(directory);

    // std::vector<std::vector<double> > m { { 1, 1, 1 },
    //                                     { 2, 2, 2 } };
    // Matrix mx(m);
    
    // mx.printSelf();
    FtreeToAttrMatrix fm(t);
    Matrix* mx = fm.toMatrix();
    mx->printSelf();

    // AttributeRowIter iter(t._a[3]);
    // IterReply r =  iter.next();
    // cout<< r.value <<"\n";
    // r =  iter.next();
    // cout<< r.value <<"\n";
    //     r =  iter.next();
    // cout<< r.value <<"\n";
    //     r =  iter.next();
    // cout<< r.value <<"\n";

}
