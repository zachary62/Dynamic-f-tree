#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include "feature.h"
#include "ftree.h"
#include "matrix.h"
#include "state.h"

using namespace std;


int main(int argc, char *argv[])
{
    if(argc <=1 ){
        cout << "Please specify data directory!\n"; 
        exit(1);
    }
    
    string directory(argv[1]);

    Ftree t(directory);


    

    FtreeState fState = {};
    fState._attr_order.push_back(t._a[0]);
    fState._attr_order.push_back(t._a[1]);
    fState._attr_order.push_back(t._a[2]);
    fState._attr_order.push_back(t._a[3]);
    
    // t.initalize(fState);
    
    // t.printState();


    FtreeToFeatureMatrix fm(fState);
    Matrix* mx1 = fm.toMatrix();
    mx1->printSelf();
    cout << "__________________\n";
    // std::vector<std::vector<double> > m2 { { 1, 4, 5, 6 },
    //                                        { 2, 2,12, 3 },
    //                                        { 3, 6, 3, 3 },
    //                                        { 4, 4, 3, 4 },
    //                                        { 1, 2, 1, 1 },
    //                                        { 2, 4, 4, 2 },
    //                                        { 3, 7, 3, 5 },
    //                                        { 4, 0, 4, 4 },
    //                                        { 1, 2, 3, 1 }};
    // Matrix mx2(m2);

    Matrix* mx3 = mx1->cofactor();
    mx3->printSelf();

    // Matrix* mx3 = mx1->rightMultiply(&mx2);
    // mx3->printSelf();

    // cout<< "--------------\n";
    // FtreeRightMultiplication frm(fState);
    // Matrix* mx4 = frm.RightMultiply(&mx2);
    // mx4->printSelf();   
    


    // clock_t start;
    // double duration;

    // start = clock();

    // for(int i = 0; i< 100;i++){
    //     FtreeToAttrMatrix fm(t);
    //     fm.toMatrix();
    // }

    // duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    // cout << "time pasted: " << duration << "\n";

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

