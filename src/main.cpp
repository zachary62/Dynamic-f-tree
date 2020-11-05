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

    // start = clock();
    // FtreeCofactor fc(t._state);
    // fc.Cofactor();
    // duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    // cout << "time for F-tree to compute cofactor: " << duration << "\n"; 

    // start = clock();
    // mx1->cofactor();
    // duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    // cout << "time for Matrix to compute cofactor: " << duration << "\n"; 

 
  
    // double* right_to_mul = new double[t._num_f*10];
    // for(int i = 0; i < t._num_f; i++){
    //     for(int j = 0; j < 10; j++){
    //         right_to_mul[i*10 + j] = 1.01;
    //     } 
    // }
    // Matrix mx2(right_to_mul,t._num_f,10);

    // start = clock();
    // FtreeRightMultiplication frm(t._state);
    // frm.RightMultiply(&mx2);
    // duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    // cout << "time for F-tree to compute right multiplication: " << duration << "\n"; 

    // start = clock();
    // mx1->rightMultiply(&mx2);
    // duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    // cout << "time for Matrix to compute right multiplication: " << duration << "\n"; 



    Count c_first = t._state.cs.at(t._state._attr_order[0]->_id);
    int total = c_first.leftCount * c_first.value;

    // double* left_to_mul = new double[total*10];
    // for(int i = 0; i < total; i++){
    //     for(int j = 0; j < 10; j++){
    //         left_to_mul[i*10 + j] = 1.01;
    //     } 
    // }

    // Matrix mx3(left_to_mul,10,total);

    // start = clock();
    // FtreeLeftMultiplication flm(t._state);
    // flm.LeftMultiply(&mx3);
    // duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    // cout << "time for F-tree to compute left multiplication: " << duration << "\n"; 

    // start = clock();
    // mx3.rightMultiply(mx1);
    // duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    // cout << "time for Matrix to compute left multiplication: " << duration << "\n"; 
    

    FtreeCofactorIterator cofi(t._state);

    start = clock();
    while(true){
        cofi.nextCofactor();
        if(!cofi.hasNext()){
            break;
        }
    }
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time for ftree to compute each cofactor: " << duration << "\n"; 

    start = clock();
    int bgn = 0;
    while(bgn < total*t._num_f){
        double* slice = new double[cofi._gsize*t._num_f];
        for(int i = 0; i < cofi._gsize*t._num_f; i++){
            slice[i] = mx1->_m[bgn + i];
        }
        Matrix* mx4 = new Matrix(slice, cofi._gsize, t._num_f);
        mx4->cofactor();
        bgn += cofi._gsize*t._num_f;
    }
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time for Matrix to compute each cofactor: " << duration << "\n"; 

}

void test_build_ftree(string directory){
    
    clock_t start;
    double duration;


    start = clock();
    Ftree t(directory);
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time to read files: " << duration << "\n";   

    start = clock();
    FtreeState fState = {};
    fState._attr_order = t._a;
    t.initalize(fState);
    FtreeCofactor fc(t._state);
    fc.Cofactor();
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time to build F-tree: " << duration << "\n";    

}

void printState(FtreeState _state){

     cout << "Attribute Order:\n";
    for(Attribute* a: _state._attr_order){
        cout<< a->_id << " ";
    }
    cout << "\n";

    unordered_map<int, Count>::iterator it;
    cout << "Count:\n";
    for (it = _state.cs.begin(); it != _state.cs.end(); it++ )
    {
        cout << it->first  
                << " leftCount: "
                << it->second.leftCount  
                << " value: "
                << it->second.value << "\n";
    }

    unordered_map<int, CountAtt>::iterator it2;


    cout << "CountAtt:\n";
    for (it2 = _state.cas.begin(); it2 != _state.cas.end(); it2++ )
    {
        cout << it2->first  
                << " leftCount: "
                << it2->second.leftCount;

        if( it2->second.allOne){
            cout << " allOne";
        }else{
            cout << " value: ";
            for(int i: *(it2->second.prefix_sum)){
                cout << i << " ";
            }

        }
        cout<< "\n";

    }

    cout << "CountCof:\n";
    for(auto const &ent1 : _state.ccofs) {
        auto const &outer_key = ent1.first;
        auto const &inner_map = ent1.second;
        for(auto const &ent2 : inner_map) {
            auto const &inner_key   = ent2.first;
            auto const &inner_value = ent2.second;

            cout<< outer_key << " " << inner_key 
                // << " leftCount " << inner_value.leftCount
                << " middleCount " << inner_value.middleCount;
            if(inner_value.cartesian){
                cout<<" is cartesian";
            } else{
                cout<<" is not cartesian";
            }
            cout<< "\n";
        }
    }
 }

