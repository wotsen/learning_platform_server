g++ *.cc *.cpp -o sdk_test -static -lprotobuf -luv -lrt -lpthread -L../../../lib
./sdk_test
