#include <openssl/sha.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>

#define PARTITION_LENGTH 4
using namespace std;

struct node {
	node* uplevel = nullptr;
	node* leftchild = nullptr;
	node* rightchild = nullptr;
	string hashvalue = "";
};

class Merkletree {
private:
	int level = 0;//除根节点之外的层数
public:
	node* root = new node;

	Merkletree(int level) {
		this->level = level;
		generatetree(level, this->root);
	}
	~Merkletree() {
		deletetree(this->root);
	}

	vector<node*> nodevec;
	void generatetree(int level, node* root) {
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
		}
		if (root->leftchild->hashvalue == "") {
			gettophash(root->leftchild);
			gettophash(root->rightchild);
		}
		string tmp = root->leftchild->hashvalue + root->rightchild->hashvalue;
		uint8_t digest[SHA256_DIGEST_LENGTH];
		SHA256(reinterpret_cast<const unsigned char*>(tmp.c_str()), PARTITION_LENGTH, digest);
		string hash;
		for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
			hash += digest[i];
		}
		root->hashvalue = hash;
		cout << hash << endl;
	}
};

void generatetext() {
	FILE* f;
	fopen_s(&f, "A Game of Thrones.txt", "r");
	
}

void logdigest(string message, uint8_t* digest) {
	cout << "Plaintext: " << message << endl;
	cout << "SHA-256 result: " << hex << setfill('0');
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		cout << setw(2) << static_cast<uint32_t>(digest[i]);
	}
	cout << endl;
}
void loghex(string tophash) {
	cout << hex << setfill('0');
	for (int i = 0; i < tophash.length(); i++) {
		cout << setw(2) << static_cast<uint32_t>(tophash[i]);
	}
	
}

int main() {
	//generatetext();
	string data; //输入数据补足为分组长度整数倍
	cin >> data;
	size_t len = data.length();
	for (int i = 0; i < (PARTITION_LENGTH - len % PARTITION_LENGTH); i++) {
		data += '0';
	}

	//建树
	Merkletree tree(static_cast<int>(log2((len - 1) / PARTITION_LENGTH + 1))+1);

	//字符串分组哈希初始化叶子结点
	for (int i = 0; i < len; i+= PARTITION_LENGTH) {
		string tmp = data.substr(i, PARTITION_LENGTH);
		uint8_t digest[SHA256_DIGEST_LENGTH];
		SHA256(reinterpret_cast<const unsigned char*>(tmp.c_str()), PARTITION_LENGTH, digest);
		for (int j = 0; j < SHA256_DIGEST_LENGTH; j++) {
			tree.nodevec[i / PARTITION_LENGTH]->hashvalue += digest[j];
		}
		logdigest(tmp, digest);
		cout << "hashvalue: " << tree.nodevec[i / PARTITION_LENGTH]->hashvalue << endl<<endl;
	}

	cout << "tophash: ";
	loghex(tree.root->hashvalue.c_str());
	
}
