# ThreadPool
Evaluating the performance of file copy with various number of threads

Build command:
g++ -g readFileList.cpp fileOps.cpp md5checkSum.cpp main.cpp -o fileCopy -lcrypto -lssl -lpthread -DTHREADPOOL

Usage:
* Create a folder TestFiles
* Use the script createFiles.sh to create the required dummy files with their checksum.
* Modify the script sc.sh to run the application with various number of threads.

I observed that the performance actually deteriorated with the increase in the number of threads.
I have posted a question on Stack over flow to clarify on the same.

https://stackoverflow.com/questions/59370872/mulithreading-does-not-help-for-io-intensive-task
