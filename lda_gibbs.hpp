#include "lda.hpp"
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <unordered_map>
#include <utility>
#include <string>
#include <iostream>
#include <fstream>
#include <queue>

class Compair {
public:
    bool operator()(std::pair<int, double> n1, std::pair<int, double> n2) {
        return n1.second > n2.second; 
    }
};

class Compair_count {
public:
    bool operator()(std::pair<int, int> n1, std::pair<int, int> n2) {
        return n1.second > n2.second; 
    }

};

lda_model::lda_model():topic_model(), Corp_status() {
}

lda_model::lda_model(int t_nu, double a, double b, double ratio) : topic_model(t_nu, a, b, ratio),Corp_status() {
}

//inference all
bool lda_model::Inference(std::string data_file, std::string dictionary_file, double test_ratio, int iter) {
   //text_process(data_file, dictionary_file);    
   text_process_2("/home/louis/data/",1985,1985);
   //initial global status
   Corp_status.init_global_status(Topic_num); 
   //initial local status
   for (int pos = 0; pos < Corp_status.num_years(); ++pos) {
       int yr = Corp_status.get_year(pos);
       Corp_status.init_local_status( yr, Topic_num);
   }
   //fixed hyper parameters && its extension for further convience
   for (int hp = 1; hp < Topic_num; hp++) {
       alpha.push_back(alpha[0]);
   } 
   for (int hpb = 1; hpb < Corp_status.get_word_length(); ++hpb) {
       beta.push_back(beta[0]);
   }
   //start to sample
   for (int pos = 0; pos < Corp_status.num_years(); ++pos) {
       int yr = Corp_status.get_year(pos);
       Sampler(iter, yr);
   }
   //inference alpha/beta
   for (int pos = 0; pos < Corp_status.num_years(); ++pos) {
       int yr = Corp_status.get_year(pos);
       Inference_param_theta(yr);
   }
   Inference_param_phi(); 
   display_topic_words(10);
   display_topic_word_count(10);
   
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
                double sum_alpha = 0.0, sum_beta = 0.0;
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
                    sample_record[t] = first_part * second_part * 10000; 
                    sum_sampler += sample_record[t];
                }
                // uniform sample 
                srand (time(NULL));
                double ui_random = ((double)rand()) / RAND_MAX;
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

bool lda_model::Inference_param_theta(int yr) {
    double sum_alpha = 0.0;
    for (int i = 0; i < alpha.size() ; ++i) {
        sum_alpha += alpha[i];
    }
    for ( int m = 0; m < Corp_status.get_num_doc_per_yr(yr); ++m ) {
        for(int t = 0; t < Topic_num; ++t) {  
           Corp_status.topic_status_per_doc_update( t, yr, m, alpha[t], sum_alpha); 
        }    
    }
    return true; 
}

bool lda_model::Inference_param_phi(){
   double sum_beta = 0.0;
   for (int j = 0; j < beta.size(); ++j) {
        sum_beta += beta[j];
   }
   for (int t = 0; t < Topic_num; ++t) {
        for ( int v = 0; v < Corp_status.get_word_length(); ++v ) {
             Corp_status.word_topic_status_update(t, v,beta[v], sum_beta); 
        }
    }
   
    
    
}

bool lda_model::text_process_2(std::string folder,int start, int end)  {
        std::ifstream data_Reader;
        std::ifstream test_Reader;
        std::vector<std::string> years;
        for ( int i = start; i <= end; ++i) {
            years.push_back(std::to_string(i));
        }
        std::string data_folder(folder); 
        int word_index = 0;
        std::cout << "Loading into training data" ; 
        //for (std::string year :years) {
        std::string year;
        for(int i = 0 ; i < years.size(); ++i) {
            year = years[i];
            std::cout << "<<" ;  
            data_Reader.open((data_folder + year + "/content.txt").c_str());
            int fc_num;
            std::string receiver;
            int this_year = std::stoi(year);
            Corp_status.insert_year(this_year);
            if (data_Reader.is_open()) {
                while (!data_Reader.eof()) {
                    data_Reader >> receiver;
                    data_Reader >> fc_num;
                    lda_doc new_doc(fc_num,  this_year);
                    lda_doc_status new_doc_status(fc_num,this_year,Topic_num);
                    for (int i = 0; i < fc_num; ++i) {
                        data_Reader >> receiver;
                        if (!Corp_status.check_if_in_dict(receiver)) {
                            Corp_status.build_dictionary(receiver, word_index); 
                            word_index ++;
                            new_doc.update(word_index);
                        }
                        else {
                            new_doc.update(Corp_status.mapping(receiver));
                        }
                    }
                    Corp_status.lda_cps.doc_stack[this_year].push_back(new_doc);
                    Corp_status.lda_doc_status_stack[this_year].push_back(new_doc_status);
                }      
            }  
           data_Reader.close(); 
        }
        Corp_status.set_word_length(word_index);
        std::cout << std::endl;
        std::cout << "Loading into testing data" ; 
        //for (std::string year : years) {
        for(int i = 0 ; i < years.size(); ++i) {
            year = years[i];
            std::cout << "<<";
            test_Reader.open((data_folder + year + "/test.txt").c_str());
            int fc_num;
            std::string receiver;
            int this_year = std::stoi(year);
            if (test_Reader.is_open()) {
                while(!test_Reader.eof()) {
                    test_Reader >> receiver;
                    test_Reader >> fc_num;
                    lda_doc new_doc(fc_num, this_year);
                    lda_doc_status new_doc_status(fc_num, this_year,Topic_num);
                    for (int i = 0; i < fc_num; ++i) {
                        test_Reader >> receiver;
                        if (!Corp_status.check_if_in_dict(receiver)) {
                            new_doc.update(-1);
                        }
                        else {
                            new_doc.update(Corp_status.mapping(receiver));
                        }
                    } 
                    Corp_status.lda_cps.test_doc_stack[this_year].push_back(new_doc);
                    Corp_status.test_lda_doc_status_stack[this_year].push_back(new_doc_status);

                }
            }
             
           test_Reader.close(); 
        }
        std::cout << std::endl;
        return true;
}            
 
