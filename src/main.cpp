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


void test_matrix_op(string directory){
    
    clock_t start;
    double duration;

    start = clock();
    Ftree t(directory);
    FtreeState fState = {};
    fState._attr_order = t._a;
    t.initalize(fState);
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time to build F-tree: " << duration << "\n";    
    
    start = clock();
    FtreeToFeatureMatrix fm(fState);
    Matrix* mx1 = fm.toMatrix();
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time to build Matrix: " << duration << "\n";  

    start = clock();
    FtreeCofactor fc(t._state);
    fc.Cofactor();
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time for F-tree to compute cofactor: " << duration << "\n"; 

    start = clock();
    mx1->cofactor();
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time for Matrix to compute cofactor: " << duration << "\n"; 

 
    vector<vector<double> > right_to_mul;
    
    for(int i = 0; i < t._num_f; i++){
        vector<double> row;
        row.push_back(1.01);
        right_to_mul.push_back(row);
    }

    Matrix mx2(right_to_mul);

    start = clock();
    FtreeRightMultiplication frm(t._state);
    frm.RightMultiply(&mx2);
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time for F-tree to compute right multiplication: " << duration << "\n"; 

    start = clock();
    mx1->rightMultiply(&mx2);
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time for Matrix to compute right multiplication: " << duration << "\n"; 



    Count c_first = t._state.cs.at(t._state._attr_order[0]->_id);
    int total = c_first.leftCount * c_first.value;

    vector<vector<double> > left_to_mul;
    vector<double> row;
    for(int i = 0; i < total; i++){
        row.push_back(1.01);
    }
    left_to_mul.push_back(row);

    Matrix mx3(left_to_mul);

    start = clock();
    FtreeLeftMultiplication flm(t._state);
    flm.LeftMultiply(&mx3);
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time for F-tree to compute left multiplication: " << duration << "\n"; 

    start = clock();
    mx3.rightMultiply(mx1);
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time for Matrix to compute left multiplication: " << duration << "\n"; 


}

int main(int argc, char *argv[])
{
    if(argc <=1 ){
        cout << "Please specify data directory!\n"; 
        exit(1);
    }
    
    string directory(argv[1]);

    test_matrix_op(directory);


    // t.initalize(fState);
    


    // FtreeToFeatureMatrix fm(fState);
    // Matrix* mx1 = fm.toMatrix();
    // mx1->printSelf();
    // cout << "__________________\n";
    // std::vector<std::vector<double> > m2 { { 1, 4, 5, 6, 1, 3, 4, 5, 7, 2, 3, 7, 6, 1, 3, 4},
    //                                        { 2, 2,12, 3, 2, 2,12, 3, 6, 1, 3, 4, 2, 2,12, 3}};
    // Matrix mx2(m2);

    // Matrix* mx3 = mx2.rightMultiply(mx1);
    // mx3->printSelf();

    // cout<< "--------------\n";
    // FtreeLeftMultiplication flm(t._state);
    // Matrix* mx4 = flm.LeftMultiply(&mx2);
    // mx4->printSelf();   

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