// used on small data
void test_operation_verbose(string directory){

    Ftree t(directory);
    FtreeState fState = {};
    fState._attr_order = t._a;
    t.initalize(fState);
    cout << "Feature Matrix from Ftree\n";
    FtreeToFeatureMatrix fm(fState);
    Matrix* mx1 = fm.toMatrix();
    mx1->printSelf();
    cout << "__________________\n";
    cout << "Cofactor from Ftree\n";
    FtreeCofactor fc(t._state);
    fc.Cofactor()->printSelf();
    cout << "__________________\n";
    cout << "Cofactor from Matrix\n";
    mx1->cofactor()->printSelf();
    cout << "__________________\n";
    double* right_to_mul = new double[t._num_f];
    for(int i = 0; i < t._num_f; i++){
        right_to_mul[i] = 1.01;
    }
    Matrix mx2(right_to_mul,t._num_f,1);

    cout << "Right mul from Ftree\n";
    FtreeRightMultiplication frm(t._state);
    frm.RightMultiply(&mx2)->printSelf();

    cout << "__________________\n";
    cout << "Right mul from Matrix\n";
    mx1->rightMultiply(&mx2)->printSelf();

    Count c_first = t._state.cs.at(t._state._attr_order[0]->_id);
    int total = c_first.leftCount * c_first.value;

    double* left_to_mul = new double[total];
    for(int i = 0; i < total; i++){
        left_to_mul[i] = 1.01;
    }
    Matrix mx3(left_to_mul,1,total);

    cout << "__________________\n";
    cout << "Left mul from Ftree\n";

    FtreeLeftMultiplication flm(t._state);
    flm.LeftMultiply(&mx3)->printSelf();
    cout << "__________________\n";
    cout << "Left mul from Matrix\n";
    mx3.rightMultiply(mx1)->printSelf();



    cout << "__________________\n";
    cout << "Cofactor each\n";
    FtreeCofactorIterator cofi(t._state);

    while(true){
        Matrix* mx = cofi.nextCofactor();
        if(!cofi.hasNext()){
            break;
        }
        mx->printSelf();
        cout << "__________________\n";

        // vector<vector<double>> slice(mx1->_m.begin() + start, mx1->_m.begin() + start + cofi._gsize);
        // Matrix* mx4 = new Matrix(slice);
        // mx4->cofactor()->printSelf();
        // cout << "__________________\n";
        // cout << "__________________\n";
        // start += cofi._gsize;
    }
    


}

// only run this test on dataset in ./data/dimhie/
void test_drill_down(string directory, int atts){
    
    clock_t start;
    double duration;

    start = clock();
    Ftree t(directory);
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time to read files: " << duration << "\n";   

    start = clock();
    FtreeState fState = {};
    vector<Attribute*> att_vec;
    for(int i = 0; i < t._num_d; i++){
        for(int k = 0 ; k < atts; k++){
            att_vec.push_back(t._a[7*i + k]);
        }
        
    }
    fState._attr_order = att_vec;
   
    t.initalize(fState); 
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time to intialize: " << duration << "\n"; 

    start = clock();
    for(int i = 1; i < t._num_d; i++){
        t.attemptDrillDown(i+1, true);
    }
    t.setState(t.attemptDrillDown(1, true));
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time for first drill down with cache: " << duration << "\n"; 

    start = clock();
    for(int i = 0; i < t._num_d; i++){
        t.attemptDrillDown(i+1, true);
    }
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time for second drill down with cache: " << duration << "\n"; 

    t.initalize(fState); 

    start = clock();
    for(int i = 1; i < t._num_d; i++){
        t.attemptDrillDown(i+1, false);
    }
    t.setState(t.attemptDrillDown(1, false));
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time for first drill down without cache: " << duration << "\n"; 

    start = clock();
    for(int i = 0; i < t._num_d; i++){
        t.attemptDrillDown(i+1, false);
    }
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time for second drill down without cache: " << duration << "\n"; 


    start = clock();
    for(int i = 0; i < t._num_d; i++){
        FtreeState fState2 = {};
        vector<Attribute*> att_v;
        for(int j = 0; j < t._num_d; j++){
            for(int k = 0; k < atts; k++){
                att_v.push_back(t._a[7*j + k]);
            }

            if(i == j){
                att_v.push_back(t._a[7*j + atts]);
            }
        }   
        fState2._attr_order = att_v;
        t.initalize(fState2); 
    }
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time for first drill down without propogate: " << duration << "\n"; 

    start = clock();
    for(int i = 1; i < t._num_d; i++){
        FtreeState fState2 = {};
        vector<Attribute*> att_v;
        for(int k = 0; k < atts + 1; k++){
            att_v.push_back(t._a[k]);
        }

        for(int j = 1; j < t._num_d; j++){
            for(int k = 0; k < atts; k++){
                att_v.push_back(t._a[7*j + k]);
            }
            if(i == j){
                att_v.push_back(t._a[7*j + atts]);
            }
        }   
        
        fState2._attr_order = att_v;
        t.initalize(fState2); 
    }

    FtreeState fState3 = {};
    vector<Attribute*> att_v2;
    for(int k = 0; k < atts + 2; k++){
        att_v2.push_back(t._a[k]);
    }

    for(int i = 1; i < t._num_d; i++){
        for(int k = 0; k < atts; k++){
            att_v2.push_back(t._a[7*i + k]);
        }
    }   

    fState3._attr_order = att_v2;
    t.initalize(fState3); 

    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time for second drill down without propogate: " << duration << "\n"; 

}

int main(int argc, char *argv[])
{
    if(argc <=1 ){
        cout << "Please specify data directory!\n"; 
        exit(1);
    }
    
    string directory(argv[1]);

    // test_build_ftree(directory);
    test_matrix_op(directory);
    // test_operation_verbose(directory);
    // test_drill_down(directory, 5);




    // Ftree t(directory);



    // FtreeState fState = {};
    // fState._attr_order = t._a;
    // t.initalize(fState);

    // double right[] =  {1,2,3,4,5,6,7,8,9};
    // Matrix* mx = new Matrix(right,9,1);
    // FtreeRightMultiplicationIterator lmi(t._state);

    // while(true){
    //     Matrix* rs = lmi.RightMultiplyNext(mx);
    //     if(!lmi.hasNext()){
    //         break;
    //     }
    //     for(int i = 0; i < 2; i++){
    //         cout<< rs->_m[i] << " ";
    //     }
    //     cout <<"\n";
    //     cout << "______________\n";
    // }
    

    // FtreeToFeatureMatrix fm(fState);
    // Matrix* mx1 = fm.toMatrix();
    // mx1->printSelf();
    // cout << "______________\n";
    // vector<vector<double>> right2 = {{1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2}};
    // Matrix(right2).rightMultiply(mx1)->printSelf();



}


