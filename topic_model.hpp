#include <uordered_map>
#include <string>
#include <vector>
#include <utility>

//raw document
class doc{
public:
    int doc_len; //the len of the document
    std::vector<int> content; //doc content with word index   
    int year; // year
    
public:
    doc();

};
 
//raw corpus
class Corpus{
private:
    //std::vector<doc> doc_stack; 
    std::unordered_map<int, std::vector<doc> > doc_stack;// year - doc_stack
    std::unordered_map<int, std::vector<doc> > test_doc_stack;
public:
    std::unordered_map<std::srtring, int > dictionary;
    std::unordered_map<int, std::string> re_dictionary;
    //int dict_len;
    //int doc_num; 
    //double test_ratio;
    //std::vector<int> year_stack;

public: 
    Corpus();
    virtual bool corpus_process();
};

//document_status
class doc_status {
public:
    int doc_len;// document length
    int year;
    std::vector<int> topic_label; //record the topic label of every word 
    std::vector<int> count_per_topic; // words count per topic, t dimension 
    std::vector<double> topic_vector; //topic vector, t dimension

public:
    doc_status(); 
};	


//corpus_status
class Corpus_status {
private:
    Corpus cps;
    std::unordered_map<int, std::vector<doc_status> > doc_status_stack;// year -doc_status_stack
    std::unordered_map<int, std::vector<doc_status> > test_doc_status_stack;
    //std::vector<doc_status> doc_status_stack;
public:
    int dict_len;
    std::vector<int> year_stack;// year - stack
    std::vector<vector<double> > word_topic_matrix;
    std::vector<vector<int> > count_word_topic;
    std::vector<int> count_per_topic;

public:
    Corpus_status();    
};


class topic_model{
    
//define parameter
protect:
    int Topic_num;   
    double test_ratio;
    std::vector<double> alpha, beta;
    virtual bool text_process();
    virtual bool Inference(int iter, int year);
private:
    Corpus cps;  
public:
    topic_model(int topic_num, double a, double b);
};
