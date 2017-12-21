#include "lda.hpp"
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <unordered_map>
#include <pair>

class lda_doc: doc {
    lda_doc();
}

class lda_Corpus: Corpus {
private:
    std::unordered_map<pair<int, std::vector<lda_doc> > > doc_stack; 
    std::unordered_map<pair<int, std::vector<lda_doc> > > test_doc_stack;
public:
    lda_Corpus();
}

class lda_doc_status: doc_status {
    lda_doc_status();
}

class lda_Corpus_status: Corpus_status {
public:
    lda_Corpus lda_cps;
    std::unordered_map<pair<int,std::vector<lda_doc_status> > > lda_doc_status_stack; 
    std::unordered_map<pair<int,std::vector<lda_doc_status> > > test_lda_doc_status_stack;
public: 
    lda_Corpus_status();
    int get_num_doc_per_yr(int yr);
    int get_document_length(int yr, int nc);
    int get_word_index(int yr, int nc, int pos);
    int get_current_topic(int yr, int nc, int pos);
    int get_word_length();
    int get_count_per_topic_per_doc(int yr, int nc, int topic_index);
    int get_count_word_topic(int topic_index, int word_index); 
    int get_count_per_topic(int topic_index);
    
    bool word_status_update(int pos, int word_index, int yr, int nc, int new_topic, int obtopic);
    bool word_topic_status_update(int topic_index, int word_index,doubel beta, double sum_beta); 
    bool topic_status_per_doc_update(int topic_index, int yr, int doc_index, double alpha, double sum_alpha);
}

class lda_model : topic_model {
public:
    lda_Corpus_status Corp_status;  
    
    lda_model();
    bool text_process();
    bool Sampler(int iter, int year);
    bool Inference_param_alpha(int yr);
    bool Inference_param_beta();
}                                             

