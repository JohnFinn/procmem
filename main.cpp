#include <sys/stat.h>
#include <unistd.h> // read
#include <fcntl.h>
#include <string>
#include <iostream>
#include <iomanip> // setw
#include <cctype>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;


class ProcMem{
	int fd;
public:
	ProcMem(int pid){
		string fname {string("/proc/") + to_string(pid) + "/mem"};
		fd = open(fname.c_str(), O_RDWR);
		if (fd == -1){
			throw runtime_error("error openning " + fname);
		}
	}

	vector<char> get(unsigned long offset, size_t size){
		vector<char> result(size);
		lseek(fd, offset, SEEK_SET);
		read(fd, result.data(), size);
		return result;
	}

	void set(unsigned long offset, vector<char> data){
		lseek(fd, offset, SEEK_SET);
		write(fd, data.data(), data.size());
	}

	void set(unsigned long offset, string data){
		lseek(fd, offset, SEEK_SET);
		write(fd, data.c_str(), data.size());
	}

	~ProcMem(){
		close(fd);
	}

};



void print_string(unsigned char* buffer, size_t size){
	for (int i = 0; i < size; ++i){
		cout	<< setw(2) << hex << setfill('0')
				<< (unsigned int)buffer[i] << ' ';
	}
	cout << "| ";
	for (int i = 0; i < size; ++i){
		if (isprint(buffer[i]))
			cout << buffer[i];
		else
			cout << "\033[1;31m_\033[0m"; // red '_'
	}
	cout << '\n';

}

void print_buffer(vector<char> buffer, unsigned long offset){
	int incr = 10;
	for (int i = 0; i < buffer.size(); i += incr){
		cout << hex << offset + i << " | ";
		print_string((unsigned char*)buffer.data()+i, incr);
	}
}


int main(int argc, char const *argv[]){
	if (argc < 2){
		cerr << "Usage: a.out PID\n";
		return 1;
	}

	ProcMem pm{stoi(argv[1])};

	// ss addr string, sr addr xx xx xx ...
	// g addr size

	string arg, cmd;
	cout << ">>> ";
	while (cin >> cmd) {
		unsigned long address;
		cin >> hex >> address;
		cin.ignore(1);
		getline(cin, arg);

		if (cmd == "ss"){
			pm.set(address, arg);
		} else if (cmd == "sr"){
			vector<char> data;
			unsigned int byte;
			for (stringstream ss{arg}; ss >> hex >> byte;)
				data.push_back(byte);
			pm.set(address, data);
		} else if (cmd == "g"){
			auto data = pm.get(address, stoi(arg));
			print_buffer(data, address);
		} else {
			cout << "usage: ss|sr|g address arg";
		}

		cout << "\n>>> ";
	}

	cout << '\n';



	return 0;
}
