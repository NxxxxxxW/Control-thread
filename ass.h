
#ifndef ASS_H
#define ASS_H
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <pthread.h>
using namespace std;
const int kMaxMaterialsCount = 10;
const int kLimitMaterialCount = 4;
const char kToolType[] = {'x', 'y', 'z'};
const int kMaterialsType[] = {1, 2, 3};
const char kProductType[] = {'A', 'B', 'C'};
const int kToolTypeCount = 3;
const int kMaterialsTypeCount = 3;
const int kProductTypeCount = 3;
const int kGeneratosCount = 3;
const int kDefaultOperatorCount = 3;
const int generator_sleep_time = 1000 * 500;
const int operator_sleep_time = 1000 * 1000;
enum MATERIAL_TYPE{
    FIRST_M = 1,
    SECOND_M,
    THIRD_M
};
class Generator;
class Operator;
class Ass{
public:
    vector<int> materials_buff;
    map<int, int> current_m_count;
    map<int, int> total_m_count;

    map<char,bool> tools;

    vector<char> product_buff;
    map<char, int> product_in_making;
    map<char, int> product_in_buff;

    bool status;

    vector<Generator*> generators;
    vector<Operator*> operators;

public:
    pthread_mutex_t materials_mtx;
    pthread_cond_t materials_cond;
    pthread_cond_t materials_create_cond;

    pthread_mutex_t tools_mtx;
    pthread_cond_t tools_cond;
    pthread_mutex_t products_mtx;
    pthread_cond_t products_cond;

    pthread_mutex_t status_mtx;
    pthread_cond_t status_cond;

public:
    Ass();
    ~Ass();
    void create_generators();
    void create_operators(int operators_count = 3);
    bool check_material_full(int type);
    bool check_material_empty(int type);
    bool get_material(int type);
    bool generate_material(int type);
    char get_producing_type();
    bool producing_product(char ptype);
    bool producing_product_A();
    bool producing_product_B();
    bool producing_product_C();
    bool check_tool_need(char ptype);
    bool check_material_need(char ptype);
    void thread_pause();
    void thread_resume();
    void show_info();
    void set_pause_resume_switch();
};

class Generator{
private:
    pthread_t tid;
    int material_type;
public:
    Generator(int m_type);
    bool start(Ass *pAss);
    static void* generate_first_material(void* param);
    static void* generate_second_material(void* param);
    static void* generate_third_material(void* param);
};

class Operator{
private: 
    pthread_t tid;
public:
    Operator();
    bool start(Ass *pAss);
    static void* producing_product(void* param);
};
#endif