lda_Corpus_status()::lda_Corpus_status():lda_cps(), lda_doc_status_stack(), test_doc_status_stack,test_ratio,year_stack(), word_topic_matrix(), count_word_topic(), count_per_topic(), doc_num(0), dict_len(0) {
//initiate  word_topic_matrix, count_word_topic,count_per_topic
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

int lda_Corpus_status::get_count_per_topic_per_doc(int yr, int nc, int topic_index){
    return lda_doc_status_stack[yr][nc].count_per_topic[topic_index];_
}


int lda_Corpus_status::get_count_word_topic(int topic_index, int word_index) {
    return count_word_topic[topic_index][word_index];
}


int lda_Corpus_status::get_count_per_topic(int topic_index){
    return count_per_topic[topic_index];
}

bool lda_Corpus_status::word_status_update(int pos, int word_index, int yr, int nc, int new_topic, int obtopic) {
    lda_doc_status_stack[yr][nc].topic_label[pos] = new_topic;

    lda_doc_status_stack[yr][nc].count_per_topic[obtopic] --;
    count_word_topic[obtopic][word_index] --;
    count_per_topic[obtopic] --;

    lda_doc_status_stack[yr][nc].count_per_topic[new_topic] ++;
    count_word_topic[new_topic][word_index] ++;
    count_per_topic[new_topic] ++;

    return true;

} 

bool lda_Corpus_status::word_topic_status_update(int topic_index, int word_index, doubel beta, double sum_beta) {
    word_topic_matrix[topic_index][word_index] = ( count_word_topic[topic_index][word_index] + beta)/(sum_beta + count_per_topic[topic_index]);
}


bool lda_Corpus_status::topic_status_per_doc_update(int topic_index, int yr, int doc_index, double alpha, double sum_alpha) {
    lda_doc_status_stack[yr][doc_index].topic_vector[topic_index] =  (lda_doc_status_stack[yr][doc_index].count_per_topic[topic_index] + alpha)/(sum_alpha + get_document_length(yr, doc_index);
}

int lda_Corpus_status::get_num_doc_per_yr(int yr){
    return lda_doc_status_stack[yr].size();
}
//

lda_Corpus::lda_Corpus():doc_stack(), dictionary(), dict_len(0), doc_num(0) {
}

lda_doc::lda_doc():doc_len(0),content(),doc_len(0),year(-1) {
}

lda_doc_status::lda_doc_status():topic_label(), doc_len(0), year(-1),count_per_topic(), topic_vector() {
//initiate count_per_topic, topic_label, topic_vector

}

lda_model::lda_model():lda_Corpus_status() {
}

bool lda_model::Sampler(int iter, int yr){
    
    if (iter < 0 || yr < 0){
        return false;
    }
    
    int num_corp = Corp_status.get_num_doc_per_yr(yr);// num of doc for year yr
 
    for (int iteration = 0; iteration < iter; ++iteration) {
        for (int nc = 0; nc < num_corp; ++nc){
            int doc_nc_len  = Corp_status.get_document_length(yr,nc);// length of doc nc
            int doc_word_index = 0;
            while( doc_word_index < doc_nc_len) {
                int w_index = Corp_status.get_word_index(yr, nc, doc_word_index);//word index
                int cur_topic_index = Corp_status.get_current_topic(yr, nc, doc_word_index);
                //here to sample
                std::vector<double> sample_record(Topic_num);
                double sum_alpha = 0.0, sum_double = 0.0;
                for( int hp = 0; hp < Topic_num; ++hp ) {
                    sum_alpha += alpha[hp];
                }
                for (int hpb = 0; hpb < Corp_status.get_word_length(); ++hpb) {
                    sum_beta += beta[hpb];
                }
                double sum_sampler = 0.0;

                for (int t = 0; t < Topic_num; ++t) {
                       //modify
                    double first_part = 0.0, second_part = 0.0;
                    if (t == cur_topic_index) {
                       first_part = (alpha[t] + Corp_status.get_count_per_topic_per_doc(yr, nc, t) - 1)/(sum_alpha +  Corp_status.get_document_length(yr, nc) - 1);
                       second_part = (beta[w_index] + Corp_status.get_count_word_topic(t,w_index) - 1)/(sum_beta + Corp_status.get_count_per_topic(t) - 1);
                    }
                    else {
                       first_part = (alpha[t] + Corp_status.get_count_per_topic_per_doc(yr,nc, t) )/(sum_alpha +  Corp_status.get_document_length(yr, nc) - 1);
                       second_part = (beta[w_index] + Corp_status.get_count_word_topic(t, w_index) )/(sum_beta + Corp_status.get_count_per_topic(t) );

                    } 
                    sample_record[t] = first_part * second_part; 
                    sum_sampler += sample_record[t]
                }
                // uniform sample 
                strand (time(NULL));
                double ui_random = rand() % RAND_MAX;
                ui_random *= sum_sampler;
                sum_sampler = 0.0;
                int T = -1;
                for (int t = 0; t < Topic_num; ++t) {
                   sum_sampler += sample_record[t];
                   if( ui_random <= sum_sampler ) {
                      T = t; 
                      break;
                   } 
                }

                if (T == -1) {
                   return false; 
                }

                if ( T != cur_topic_index ) {
                   Corp_status.word_status_update(doc_word_index,  w_index, yr, nc, T, cur_topic_index);
                }
               doc_word_index ++; 
            }
        }

    }
    return true;
}

bool lda_model::Inference_param_alpha(int yr) {
    double sum_alpha = 0.0;
    for (int i = 0; i < alpha.size() ; ++i) {
        sum_alpha += alpha[i];
    }
    for ( int m = 0; m < Corp_status.get_num_doc_per_yr(yr); ++m ) {
        for(int t = 0; t < Topic_num.get_num_doc_per_yr(yr); ++t) {  
           topic_status_per_doc_update( t, yr, m, alpha[t], sum_alpha); 
        }    
    }
    return true; 
}

bool lda_model::Inference_param_beta(){
   double sum_beta = 0.0;
   for (int j = 0; j < beta.size(); ++j) {
        sum_beta += beta[i];
   }
   for (int t = 0; t < Topic_num; ++t) {
        for ( int v = 0; v < Corp_status.get_word_length(); ++v ) {
             Corp_status.word_topic_status_update(t, v,beta[v], sum_beta); 
        }
    }
   
    
    
}

bool lda_model::text_process() {
   
}
