#include "ass.h"
#include <unistd.h>
Ass::Ass():status(true)
{
    for(int i = 0; i < kMaterialsTypeCount; i++){
        current_m_count[kMaterialsType[i]] = 0; 
        total_m_count[kMaterialsType[i]] = 0;
    }

    for(int i = 0; i < kToolTypeCount; i++){
        tools[kToolType[i]] = false;
    }

    for(int i = 0; i < kProductTypeCount; i++){
        product_in_making[kProductType[i]] = 0;
        product_in_buff[kProductType[i]] = 0;
    }
    pthread_mutex_init (&materials_mtx, NULL);
    pthread_mutex_init (&tools_mtx, NULL);
    pthread_mutex_init (&products_mtx, NULL);
    pthread_mutex_init (&status_mtx, NULL);
    pthread_cond_init(&materials_cond, NULL);
    pthread_cond_init(&materials_create_cond, NULL);
    pthread_cond_init(&tools_cond, NULL);
    pthread_cond_init(&products_cond, NULL);
    pthread_cond_init(&status_cond, NULL);
}

Ass::~Ass()
{
    for(int i = 0; i < generators.size();i++){
        delete generators[i];
    }
    generators.clear();

    for(int i = 0; i < operators.size();i++){
        delete operators[i];
    }
    operators.clear();

}

void Ass::create_generators()
{
    for(int i = 0; i < kMaterialsTypeCount; i++){
        Generator* g = new Generator(kMaterialsType[i]);
        g->start(this);
        generators.push_back(g);
    }
}

void Ass::create_operators(int operators_count)
{
    for(int i = 0; i < operators_count; i++){
        Operator* op = new Operator();
        op->start(this);
        operators.push_back(op);
    }
}

bool Ass::check_material_full(int type)
{
    bool ret = (materials_buff.size() >= kMaxMaterialsCount || current_m_count[type] >= kLimitMaterialCount);
    return ret;
}

bool Ass::check_material_empty(int type)
{
    return current_m_count[type] == 0;
}

bool Ass::get_material(int type)
{
    for(vector<int>::iterator it = materials_buff.begin(); it != materials_buff.end(); it++){
        if(*it == type){
            materials_buff.erase(it);
            current_m_count[type] -= 1;
            return true;
        }
    }
    return false;
}

bool Ass::generate_material(int type)
{
    materials_buff.push_back(type);
    current_m_count[type] += 1;
    total_m_count[type] += 1;
    return true;
}

char Ass::get_producing_type()
{
    char ptype = 'D';
    int count = product_buff.size();
    for(int i = 0; i < kProductTypeCount; i++){
        if(product_in_making[kProductType[i]] > 0 || (count && product_buff[count-1] == kProductType[i])){
            continue;
        }else{
            if(count >= product_in_buff[kProductType[i]]){
                count = product_in_buff[kProductType[i]];
                ptype = kProductType[i];
            }
        }
    }

    if(ptype != 'D'){
        for(int i = 0; i < kProductTypeCount; i++){
            if(product_in_buff[ptype] - product_in_buff[kProductType[i]] >= 10){
                ptype = 'D';
                break;
            }
        }
    }
    return ptype;
}

bool Ass::producing_product(char ptype)
{
    //cout << "producing " << ptype << endl;
    switch(ptype){
        case 'A':
            producing_product_A();
            break;
        case 'B':
            producing_product_B();
            break;
        case 'C':
            producing_product_C();
            break;
        default:
            break;
    }
    return true;
}


bool Ass::check_tool_need(char ptype)
{    
    bool ret = false;
    switch(ptype){
        case 'A':
            if(!tools['x'] && !tools['y'])
                ret = true;
            break;
        case 'B':
            if(!tools['y'] && !tools['z'])
                ret = true;
            break;
        case 'C':
            if(!tools['z'] && !tools['x'])
                ret = true;
            break;
        default:
            break;
    }
    return ret;
}

bool Ass::check_material_need(char ptype)
{    
    bool ret = false;
    switch(ptype){
        case 'A':
            if(current_m_count[1] && current_m_count[2])
                ret = true;
            break;
        case 'B':
            if(current_m_count[2] && current_m_count[3])
                ret = true;
            break;
        case 'C':
            if(current_m_count[3] && current_m_count[1])
                ret = true;
            break;
        default:
            break;
    }
    return ret;
}

