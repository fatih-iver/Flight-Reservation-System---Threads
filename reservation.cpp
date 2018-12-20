#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <chrono>
#include <vector>
#include <thread>

using namespace std;

int NUMBER_OF_SEATS;

pthread_mutex_t *seat_locks;

pthread_mutex_t write = PTHREAD_MUTEX_INITIALIZER;

bool *isReserved;
int *clientIDs;

void *handleRequest(void* _id);
void *reserveSeat(void* _request);

typedef struct {
    int clientID;
    int seatNumber;
} Request;

// Define array for seat mutexes and seat availability

int main(int argc, char* argv[]) 
{
	
    NUMBER_OF_SEATS = atoi(argv[1]);
    
   	pthread_t clients[NUMBER_OF_SEATS];
	
    cout << "Number of total seats: " << NUMBER_OF_SEATS << "\n";
    
    srand(time(NULL));
	
	clientIDs = new int[NUMBER_OF_SEATS+1];
	isReserved = new bool[NUMBER_OF_SEATS+1];
	seat_locks = new pthread_mutex_t[NUMBER_OF_SEATS+1];
	
	// initialize client ids
	for (int i = 0; i <= NUMBER_OF_SEATS; i++)
		clientIDs[i] = i;
	// initialize available seats
	for (int i = 0; i <= NUMBER_OF_SEATS; i++)
		isReserved[i] = false;
	// initialize seat locks
	for (int i = 0; i <= NUMBER_OF_SEATS; i++)
		pthread_mutex_init(&seat_locks[i],NULL);
   	// initialize client threads
    for (int i = 1; i <= NUMBER_OF_SEATS; i++)
		pthread_create(&clients[i], NULL, handleRequest, &clientIDs[i]);
	// wait for client threads
	for (int i = 1; i <= NUMBER_OF_SEATS ; i++)
		pthread_join(clients[i], NULL);
    
    cout << "All seats are reserved.\n";
    return 0;
}

void *handleRequest(void* _id) 
{ 
	this_thread::sleep_for(chrono::milliseconds(rand() % 150 + 50));
	
	Request request;
	
	request.clientID = *((int *)_id);
		
	while(true) {
		
		int seatNumber = rand() % NUMBER_OF_SEATS + 1;
		
		pthread_mutex_lock(&seat_locks[seatNumber]);
		
		pthread_t server;
		
		if(!isReserved[seatNumber]) {
			request.seatNumber = seatNumber;
			pthread_create(&server, NULL, reserveSeat, &request);
			pthread_join(server, NULL);  
			pthread_mutex_unlock(&seat_locks[seatNumber]);
			break;
		} 
		
		pthread_mutex_unlock(&seat_locks[seatNumber]);
	
	}

	pthread_exit(0);
}

void *reserveSeat(void* _request)
{   
	Request request = *((Request *) _request);

	isReserved[request.seatNumber] = true;
	
	pthread_mutex_lock(&write);
	cout << "Client" << request.clientID << " reserves Seat" << request.seatNumber << "\n";
	pthread_mutex_unlock(&write);

    pthread_exit(0); 
}
