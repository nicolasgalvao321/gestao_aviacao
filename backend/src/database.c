#include "database.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Database* db_create() {
    Database *db = (Database*)malloc(sizeof(Database));
    if (db) {
        db->flights_count = 0;
        db->reservations_count = 0;
        db->next_flight_id = 1;
        db->next_reservation_id = 1;
    }
    return db;
}

void db_init(Database *db) {
    if (!db) return;
    
    // Insert sample flights
    db_add_flight(db, "BR001", "São Paulo", "Rio de Janeiro", "2026-05-20", "10:00", 250.00);
    db_add_flight(db, "BR002", "São Paulo", "Brasília", "2026-05-20", "14:00", 350.00);
    db_add_flight(db, "BR003", "Rio de Janeiro", "Salvador", "2026-05-21", "08:00", 400.00);
}

void db_close(Database *db) {
    if (db) free(db);
}

Flight* db_get_flights(Database *db, int *count) {
    *count = 0;
    if (!db) return NULL;
    
    Flight *flights = (Flight*)malloc(sizeof(Flight) * db->flights_count);
    if (flights) {
        memcpy(flights, db->flights, sizeof(Flight) * db->flights_count);
        *count = db->flights_count;
    }
    return flights;
}

int db_add_flight(Database *db, const char *code, const char *origin,
                  const char *destination, const char *date,
                  const char *time, double price) {
    if (!db || db->flights_count >= MAX_FLIGHTS) return 0;
    
    Flight *f = &db->flights[db->flights_count];
    f->id = db->next_flight_id++;
    strncpy(f->code, code, sizeof(f->code) - 1);
    strncpy(f->origin, origin, sizeof(f->origin) - 1);
    strncpy(f->destination, destination, sizeof(f->destination) - 1);
    strncpy(f->date, date, sizeof(f->date) - 1);
    strncpy(f->time, time, sizeof(f->time) - 1);
    f->price = price;
    
    db->flights_count++;
    return 1;
}

Reservation* db_get_reservations(Database *db, int *count) {
    *count = 0;
    if (!db) return NULL;
    
    Reservation *reservations = (Reservation*)malloc(sizeof(Reservation) * db->reservations_count);
    if (reservations) {
        memcpy(reservations, db->reservations, sizeof(Reservation) * db->reservations_count);
        *count = db->reservations_count;
    }
    return reservations;
}

int db_add_reservation(Database *db, int flight_id, const char *seat_code,
                       const char *passenger_name, const char *passenger_document) {
    if (!db || db->reservations_count >= MAX_RESERVATIONS) return 0;
    
    // Check if seat is already reserved
    for (int i = 0; i < db->reservations_count; i++) {
        if (db->reservations[i].flight_id == flight_id && 
            strcmp(db->reservations[i].seat_code, seat_code) == 0) {
            return 0; // Seat already reserved
        }
    }
    
    Reservation *r = &db->reservations[db->reservations_count];
    r->id = db->next_reservation_id++;
    r->flight_id = flight_id;
    strncpy(r->seat_code, seat_code, sizeof(r->seat_code) - 1);
    strncpy(r->passenger_name, passenger_name, sizeof(r->passenger_name) - 1);
    strncpy(r->passenger_document, passenger_document, sizeof(r->passenger_document) - 1);
    
    db->reservations_count++;
    return 1;
}

int db_cancel_reservation(Database *db, int reservation_id) {
    if (!db) return 0;
    
    for (int i = 0; i < db->reservations_count; i++) {
        if (db->reservations[i].id == reservation_id) {
            // Remove by shifting remaining elements
            for (int j = i; j < db->reservations_count - 1; j++) {
                db->reservations[j] = db->reservations[j + 1];
            }
            db->reservations_count--;
            return 1;
        }
    }
    return 0;
}

void db_free_flights(Flight *flights) {
    if (flights) free(flights);
}

void db_free_reservations(Reservation *reservations) {
    if (reservations) free(reservations);
}
