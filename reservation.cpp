#include <iostream>
#include <cstdlib>
#include <pthread.h>

int NUMBER_OF_SEATS;

using namespace std;

void *handleRequest(void*);
void *reserveSeat(void* param);

// Define array for seat mutexes and seat availability

int main(int argc, char* argv[]) 
{
    
    NUMBER_OF_SEATS = atoi(argv[1]);
    
   	pthread_t clients[NUMBER_OF_SEATS];
	
    cout << "Number of total seats: " << NUMBER_OF_SEATS << "\n";
    
    srand(time(NULL));
   	
    for (int i = 0; i < NUMBER_OF_SEATS; i++)
		pthread_create(&clients[i], NULL, handleRequest, NULL);
	
	for (int i = 0; i < NUMBER_OF_SEATS ; i++)
		pthread_join(clients[i], NULL);
    
    cout << "All seats are reserved.\n";
    return 0;
}

void *handleRequest(void*) 
{
    // Try until you get a not locked and available seat number
    int seatNumber = rand() % NUMBER_OF_SEATS + 1;
	cout << "Wll make a reservation request for: " << seatNumber << "\n";
	
	pthread_t server;
	pthread_create(&server, NULL, reserveSeat, &seatNumber);
	pthread_join(server, NULL);

	pthread_exit(0);
}

void *reserveSeat(void* param)
{   
    // Change Seat to Unavailable
    int seatNumber = *((int *)param);
    cout << "Making a reservation for: " << seatNumber << "\n";
    pthread_exit(0); 
}
