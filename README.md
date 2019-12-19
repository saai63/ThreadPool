# ThreadPool
Evaluating the performance of file copy with various number of threads

Build command:
g++ -g readFileList.cpp fileOps.cpp md5checkSum.cpp msgThread.cpp main.cpp -o fileCopy -lcrypto -lssl -lpthread -D<option>
  -DCURRENT_CODE ==> Copy followed by md5sum check
  -DTHREADPOOL ==> Copy followed by md5sum check in multiple threads
  -DPARALLEL_CHECKSUM ==> Copy in main thread and checksum in another

Usage:
* Create a folder TestFiles
* Use the script createFiles.sh to create the required dummy files with their checksum.
* Modify the script sc.sh to run the application with various number of threads.
* Use sc1.sh to test the performance of current code versus calculating the checksum in a seperate thread.

I observed that the performance actually deteriorated with the increase in the number of threads.
I have posted a question on Stack over flow to clarify on the same.

https://stackoverflow.com/questions/59370872/mulithreading-does-not-help-for-io-intensive-task

The performance is improved by close to 33% when md5sum check if moved to another thread.