bool Ass::producing_product_A()
{
    char ptype = 'A';
    pthread_mutex_lock(&tools_mtx);
    while(!check_tool_need(ptype)){
        pthread_cond_wait(&tools_cond, &tools_mtx);
    }
    tools['x'] = true;
    tools['y'] = true;
    pthread_mutex_unlock(&tools_mtx);
    set_pause_resume_switch();

    pthread_mutex_lock(&materials_mtx);
    while(!check_material_need(ptype)){
        pthread_cond_wait(&materials_create_cond, &materials_mtx);
    }
    get_material(1);
    get_material(2);
    pthread_cond_broadcast(&materials_cond);
    pthread_mutex_unlock(&materials_mtx);

    usleep(operator_sleep_time);

    pthread_mutex_lock(&products_mtx);

    product_buff.push_back(ptype);
    product_in_buff[ptype] += 1;
    product_in_making[ptype] -= 1;

    pthread_cond_broadcast(&products_cond);
    pthread_mutex_unlock(&products_mtx);
    show_info();


    pthread_mutex_lock(&tools_mtx);
    tools['x'] = false;
    tools['y'] = false;
    pthread_cond_broadcast(&tools_cond);
    pthread_mutex_unlock(&tools_mtx);

    return true;
}

bool Ass::producing_product_B()
{
    char ptype = 'B';
    pthread_mutex_lock(&tools_mtx);
    while(!check_tool_need(ptype)){
        pthread_cond_wait(&tools_cond, &tools_mtx);
    }
    tools['y'] = true;
    tools['z'] = true;
    pthread_mutex_unlock(&tools_mtx);
    set_pause_resume_switch();

    pthread_mutex_lock(&materials_mtx);
    while(!check_material_need(ptype)){
        pthread_cond_wait(&materials_create_cond, &materials_mtx);
    }
    get_material(2);
    get_material(3);
    pthread_cond_broadcast(&materials_cond);
    pthread_mutex_unlock(&materials_mtx);

    usleep(operator_sleep_time);

    pthread_mutex_lock(&products_mtx);

    product_buff.push_back(ptype);
    product_in_buff[ptype] += 1;
    product_in_making[ptype] -= 1;

    pthread_cond_broadcast(&products_cond);
    pthread_mutex_unlock(&products_mtx);
    show_info();

    pthread_mutex_lock(&tools_mtx);
    tools['y'] = false;
    tools['z'] = false;
    pthread_cond_broadcast(&tools_cond);
    pthread_mutex_unlock(&tools_mtx);
    
    return true;
}

bool Ass::producing_product_C()
{
    char ptype = 'C';
    pthread_mutex_lock(&tools_mtx);
    while(!check_tool_need(ptype)){
        pthread_cond_wait(&tools_cond, &tools_mtx);
    }
    tools['z'] = true;
    tools['x'] = true;
    pthread_mutex_unlock(&tools_mtx);
    set_pause_resume_switch();

    pthread_mutex_lock(&materials_mtx);
    while(!check_material_need(ptype)){
        pthread_cond_wait(&materials_create_cond, &materials_mtx);
    }
    get_material(3);
    get_material(1);
    pthread_cond_broadcast(&materials_cond);
    pthread_mutex_unlock(&materials_mtx);

    usleep(operator_sleep_time);

    pthread_mutex_lock(&products_mtx);

    product_buff.push_back(ptype);
    product_in_buff[ptype] += 1;
    product_in_making[ptype] -= 1;

    pthread_cond_broadcast(&products_cond);
    pthread_mutex_unlock(&products_mtx);
    show_info();

    pthread_mutex_lock(&tools_mtx);
    tools['z'] = false;
    tools['x'] = false;
    pthread_cond_broadcast(&tools_cond);
    pthread_mutex_unlock(&tools_mtx);
    
    return true;
}

void Ass::thread_pause()
{
    if(status){
        pthread_mutex_lock(&status_mtx);
        status = false;
        pthread_mutex_unlock(&status_mtx);
        cout << "application pause success!" << endl;

    }else{
        cout << "application pause already!" << endl;
    }
}

void Ass::thread_resume()
{
    if(!status){
        pthread_mutex_lock(&status_mtx);
        status = true;
        pthread_cond_broadcast(&status_cond);
        pthread_mutex_unlock(&status_mtx);
        cout << "application run success!" << endl;
    }else{

        cout << "application run already!" << endl;
    }
}

