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
        row.push_back(1.01);
        row.push_back(1.01);
        row.push_back(1.01);
        row.push_back(1.01);
        row.push_back(1.01);
        row.push_back(1.01);
        row.push_back(1.01);
        row.push_back(1.01);
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
    for(int j = 0; j < 10; j++){
        vector<double> row;
        for(int i = 0; i < total; i++){
            row.push_back(1.01);
        }
        left_to_mul.push_back(row);
    }
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
    vector<vector<double> > right_to_mul;
    for(int i = 0; i < t._num_f; i++){
        vector<double> row;
        row.push_back(1.01);
        right_to_mul.push_back(row);
    }
    Matrix mx2(right_to_mul);

    cout << "Right mul from Ftree\n";
    FtreeRightMultiplication frm(t._state);
    frm.RightMultiply(&mx2)->printSelf();

    cout << "__________________\n";
    cout << "Right mul from Matrix\n";
    mx1->rightMultiply(&mx2)->printSelf();

    Count c_first = t._state.cs.at(t._state._attr_order[0]->_id);
    int total = c_first.leftCount * c_first.value;

    vector<vector<double> > left_to_mul;
    vector<double> row;
    for(int i = 0; i < total; i++){
        row.push_back(1.01);
    }
    left_to_mul.push_back(row);
    Matrix mx3(left_to_mul);

    cout << "__________________\n";
    cout << "Left mul from Ftree\n";

    FtreeLeftMultiplication flm(t._state);
    flm.LeftMultiply(&mx3)->printSelf();
    cout << "__________________\n";
    cout << "Left mul from Matrix\n";
    mx3.rightMultiply(mx1)->printSelf();



}

// only run this test on dataset in ./data/dimhie/
void test_drill_down(string directory){
    
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
        att_vec.push_back(t._a[4*i]);
        att_vec.push_back(t._a[4*i + 1]);
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
            att_v.push_back(t._a[4*j]);
            att_v.push_back(t._a[4*j + 1]);
            if(i == j){
                att_v.push_back(t._a[4*j + 2]);
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
        att_v.push_back(t._a[0]);
        att_v.push_back(t._a[1]);
        att_v.push_back(t._a[2]);
        for(int j = 1; j < t._num_d; j++){
            att_v.push_back(t._a[4*j]);
            att_v.push_back(t._a[4*j + 1]);
            if(i == j){
                att_v.push_back(t._a[4*j + 2]);
            }
        }   
        
        fState2._attr_order = att_v;
        t.initalize(fState2); 
    }

    FtreeState fState3 = {};
    vector<Attribute*> att_v2;

    att_v2.push_back(t._a[0]);
    att_v2.push_back(t._a[1]);
    att_v2.push_back(t._a[2]);
    att_v2.push_back(t._a[3]);

    for(int i = 1; i < t._num_d; i++){
        att_v2.push_back(t._a[4*i]);
        att_v2.push_back(t._a[4*i + 1]);
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
    // test_matrix_op(directory);
    // test_operation_verbose(directory);
    test_drill_down(directory);
    // Ftree t(directory);
    // vector<Attribute*> att_v;

    // FtreeState fState = {};
    // fState._attr_order = att_v;
    // // fState._attr_order = t._a;




    // t.initalize(fState); 
    // printState(t.attemptDrillDown(1));
    // t.setState(t.attemptDrillDown(1));
    // cout << "__________________\n";
    // // t.setState(t.attemptDrillDown(1));
    // printState(t.attemptDrillDown(1));


    // FtreeState fState2 = {};
    // vector<Attribute*> att_v2;
    // att_v2.push_back(t._a[0]);
    // att_v2.push_back(t._a[1]);
    // att_v2.push_back(t._a[2]);

    // fState2._attr_order = att_v2;
    // Ftree t2(directory);
    // t2.initalize(fState2);
    // cout << "__________________\n";
    // printState(t2._state);


}


