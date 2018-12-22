all: reserve.out

reserve.out: reservation.cpp
	g++ -std=c++11 -o reserve.out reservation.cpp -lpthread
