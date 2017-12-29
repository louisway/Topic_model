#include <unordered_map>
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
    doc(int n,int y);

};
 
//raw corpus
class Corpus{
private:
    //std::vector<doc> doc_stack; 
    std::unordered_map<int, std::vector<doc> > doc_stack;// year - doc_stack
    std::unordered_map<int, std::vector<doc> > test_doc_stack;
public:
    std::unordered_map<std::string, int > dictionary;
    std::unordered_map<int, std::string> re_dictionary;
    //int dict_len;
    //int doc_num; 
    //double test_ratio;
    //std::vector<int> year_stack;

public: 
    Corpus();
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
    doc_status(int n, int y);
};	


//corpus_status
class Corpus_status {
public:
    Corpus cps;
    std::unordered_map<int, std::vector<doc_status> > doc_status_stack;// year -doc_status_stack
    std::unordered_map<int, std::vector<doc_status> > test_doc_status_stack;
    //std::vector<doc_status> doc_status_stack;
public:
    int dict_len;
    std::vector<int> year_stack;// year - stack
    std::vector<std::vector<double> > word_topic_matrix;
    std::vector<std::vector<int> > count_word_topic;
    std::vector<int> count_per_topic;

public:
    Corpus_status();    
};


class topic_model{
    
//define parameter
public:
    int Topic_num;   
    double test_ratio;
    std::vector<double> alpha, beta;
    bool text_process();
    bool Inference(int iter, int year);
private:
    Corpus cps;  
public:
    topic_model();
    topic_model(int topic_num, double a, double b, double ratio);
};

topic_model::topic_model():Topic_num(0),alpha(),beta(),test_ratio(0) {
}
 
topic_model::topic_model(int topic_num, double a, double b, double ratio):Topic_num(topic_num), alpha(1, a), beta(1, b), test_ratio(ratio) {
}

Corpus_status::Corpus_status():dict_len(0), year_stack(), word_topic_matrix(), count_word_topic(), count_per_topic(){
}

doc_status::doc_status(): doc_len(0),year(-1),topic_label(),count_per_topic(),topic_vector(){
}

doc_status::doc_status(int n, int y):doc_len(n),year(y),topic_label(),count_per_topic(), topic_vector() {
}

Corpus::Corpus():doc_stack(),test_doc_stack(),dictionary(),re_dictionary() {
}

doc::doc():doc_len(0),content(),year(-1){
}

doc::doc(int n,int y):doc_len(n), year(-1),content() {
}
