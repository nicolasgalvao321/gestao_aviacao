#ifndef DATABASE_H
#define DATABASE_H

#define MAX_FLIGHTS 100
#define MAX_RESERVATIONS 1000

typedef struct {
    int id;
    char code[50];
    char origin[50];
    char destination[50];
    char date[20];
    char time[20];
    double price;
} Flight;

typedef struct {
    int id;
    int flight_id;
    char seat_code[10];
    char passenger_name[100];
    char passenger_document[20];
} Reservation;

typedef struct {
    Flight flights[MAX_FLIGHTS];
    int flights_count;
    Reservation reservations[MAX_RESERVATIONS];
    int reservations_count;
    int next_flight_id;
    int next_reservation_id;
} Database;

// Database functions
Database* db_create();
void db_init(Database *db);
void db_close(Database *db);

// Flight functions
Flight* db_get_flights(Database *db, int *count);
int db_add_flight(Database *db, const char *code, const char *origin, 
                  const char *destination, const char *date, 
                  const char *time, double price);

// Reservation functions
Reservation* db_get_reservations(Database *db, int *count);
int db_add_reservation(Database *db, int flight_id, const char *seat_code,
                       const char *passenger_name, const char *passenger_document);
int db_cancel_reservation(Database *db, int reservation_id);

#endif // DATABASE_H