bool lda_model::text_process(std::string data_file, std::string dictionary_file, double test_ratio) {
    	std::ifstream data_Reader;
        std::ifstream dict_Reader;
        data_Reader.open(data_file.c_str());
        dict_Reader.open(dictionary_file.c_str());
        std::string receiver;
        int fc_year;
        int fc_num;
        std::cout << " loading dictionary....." << std::endl;
        if (dict_Reader.is_open()) {
            std::string key;
            int count; 
            int nu_count = 0;
            while (!dict_Reader.eof()) {
                dict_Reader >> key;
                dict_Reader >> count;
                Corp_status.build_dictionary(key, nu_count);     
                nu_count ++;

            }
        }
        std::cout << " loading documents....." << std::endl; 
        if (data_Reader.is_open()) {
            int exist_yr = -1;
            while (!data_Reader.eof()) {
               data_Reader >> fc_year;
               data_Reader >> receiver;
               data_Reader >> fc_num; 
               if ( exist_yr != fc_year) {
                   exist_yr = fc_year;
                   Corp_status.insert_year(exist_yr);
               }
               
               lda_doc new_doc(fc_num,fc_year); 
               for (int i = 0; i < fc_num; ++i){
                   data_Reader >> receiver;
                   new_doc.update(Corp_status.mapping(receiver));
               }
               lda_doc_status new_doc_status(fc_num, fc_year, Topic_num);
               srand (time(NULL));
               double ui_random = ((double)rand()) / RAND_MAX;
               if (ui_random < test_ratio) { 
                   Corp_status.lda_cps.doc_stack[fc_year].push_back(new_doc); 
                   Corp_status.doc_status_stack[fc_year].push_back(new_doc_status);        
               }
               else {
                   Corp_status.lda_cps.test_doc_stack[fc_year].push_back(new_doc);         
                   Corp_status.test_doc_status_stack[fc_year].push_back(new_doc_status);        
               }
            }  
        }
    data_Reader.close();
    dict_Reader.close();

}

bool lda_model::display_topic_words(int topk) {
    for (int i = 0; i < Topic_num; ++i) {
        std::cout << "Topic : " << i << "   ";
        // sort the top 10
        std::priority_queue<std::pair<int, double>, std::vector<std::pair<int,double> > , Compair >  p_queue;
        for (int j = 0; j < Corp_status.word_topic_matrix[i].size(); ++j) {
            double value = Corp_status.word_topic_matrix[i][j];
            if (p_queue.size() < topk) {
                p_queue.push(std::make_pair(j, value));
            }
            else {
                if (p_queue.top().second < value) {
                    p_queue.pop();
                    p_queue.push(std::make_pair(j, value));
                }
            }
        }   
        std::vector<int> p_index;
        std::vector<double> p_value;
        while (!p_queue.empty()) {
            p_index.push_back(p_queue.top().first);
            p_value.push_back(p_queue.top().second);
            p_queue.pop();
        }
        for (int out = p_index.size() -1; out >= 0; --out) {
            std::cout << Corp_status.remapping(p_index[out]) << " : " << p_value[out] << "   "; 
        }
        std::cout << std::endl;
    }
}

bool lda_model::display_topic_word_count(int topk) {
    for (int i = 0; i < Topic_num; ++i) {
        std::cout << "Topic : " << i << "   ";
        // sort the top 10
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int,int> > , Compair_count >  p_queue;
        for (int j = 0; j < Corp_status.count_word_topic.size(); ++j) {
            int value = Corp_status.count_word_topic[i][j];
            if (p_queue.size() < topk) {
                p_queue.push(std::make_pair(j, value));
            }
            else {
                if (p_queue.top().second < value) {
                    p_queue.pop();
                    p_queue.push(std::make_pair(j, value));
                }
            }
        }   
        std::vector<int> p_index;
        std::vector<int> p_value;
        while (!p_queue.empty()) {
            p_index.push_back(p_queue.top().first);
            p_value.push_back(p_queue.top().second);
            p_queue.pop();
        }
        for (int out = p_index.size() -1; out >= 0; --out) {
            std::cout << Corp_status.remapping(p_index[out]) << " : " << p_value[out] << "   "; 
        }
        std::cout << std::endl;
    }
}
