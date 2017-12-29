#include "topic_model.hpp"
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <unordered_map>
#include <utility>
#include <iostream>

class lda_doc: public doc {
public:
    lda_doc();
    lda_doc(int n,int y);
    bool update(int c);
};

class lda_Corpus: public Corpus {
public:
    std::unordered_map<int, std::vector<lda_doc> > doc_stack; 
    std::unordered_map<int, std::vector<lda_doc> > test_doc_stack;
public:
    lda_Corpus();
    int mapping(std::string str);
    std::string remapping(int index);
    bool dict_insert(std::string key, int nu_count);
    bool check_if_in_dict(std::string word);
    bool redict_insert(std::string key, int nu_count);
    //bool insert_year(int insert_year);
};

class lda_doc_status: public doc_status {
public:
    lda_doc_status();
    lda_doc_status(int n, int y,int t);
};

class lda_Corpus_status: public Corpus_status {
public:
    lda_Corpus lda_cps;
    std::unordered_map<int,std::vector<lda_doc_status> > lda_doc_status_stack; 
    std::unordered_map<int,std::vector<lda_doc_status> > test_lda_doc_status_stack;
public: 
    lda_Corpus_status();
    int get_num_doc_per_yr(int yr);
    int get_document_length(int yr, int nc);
    bool set_word_length(int len);
    int get_word_index(int yr, int nc, int pos);
    int get_current_topic(int yr, int nc, int pos);
    int get_word_length();
    int get_count_per_topic_per_doc(int yr, int nc, int topic_index);
    int get_count_word_topic(int topic_index, int word_index); 
    int get_count_per_topic(int topic_index);
    
    bool word_status_update(int pos, int word_index, int yr, int nc, int new_topic, int obtopic);
    bool word_topic_status_update(int topic_index, int word_index,double beta, double sum_beta); 
    bool topic_status_per_doc_update(int topic_index, int yr, int doc_index, double alpha, double sum_alpha);
    int mapping(std::string str); // map from word to word_index
    std::string remapping(int index); // map from index to word 
    bool build_dictionary(std::string key, int nu_count);
    bool insert_year(int insert_year);
    bool init_global_status(int topic_num);
    int num_years();
    int get_year(int pos);
    bool update_topic_label(int yr, int doc_index, int word_index, int update_topic); 
    bool update_topic_count_per_doc(int yr, int doc_index,int topic, int bias);
    bool init_local_status( int yr, int T_n);
    bool check_if_in_dict(std::string word);
};

class lda_model : public topic_model {
public:
    lda_Corpus_status Corp_status;  
    
    lda_model();
    lda_model(int t_nu, double a, double b, double ratio);
    bool text_process( std::string data_file, std::string dictionary_file, double test_ratio);
    bool text_process_2(std::string folder,int start, int end);
    bool Sampler(int iter, int year);
    bool Inference_param_theta(int yr);
    bool Inference_param_phi();
    bool Inference(std::string data_file, std::string dictionary_file, double test_ratio, int iter);  
    bool display_topic_words(int topk);
    bool display_topic_word_count(int topk);
    double perplexity(int yr);
};

//1.lda_doc define

lda_doc::lda_doc():doc() {
}

lda_doc::lda_doc(int n, int y):doc(n,y){
}

bool lda_doc::update(int c){
    content.push_back(c);
}

//2. lda_Corpus define

lda_Corpus::lda_Corpus():Corpus(), doc_stack(), test_doc_stack() {
}

int lda_Corpus::mapping(std::string str){ 
    return dictionary[str];
}

std::string lda_Corpus::remapping(int index) {
    return re_dictionary[index]; 
}
bool lda_Corpus::dict_insert(std::string key, int nu_count) {
     dictionary[key] = nu_count;
     return true;
}

bool lda_Corpus::check_if_in_dict(std::string word) {
    std::unordered_map<std::string,int>::const_iterator got = dictionary.find(word);
    if (got == dictionary.end()) {
        return false;
    }
    else {
        return true;
    }
}
bool lda_Corpus::redict_insert(std::string key, int nu_count) {
    re_dictionary[nu_count] = key;
    return true;
}


/*bool lda_Corpus::insert_year(int insert_year) {
    year_stack.push_back(insert_year);
    return true;
}*/


//3. lda_doc_status
lda_doc_status::lda_doc_status():doc_status() {
}

lda_doc_status::lda_doc_status(int n, int y,int t): doc_status(n,y) {
  for (int tt = 0; tt < t; ++tt) {
      count_per_topic.push_back(0);
      topic_vector.push_back(0.0);
  }  
  for (int nt = 0; nt < n; ++nt) {
      topic_label.push_back(0);
  }
}
                                           
//4.lda_Corpus_status define

lda_Corpus_status::lda_Corpus_status():Corpus_status(),lda_cps(), lda_doc_status_stack(), test_lda_doc_status_stack() {
}

int lda_Corpus_status::get_document_length(int yr, int nc) {
    return lda_doc_status_stack[yr][nc].doc_len;
}

