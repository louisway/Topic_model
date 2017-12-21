from nltk.tag import StanfordNERTagger as NERtagger
from nltk.tokenize import word_tokenize as Tokener
from nltk.corpus import stopwords
from nltk.tokenize import word_tokenize
#from os import listdir 
#from os.path import isfile, join 
import os  
import os.path as Path
import xml.etree.cElementTree as ET 
import time
import re
import sys
reload(sys)
sys.setdefaultencoding('utf-8')

def get_all_filename( path ):
    return [f for f in os.listdir(path) if Path.isdir(Path.join(path,f)) or Path.isfile(Path.join(path,f))]

    

if __name__ == "__main__":
    word_list = {}
    path = "/home/louis/Desktop/NSF/"
    folder_names = get_all_filename(path)
    count = 0
    all_count = 0 
    stop_words = set(stopwords.words('english'))
    stop_words.update(['.',',','"','?',"'",'!',':',';','(',')','[',']','{','}'])
    folder_names = sorted(folder_names) 
    store_file = "/home/louis/Desktop/content.txt"
    index_file = "/home/louis/Desktop/index.txt"
    file = open(store_file,"w")
    index_file = open(index_file,"w") 
    start_time = time.time()
    for fn in folder_names:
        sub_folder = Path.join(path + fn)

        print "folder_name: " + sub_folder

        filenames = get_all_filename(sub_folder)
        for name in filenames:
           name_split = name.split('.')
           if len(name_split) < 3:
               filename = Path.join(sub_folder, name)
               tree = ET.ElementTree(file = filename)
               for e in tree.iter(tag = 'AbstractNarration'): 
                   content = e.text
                   if content != None:
                       word_tokens = word_tokenize(re.sub('[0-9]+', '', content.lower()))
                       word_clean = [word for word in word_tokens if not word in stop_words] 

                       file.write(fn)
                       file.write(" ")
                       file.write(name.split('.')[0])
                       file.write(" ")
                       file.write(str(len(word_clean)))
                       file.write(" ")

                       for word in word_clean:
                           file.write(word)
                           file.write(" ")
                           if word in word_list.keys():
                               word_list[word] = word_list[word] + 1
                           else:
                               word_list[word] = 1  
                       count = count + 1   
        #print word_list 
          
    file.close() 
    index_file.write(str(len(word_list)))
    index_file.write(" ")
    for key in word_list:
        index_file.write(key)
        index_file.write(":")
        index_file.write(word_list[key])
        index_file.write(" ")
    index_file.close() 
    print "count:" 
    print count
    print "all_count:"
    print all_count
    end_time = time.time() - start_time
    print end_time
