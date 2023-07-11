#include <iostream>
#include <map>
#include <random>
#include <openssl/evp.h>
#include <chrono>
using namespace std;
using namespace std::chrono;
#define COLLISION_BIT 32
#define COLLISION_BYTE COLLISION_BIT/8

int main() {
	union {
		uint8_t data[COLLISION_BYTE] = {};
		uint32_t random_value;
	};
	
	uint32_t data_length = sizeof(data) / sizeof(data[0]);
	uint32_t data1 = 0;
	uint32_t data2 = 0;
	uint32_t key =0;

	OpenSSL_add_all_algorithms();
	uint8_t hash[EVP_MAX_MD_SIZE];
	uint32_t hash_length;
	uint32_t count = 0;
	map<uint32_t, uint32_t> collision_map = {};
	auto none = collision_map.find(0);

	random_device rd;
	mt19937 r_eng(rd());//�����������  
	uniform_int_distribution<uint32_t> dis;//������ֲ��� ������

	auto start = steady_clock::now();
	while(1) {
		count++;
		if (count % 10000 == 0) {
			cout << "���е�" << count << "����ײ..." << endl;
		}
		random_value= dis(r_eng);
		EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
		EVP_DigestInit_ex(mdctx, EVP_sm3(), NULL);
		EVP_DigestUpdate(mdctx, data, data_length);
		EVP_DigestFinal_ex(mdctx, hash, &hash_length);
		EVP_MD_CTX_free(mdctx);


		//cout << random_value << endl;
		//for (uint32_t i = 0; i < hash_length; i++) {
		//	printf("%02X ", hash[i]);
		//}
		//cout << endl;

		memcpy(&key, hash, COLLISION_BYTE); //get reduced hash value as key
		auto it = collision_map.find(key);
		if (it == none) { //find collision
			collision_map.insert(pair<uint32_t, uint32_t>(key, random_value));
		}
		else if(it->second!=random_value) {
			data1 = it->second;
			data2 = random_value;
			break;
		}
	}
	auto end = steady_clock::now();
	auto last = duration_cast<milliseconds>(end - start);

	random_value = data1;
	cout << "����1��";
	for (int i = 0; i < COLLISION_BYTE; i++)
	{
		printf("%02X", data[i]);
	}
	cout << endl <<"����2��";
	random_value = data2;
	for (int i = 0; i < COLLISION_BYTE; i++)
	{
		printf("%02X", data[i]);
	}
	cout << endl <<"reduced SM3��ϣֵ32λ��";
	for (int i = 0; i < COLLISION_BYTE; i++)
	{
		printf("%02X", hash[i]);
	}
	cout << endl << "����ײ������" << count << endl;
	cout <<"��ʱ��" << last.count() << "ms";
	return 0;
}