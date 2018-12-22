#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <chrono>
#include <fstream>
#include <vector>
#include <thread>

using namespace std;

int NUMBER_OF_SEATS;

// Seat Locks
pthread_mutex_t *seat_locks;

// Write Lock 
pthread_mutex_t write = PTHREAD_MUTEX_INITIALIZER;

// Global pointers to access arrays 
bool *isReserved;
int *clientIDs;

// Methods to run by clients and servers
void *handleRequest(void* _id);
void *reserveSeat(void* _request);

ofstream logfile;

// Request Object
typedef struct {
    int clientID;
    int seatNumber;
} Request;

// Define array for seat mutexes and seat availability

int main(int argc, char* argv[]) 
{
	// Read available seats
    NUMBER_OF_SEATS = atoi(argv[1]);
	// Check for validity of the given number of seats
	if(NUMBER_OF_SEATS < 50 || NUMBER_OF_SEATS > 200) {
		cout << "Number of available seats must be between 50 and 200!\n";    
		return -1;
	}
	
    // Create threads for each available seat
   	pthread_t clients[NUMBER_OF_SEATS];
    // For randomness
    srand(time(NULL));
	// Client IDs
	clientIDs = new int[NUMBER_OF_SEATS+1];
	// Reservation states
	isReserved = new bool[NUMBER_OF_SEATS+1];
	// Seat locks
	seat_locks = new pthread_mutex_t[NUMBER_OF_SEATS+1];
	// Open log file
	logfile.open ("output.txt");
	
	// Initial Log, No Need to Take Lock
	logfile << "Number of total seats: " << NUMBER_OF_SEATS << "\n";
	
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
    // Final Log, No Need to Take Lock
    logfile << "All seats are reserved.\n";
	// Close log file
	logfile.close();
	
    return 0;
}
// Try to find an available seat and make a reservation request
void *handleRequest(void* _id) 
{ 
	// Sleep for some random amount of time
	this_thread::sleep_for(chrono::milliseconds(rand() % 150 + 50));
	// Create request object
	Request request;
	// Set client id of the request
	request.clientID = *((int *)_id);
		
	while(true) {
		// Choose a seat randomly
		int seatNumber = rand() % NUMBER_OF_SEATS + 1;
		// Take the lock of the chosen seat
		pthread_mutex_lock(&seat_locks[seatNumber]);
		// Create server thread
		pthread_t server;
		// Check whether it is reserved or not
		if(!isReserved[seatNumber]) {
			// Set seat number of the request
			request.seatNumber = seatNumber;
			// Make a reservation request
			pthread_create(&server, NULL, reserveSeat, &request);
			// Wait until reservation is done
			pthread_join(server, NULL);  
			// Release the lock
			pthread_mutex_unlock(&seat_locks[seatNumber]);
			break;
		} 
		// Release the lock 
		pthread_mutex_unlock(&seat_locks[seatNumber]);
	
	}

	pthread_exit(0);
}
// Take a request and make the reservation requested
void *reserveSeat(void* _request)
{   
	// Get request object 
	Request request = *((Request *) _request);
	// Change reservation state of the chosen seat
	isReserved[request.seatNumber] = true;
	// Take the lock for writing the log
	pthread_mutex_lock(&write);
	// Write the log
	logfile << "Client" << request.clientID << " reserves Seat" << request.seatNumber << "\n";
	// Release the lock for writing the lock
	pthread_mutex_unlock(&write);
	// Exit
    pthread_exit(0); 
}
