# ThreadPool
Evaluating the performance of file copy with various number of threads

Build command:
g++ -g readFileList.cpp fileOps.cpp md5checkSum.cpp main.cpp -o fileCopy -lcrypto -lssl -lpthread -DTHREADPOOL

Usage:
* Create a folder TestFiles
* Use the script createFiles.sh to create the required dummy files with their checksum.
* Modify the script sc.sh to run the application with various number of threads.
