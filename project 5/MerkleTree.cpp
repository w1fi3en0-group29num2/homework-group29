#include <openssl/sha.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>

#define PARTITION_LENGTH 4
using namespace std;

void loghex(uint8_t* hash) {
	cout << hex << setfill('0');
	for (uint32_t i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		cout << setw(2) << static_cast<uint32_t>(hash[i]);
	}
	cout << endl;
}

struct node {
	node* uplevel = nullptr;
	node* leftchild = nullptr;
	node* rightchild = nullptr;
	uint8_t* hashvalue = nullptr;

};

class Merkletree {
private:
	uint32_t level = 0;//除根节点之外的层数
public:
	node* root = new node;

	Merkletree(uint32_t level) {
		this->level = level;
		generatetree(level, this->root);
	}
	~Merkletree() {
		deletetree(this->root);
	}

	vector<node*> nodevec;
	void generatetree(uint32_t level, node* root) {
		if (level--) {
			node* leftchild = new node;
			root->leftchild = leftchild;
			root->leftchild->uplevel = root;
			generatetree(level, root->leftchild);
			node* rightchild = new node;
			root->rightchild = rightchild;
			root->rightchild->uplevel = root;
			generatetree(level, root->rightchild);
		}else{
			root->hashvalue = new uint8_t[SHA256_DIGEST_LENGTH];
			nodevec.push_back(root);
			return;
		}
		return;
	}

	void deletetree(node* root) {
		if (root->leftchild) {
			deletetree(root->leftchild);
		}else if(root->rightchild) {
			deletetree(root->rightchild);
		}
		else {
			delete root;
			return;
		}
	}
	void gettophash(node* root) {
		if (root->leftchild == nullptr) {
			return;
		}//到达叶子结点，返回
		if (root->leftchild->hashvalue == nullptr) {
			gettophash(root->leftchild);
			gettophash(root->rightchild);
		}
		if (root->leftchild->hashvalue == nullptr) {
			return;
		}//左右子节点无哈希值，返回
		else if (root->rightchild->hashvalue == nullptr) {
			uint8_t digest[SHA256_DIGEST_LENGTH];
			SHA256(root->leftchild->hashvalue, PARTITION_LENGTH, digest);
			root->hashvalue = new uint8_t[SHA256_DIGEST_LENGTH];
			memcpy(root->hashvalue, digest, SHA256_DIGEST_LENGTH);
			loghex(root->hashvalue);
		}//右子节点无哈希值，返回左子节点哈希值的哈希值
		else {
			uint8_t tmp[SHA256_DIGEST_LENGTH * 2];
			memcpy(tmp, root->leftchild->hashvalue, SHA256_DIGEST_LENGTH);
			memcpy(tmp + SHA256_DIGEST_LENGTH, root->rightchild->hashvalue, SHA256_DIGEST_LENGTH);

			uint8_t digest[SHA256_DIGEST_LENGTH];
			SHA256(tmp, PARTITION_LENGTH, digest);
			root->hashvalue = new uint8_t[SHA256_DIGEST_LENGTH];
			memcpy(root->hashvalue, digest, SHA256_DIGEST_LENGTH);
			loghex(root->hashvalue);
		}
	}
};

//void generatetext() {
//	FILE* f;
//	fopen_s(&f, "A Game of Thrones.txt", "r");
//	
//}

//void logdigest(string message, uint8_t* digest) {
//	cout << "Plaintext: " << message << endl;
//	cout << "SHA-256 result: " << hex << setfill('0');
//	for (uint32_t i = 0; i < SHA256_DIGEST_LENGTH; i++) {
//		cout << setw(2) << static_cast<uint32_t>(digest[i]);
//	}
//	cout << endl;
//}

uint32_t main() {
	//generatetext();
	string data; //输入数据补足为分组长度整数倍
	cout << "any input here: ";
	cin >> data;
	size_t len = data.length();
	if (len % PARTITION_LENGTH) {
		for (uint32_t i = 0; i < (PARTITION_LENGTH - len % PARTITION_LENGTH); i++) {
			data += '0';
		}
		len = data.length();
	}

	//建树
	Merkletree tree(static_cast<uint32_t>(log2(len/ PARTITION_LENGTH-1))+1);//叶子节点数log2向上取整

	//字符串分组哈希初始化叶子结点
	for (uint32_t i = 0; i < len; i+= PARTITION_LENGTH) {
		uint8_t digest[SHA256_DIGEST_LENGTH];
		string tmp = data.substr(i, PARTITION_LENGTH);
		SHA256(reinterpret_cast<const unsigned char*>(tmp.c_str()), PARTITION_LENGTH, digest);
		memcpy(tree.nodevec[i / PARTITION_LENGTH]->hashvalue, digest, SHA256_DIGEST_LENGTH);
		
		cout << tmp << endl;
		loghex(digest);
	}

	cout <<"Intermediate node hash value: " << endl;
	tree.gettophash(tree.root);
	cout << "tophash: ";
	loghex(tree.root->hashvalue);
	
}
