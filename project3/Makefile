#This target is used to compile cpp files
compile:
	g++ back.cpp -o back -pthread -std=c++11 -w
	g++ frontEnd.cpp -o front -std=c++11 -pthread -w
	g++ client.cpp -o client -std=c++11 -pthread -w
#This target is used to clear object files
clean:
	rm -rf *.out back front client