int lda_Corpus_status::get_word_index(int yr, int nc, int pos) {
    return lda_cps.doc_stack[yr][nc].content[pos];
}

int lda_Corpus_status::get_current_topic(int yr,int nc, int pos) {
    return lda_doc_status_stack[yr][nc].topic_label[pos];
}

int lda_Corpus_status::get_word_length() {
    return dict_len;
}

bool lda_Corpus_status::set_word_length(int len) {
    dict_len = len;
    return true;
}
 
int lda_Corpus_status::get_count_per_topic_per_doc(int yr, int nc, int topic_index){
    return lda_doc_status_stack[yr][nc].count_per_topic[topic_index];
}


int lda_Corpus_status::get_count_word_topic(int topic_index, int word_index) {
    return count_word_topic[topic_index][word_index];
}


int lda_Corpus_status::get_count_per_topic(int topic_index){
    return count_per_topic[topic_index];
}

bool lda_Corpus_status::word_status_update(int pos, int word_index, int yr, int nc, int new_topic, int obtopic) {
    //lda_doc_status_stack[yr][nc].topic_label[pos] = new_topic;
    update_topic_label(yr, nc, pos, new_topic);

    //lda_doc_status_stack[yr][nc].count_per_topic[obtopic] --;
    update_topic_count_per_doc(yr, nc, obtopic, -1);
    count_word_topic[obtopic][word_index] --;
    
    count_per_topic[obtopic] --;
    
   //lda_doc_status_stack[yr][nc].count_per_topic[new_topic] ++;
    update_topic_count_per_doc(yr, nc, new_topic, +1);
    count_word_topic[new_topic][word_index] ++;
    count_per_topic[new_topic] ++;

    return true;

} 

bool lda_Corpus_status::word_topic_status_update(int topic_index, int word_index, double beta, double sum_beta) {
    word_topic_matrix[topic_index][word_index] = ( count_word_topic[topic_index][word_index] + beta)/(sum_beta + count_per_topic[topic_index]);
}


bool lda_Corpus_status::topic_status_per_doc_update(int topic_index, int yr, int doc_index, double alpha, double sum_alpha) {
    lda_doc_status_stack[yr][doc_index].topic_vector[topic_index] =  (lda_doc_status_stack[yr][doc_index].count_per_topic[topic_index] + alpha)/(sum_alpha + get_document_length(yr, doc_index));
}

int lda_Corpus_status::get_num_doc_per_yr(int yr){
    return lda_doc_status_stack[yr].size();
}

int lda_Corpus_status::mapping(std::string str) {
    return lda_cps.mapping(str); 
}

std::string lda_Corpus_status::remapping(int index) {
    return lda_cps.remapping(index);
}

bool lda_Corpus_status::build_dictionary(std::string key, int nu_count) {
   lda_cps.dict_insert(key,nu_count);  
   lda_cps.redict_insert(key,nu_count);
   return true;
}

bool lda_Corpus_status::insert_year(int insert_year) {
     year_stack.push_back(insert_year);
     return true; 
}


bool lda_Corpus_status::init_global_status(int topic_num) {
   int word_nums = lda_cps.dictionary.size();
   for (int i = 0; i < topic_num; ++i) {
        std::vector<double> inter_vec(word_nums, 0.0);
        word_topic_matrix.push_back(inter_vec);
        std::vector<int> inter_vec2(word_nums, 0);
        count_word_topic.push_back(inter_vec2); 
        count_per_topic.push_back(0);
   } 
}

int lda_Corpus_status::num_years() {
    return year_stack.size(); 
}


int lda_Corpus_status::get_year(int pos){
    return year_stack[pos];
}

bool lda_Corpus_status::update_topic_label(int yr, int doc_index, int word_index, int update_topic) {
    lda_doc_status_stack[yr][doc_index].topic_label[word_index] = update_topic;
    return true; 
} 

bool lda_Corpus_status::update_topic_count_per_doc(int yr, int doc_index,int topic, int bias) {
    lda_doc_status_stack[yr][doc_index].count_per_topic[topic] = lda_doc_status_stack[yr][doc_index].count_per_topic[topic] + bias;
    return true;
} 

bool lda_Corpus_status::init_local_status( int yr, int T_n) {
   for (int i = 0; i < get_num_doc_per_yr(yr); ++i) {
        int doc_len = get_document_length(yr, i); 
        for ( int w = 0; w < doc_len; ++w ) {
           srand(time(NULL) * w);
           int t = rand() % ( T_n); 
           update_topic_label(yr,i,w,t); 
           update_topic_count_per_doc(yr,i,t,1);
           int w_index = get_word_index(yr, i, w); 
           //update global
           count_word_topic[t][w_index]++;
           count_per_topic[t]++; 
        }      
   } 
}


bool lda_Corpus_status::check_if_in_dict(std::string word) {
   return lda_cps.check_if_in_dict(word);
}


