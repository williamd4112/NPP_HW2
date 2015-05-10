#ifndef _DATASET_H_
#define _DATASET_H_

#include <map>
#include <cstring>

template <typename K, typename V>
class Dataset {
public:
	Dataset(std::string key){

	}

	~Dataset(){
		
	}

	map<K, V> datas;
};

#endif