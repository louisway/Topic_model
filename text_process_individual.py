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
import random

def get_all_filename( path ):
    return [f for f in os.listdir(path) if Path.isdir(Path.join(path,f)) or Path.isfile(Path.join(path,f))]

def do_( path,folder, stop_words,test_ratio):
       directory = Path.join( path, folder) 
       data =Path.join( "/home/louis/data/",  folder )

       if not os.path.exists(data):
           os.makedirs(data)
       
       store_file = data + "/content.txt"
       test_file = data + "/test.txt"
       store = open(store_file,"w")
       test = open(test_file,"w")
       start_time = time.time()
       filenames = get_all_filename(directory)
       file = store
       count = 1 
       for name in filenames:
           ui_random = random.uniform(0,1)
           if ui_random < test_ratio:
               file = store
           else:
               file = test 
           name_split = name.split('.')
           if len(name_split) < 3:
               filename = Path.join(directory, name)
               tree = ET.ElementTree(file = filename)
               for e in tree.iter(tag = 'AbstractNarration'): 
                   content = e.text
                   if content != None:
                       word_tokens = word_tokenize(re.sub('[0-9]+', '', content.lower()))
                       word_clean = [word for word in word_tokens if not word in stop_words] 
                       file.write(name.split('.')[0])
                       file.write(" ")
                       file.write(str(len(word_clean)))
                       file.write(" ")

                       for word in word_clean:
                           file.write(word)
                           file.write(" ")

           if count  % 1000 == 0:
               print count 
           count = count + 1 
       store.close();
       test.close(); 
       end_time = time.time() - start_time
       print end_time

if __name__ == "__main__":
    word_list = {}
    path = "/home/louis/Desktop/NSF/"
    folder_names = get_all_filename(path)
    count = 0
    all_count = 0 
    stop_words = set(stopwords.words('english'))
    stop_words.update(['.',',','"','?',"'",'!',':',';','(',')','[',']','{','}'])
    years = ["1986", "1987", "1988","1989","1990","1991","1992","1993","1994","1995","1996","1997","1998","2000","2001","2002","2003","2004","2005","2006","2007","2008","2009","2010","2011","2012","2013","2014","2015"]
    for year in years:
        print year 
        do_(path, year ,stop_words,0.9)
    
          
