/*
* File name: memoryHierarchy.cpp
* Author: Jordan Nicholls
* Created on: 4/14/23
* Last edited on: 5/6/23
* Description:
* This code simulates memory operations between cache, reg files, and physical memory
*
* Function Names: main
*/
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <bitset>
#include <algorithm>

using namespace std;

int main() {
	//creating cacheStruct:valid,tag,data
	//Initializing all values to 0 except memory which is has data of index+5
	struct cache{
		int valid = 0;
		int tag = 0;
		int data = 0;
	} block[8][2];
	int setNumber;
	int wordTag;
	int regFile[8] = {0};
	int mem[128];
	for (int i = 5; i < 133; i++){
		mem[i-5] = i;
	}
	int victimBlock = 0;
	string myString;
	string cacheStatus;
	//take data in and put it in string
	ifstream myfile;
	myfile.open("data.in");
	if (myfile.is_open()) {
		while (myfile) {
			getline (myfile, myString);
			if(myString.empty()){
				myfile.close();
				break;
			}
			//break up string into parts to decode
			string opcode = myString.substr(0,6);
			string rs = myString.substr(6,5);
			string rt = myString.substr(11,5);
			string offset = myString.substr(16,16);
			//calculate effective address (byte and word)
			string byteEffectiveAddress = rs + offset;
			int effectiveAddress = stoi(byteEffectiveAddress,0,2);
			int wordEffectiveAddress = effectiveAddress/4;
			setNumber = wordEffectiveAddress % 8;
			wordTag = wordEffectiveAddress/8;
			//check for hit and which block
			int hitAt1or0;
			if((block[setNumber][0].valid == 1)&&(block[setNumber][0].tag == wordTag)){
				cacheStatus = "hit";
				hitAt1or0 = 0;
			}
			else if((block[setNumber][1].valid == 1)&&(block[setNumber][1].tag == wordTag)){
				cacheStatus = "hit";
				hitAt1or0 = 1;
			}
			else {
				cacheStatus = "miss";
			}
			//determine read/write
			int decimalOpcode = stoi(opcode,0,2);
			string RWCondition;
			if (decimalOpcode == 35) {
				RWCondition = "read";
			}
			else if(decimalOpcode == 43) {
				RWCondition = "write";
			}
			//check for 4 conditions
			int decimalRT = stoi(rt,0,2);
			decimalRT -= 16;
			if (RWCondition == "read" && cacheStatus == "hit") {
				//cacheData -> reg.file[rt];
				regFile[decimalRT] = block[setNumber][hitAt1or0].data;
				if (victimBlock == 0){
					victimBlock = 1;
				}
				else {
					victimBlock = 0;
				}
			}
			else if (RWCondition == "read" && cacheStatus == "miss") {
				//set victim block
				//to "set victim block" we just use that as our block field throughout this else if loop
				//if victim valid == 1 update memory
				if(block[setNumber][victimBlock].valid == 1) {
					int memAddress = (8 * block[setNumber][victimBlock].tag)+ setNumber;
					mem[memAddress] = block[setNumber][victimBlock].data;
				}
				//bring copy of memblock to cacheblock
				block[setNumber][victimBlock].data = mem[wordEffectiveAddress];
				//set valid bit = 1 and tag field
				block[setNumber][victimBlock].valid = 1;
				block[setNumber][victimBlock].tag = wordEffectiveAddress/8;
				// cacheData -> reg.file[rt];
				regFile[decimalRT] = block[setNumber][victimBlock].data;
				if (victimBlock == 0){
					victimBlock = 1;
				}
				else {
					victimBlock = 0;
				}
			}
			else if (RWCondition == "write" && cacheStatus == "hit") {
				//reg.file[rt] -> cacheData; (update cacheOnly WBcache)
				block[setNumber][hitAt1or0].data = regFile[decimalRT];
				if (victimBlock == 0){
					victimBlock =1;
				}
				else {
					victimBlock = 0;
				}
			}
			else if (RWCondition == "write" && cacheStatus == "miss") {
				//reg.file[rt] -> mem[WordAddress]; (update mem only)
				mem[wordEffectiveAddress] = regFile[decimalRT];
			}
			//print the input and hit or miss
			cout << myString << "	" << cacheStatus << endl;
		}
		myfile.close();
	}
	else {
		cout << "Bad file read";
	}
	//output final information
	cout << "Registers" << endl;
	for (int a = 0; a<8; a++){
		cout << "$S" << a << "	" << bitset<16>(regFile[a]) << endl;
	}
	cout << "Cache Block 0						Cache Block 1" << endl;
	cout << "Set#	Valid	Tag	Data				Set#	Valid	Tag	Data" << endl;
	for(int b = 0; b<8; b++) {
		cout << b << "	" << block[b][0].valid << "	" << bitset<4>(block[b][0].tag) << "	" << bitset<16>(block[b][0].data) << "				" << b << "	" << block[b][1].valid << "	" << bitset<4>(block[b][1].tag) << "	" << bitset<16>(block[b][1].data) << "				" << endl;
	}
	cout << "Addr	Data" << endl;
	for(int c = 0; c < 128; c++){
		cout << c << "	" << bitset<16>(mem[c]) << endl;
	}
	return 0;
}