void Ass::show_info()
{
    cout << "**********application infomation start**********" << endl;
    cout << "materials generated info : ";
    for(int i = 0; i < kMaterialsTypeCount; i++){
        cout << "Material  " << kMaterialsType[i] << " : " << total_m_count[kMaterialsType[i]];
        if(i < kMaterialsTypeCount - 1)
            cout << " , ";
    }
    cout << endl;

    cout << "input buffer info : ";
    for(int i = 0; i < materials_buff.size(); i++){
        cout << materials_buff[i];
        if(i < materials_buff.size() - 1)
            cout << " , ";
    }
    cout << endl;

    cout << "products produced info : ";
    for(int i = 0; i < kProductTypeCount; i++){
        cout << "Material  " << kProductType[i] << " : " << product_in_buff[kProductType[i]];
        if(i < kProductTypeCount - 1)
            cout << " , ";
    }
    cout << endl;

    cout << "output buffer info : ";
    for(int i = 0; i < product_buff.size(); i++){
        cout << product_buff[i];
        if(i < product_buff.size() - 1)
            cout << " , ";
    }
    cout << endl;


    cout << "**********application infomation end**********" << endl;
    cout << endl;
}

void Ass::set_pause_resume_switch()
{
        pthread_mutex_lock(&status_mtx);
        while(!status){
            pthread_cond_wait(&status_cond, &status_mtx);
        }
        pthread_mutex_unlock(&status_mtx);

}

Generator::Generator(int m_type):tid(0),material_type(m_type)
{
}

bool Generator::start(Ass *pAss)
{
    int ret = 0;
    switch(material_type){
        case 1:
            ret = pthread_create(&tid, NULL, generate_first_material, (void*)pAss);
            break;
        case 2:
            ret = pthread_create(&tid, NULL, generate_second_material, (void*)pAss);
            break;
        case 3:
            ret = pthread_create(&tid, NULL, generate_third_material, (void*)pAss);
            break;
        default:
            break;
    }

    return ret == 0;
}

void* Generator::generate_first_material(void* param)
{
    Ass* pAss = static_cast<Ass*>(param);
    cout << "first generator created" << endl;
    while(true){
        pAss->set_pause_resume_switch();

        pthread_mutex_lock(&pAss->materials_mtx);
        while(pAss->check_material_full(FIRST_M)){
            pthread_cond_wait(&pAss->materials_cond, &pAss->materials_mtx);         
        }
        pAss->generate_material(FIRST_M);
        pthread_cond_broadcast(&pAss->materials_create_cond);

        pthread_mutex_unlock(&pAss->materials_mtx);
        pAss->show_info();
        usleep(generator_sleep_time);

    }
    return NULL;
}


void* Generator::generate_second_material(void* param)
{
    Ass* pAss = static_cast<Ass*>(param);
    cout << "second generator created" << endl;
    while(true){
        pAss->set_pause_resume_switch();

        pthread_mutex_lock(&pAss->materials_mtx);
        while(pAss->check_material_full(SECOND_M)){
            pthread_cond_wait(&pAss->materials_cond, &pAss->materials_mtx);         
        }
        pAss->generate_material(SECOND_M);
        pthread_cond_broadcast(&pAss->materials_create_cond);

        pthread_mutex_unlock(&pAss->materials_mtx);
        pAss->show_info();
        usleep(generator_sleep_time);

    }
    return NULL;
}

void* Generator::generate_third_material(void* param)
{
    Ass* pAss = static_cast<Ass*>(param);
    cout << "third generator created" << endl;
    while(true){
        pAss->set_pause_resume_switch();

        pthread_mutex_lock(&pAss->materials_mtx);
        while(pAss->check_material_full(THIRD_M)){
            pthread_cond_wait(&pAss->materials_cond, &pAss->materials_mtx);         
        }
        pAss->generate_material(THIRD_M);
        pthread_cond_broadcast(&pAss->materials_create_cond);
        pthread_mutex_unlock(&pAss->materials_mtx);
        pAss->show_info();
        usleep(generator_sleep_time);

    }
    return NULL;

}

Operator::Operator():tid(0)
{
}

bool Operator::start(Ass* pAss)
{
   int ret = pthread_create(&tid, NULL, producing_product, (void*)pAss); 
   cout << "operator created, tid = " << tid << endl;
   return ret == 0;
}

void* Operator::producing_product(void* param)
{
    Ass* pAss = static_cast<Ass*>(param);
    while(true){
        pAss->set_pause_resume_switch();

        pthread_mutex_lock(&pAss->products_mtx);
        char ptype = 'D';
        while((ptype = pAss->get_producing_type()) == 'D'){
            sleep(1);
            pthread_cond_wait(&pAss->products_cond, &pAss->products_mtx);         
        }
        pAss->product_in_making[ptype] += 1;
        pthread_mutex_unlock(&pAss->products_mtx);
        pAss->producing_product(ptype);

    }
    return NULL; 
}
