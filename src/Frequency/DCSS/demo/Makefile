main:bloom.o clock.o data.o hash_class.o csv.o main.o
	g++ -o main bloom.o clock.o data.o hash_class.o csv.o main.o -std=c++14
bloom.o:../bloom.cpp ../bloom.h ../clock.h ../data.h ../definition.h ../hash_class.h ../../../csv.h
	g++ -c ../bloom.cpp -std=c++14
clock.o:../clock.cpp ../bloom.h ../clock.h ../data.h ../definition.h ../hash_class.h ../../../csv.h
	g++ -c ../clock.cpp -std=c++14
data.o:../data.cpp ../bloom.h ../clock.h ../data.h ../definition.h ../hash_class.h ../../../csv.h
	g++ -c ../data.cpp -std=c++14
hash_class.o:../hash_class.cpp ../bloom.h ../clock.h ../data.h ../definition.h ../hash_class.h ../../../csv.h
	g++ -c ../hash_class.cpp -std=c++14
csv.o:../../../csv.cpp ../bloom.h ../clock.h ../data.h ../definition.h ../hash_class.h ../../../csv.h
	g++ -c ../../../csv.cpp -std=c++14
main.o:main.cpp ../bloom.h ../clock.h ../data.h ../definition.h ../hash_class.h ../../../csv.h
	g++ -c main.cpp -std=c++14
clean:
	rm -f *.o main