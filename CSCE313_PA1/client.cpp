/*
	Author of the starter code
    Yifan Ren
    Department of Computer Science & Engineering
    Texas A&M University
    Date: 9/15/2024
	
	Please include your Name, UIN, and the date below
	Name: Justin Busker
	UIN: 632003978
	Date: 09/29/24
*/
#include "common.h"
#include "FIFORequestChannel.h"
#include <sys/wait.h>

using namespace std;


int main (int argc, char *argv[]) {
	int opt;
	int p = -1;
	double t = -1.0;
	int e = -1;
	int m = MAX_MESSAGE;
	bool new_channel = false;
	string filename = "";

	//Add other arguments here
	while ((opt = getopt(argc, argv, "p:t:e:f:m:c")) != -1) {
		switch (opt) {
			case 'p':
				p = atoi (optarg);
				break;
			case 't':
				t = atof (optarg);
				break;
			case 'e':
				e = atoi (optarg);
				break;
			case 'f':
				filename = optarg;
				break;
			case 'm':
				m = atoi (optarg);
				break;
			case 'c':
				new_channel = true;
				break;
		}
	}

	//Task 1:
	//Run the server process as a child of the client process

	pid_t pid;

	pid = fork();

	if(pid < 0){
		std::cerr << "Fork failed" << std::endl;
		return 1;
	} else if(pid == 0){
		std::string m_string = std::to_string(m);
		char run_server[] = "./server";
		char option_m[] = "-m";
		char m_value[32];
		strcpy(m_value, m_string.c_str());
		char* server_call[] = {run_server, option_m, m_value, NULL};
		execvp(server_call[0], server_call);
		perror("execvp failed"); // Add this line
		exit(1);
	} else {
		sleep(1);

		FIFORequestChannel chan("control", FIFORequestChannel::CLIENT_SIDE);
		FIFORequestChannel* new_channel2 = nullptr; 
		FIFORequestChannel* chan_ptr = &chan; 

		//Task 4:
		//Request a new channel
		//
		if(new_channel == true){
			MESSAGE_TYPE new_channelmsg = NEWCHANNEL_MSG;
			chan.cwrite(&new_channelmsg, sizeof(MESSAGE_TYPE));

			char new_channel_name[1024];
			chan.cread(new_channel_name, sizeof(new_channel_name));

			new_channel2 = new FIFORequestChannel(new_channel_name, FIFORequestChannel::CLIENT_SIDE);
			chan_ptr = new_channel2;
		}

		FIFORequestChannel& current_chan = *chan_ptr;
		std::cout << "Current channel established" << std::endl;

		//Task 2:
		//Request data points
		//
		//
		if( p != -1 && t != -1 && e != -1){

			char* buf = new char[m];
			datamsg x(p, t, e);
			memcpy(buf, &x, sizeof(datamsg));
			chan.cwrite(buf, sizeof(datamsg));
			double reply;
			chan.cread(&reply, sizeof(double));
			cout << "For person " << p << ", at time " << t << ", the value of ecg " << e << " is " << reply << endl;
			delete[] buf;
		} else if ( p != -1 && t == -1 && e == -1){

			char* buf = new char[m];

			std::ofstream outfile("received/x1.csv");

			if(!outfile.is_open()){
				std::cerr << "Failed to open x1.csv" << std::endl;
				return 1;
			}

			double time = 0.0;
			for (int i = 0; i < 1000; ++i){
				datamsg x(p, time, 1);
				memcpy(buf, &x, sizeof(datamsg));
				chan.cwrite(buf, sizeof(datamsg));
				double reply;
				chan.cread(&reply, sizeof(double));

				datamsg y(p, time, 2);
				memcpy(buf, &y, sizeof(datamsg));
				chan.cwrite(buf, sizeof(datamsg));
				double reply2;
				chan.cread(&reply2, sizeof(double));

				outfile << time << "," << reply << "," << reply2 << std::endl;

				time = time + 0.004;
			}

			outfile.close();
			delete[] buf;
		}
		else if (!filename.empty()) {
			filemsg fm(0, 0);
			string fname = filename;

			int len = sizeof(filemsg) + (fname.size() + 1);
			char* buf2 = new char[len];
			memcpy(buf2, &fm, sizeof(filemsg));
			strcpy(buf2 + sizeof(filemsg), fname.c_str());
			current_chan.cwrite(buf2, len);

			delete[] buf2;
			__int64_t file_length;
			current_chan.cread(&file_length, sizeof(__int64_t));
			cout << "The length of " << fname << " is " << file_length << endl;

			ofstream outfile("received/" + fname, ios::binary);
			if (!outfile.is_open()) {
				cerr << "Failed to open file " << "received/" << fname << " for writing." << endl;
				return 1;
			}

			__int64_t offset = 0;
			char* recieve_buffer = new char[m];

			while (offset < file_length) {
				__int64_t remainder = file_length - offset;
				int length = static_cast<int>(min(static_cast<__int64_t>(m), remainder));

				filemsg fm(offset, length);

				int len = sizeof(filemsg) + (fname.size() + 1);
				char* buf3 = new char[len];
				memcpy(buf3, &fm, sizeof(filemsg));
				strcpy(buf3 + sizeof(filemsg), fname.c_str());
				current_chan.cwrite(buf3, len);
				current_chan.cread(recieve_buffer, length);
				outfile.seekp(offset);
				outfile.write(recieve_buffer, length);
				delete[] buf3;
				offset += length;
			}
			delete[] recieve_buffer;
			outfile.close();
		}




		if (new_channel == true) {		//if a new channel was used, send the quit message on that new channel, then delete the channel
			// Send QUIT_MSG on the new channel
			MESSAGE_TYPE quit_msg = QUIT_MSG;
			current_chan.cwrite(&quit_msg, sizeof(MESSAGE_TYPE));
		}

		//send the quite message on the control channel
		MESSAGE_TYPE m = QUIT_MSG;
		chan.cwrite(&m, sizeof(MESSAGE_TYPE));

		int status;
		waitpid(pid, &status, 0);


	} 
}
