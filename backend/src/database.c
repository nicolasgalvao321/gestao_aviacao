#include "database.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Database* db_create(const char *db_path) {
    Database *db = (Database*)malloc(sizeof(Database));
    if (db) {
        strcpy(db->db_path, db_path);
        db->db = NULL;
    }
    return db;
}

int db_init(Database *db) {
    int rc = sqlite3_open(db->db_path, &db->db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao abrir banco: %s\n", sqlite3_errmsg(db->db));
        return 0;
    }

    // Create tables
    const char *sql = 
        "CREATE TABLE IF NOT EXISTS aircraft ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  model TEXT NOT NULL,"
        "  seats_count INTEGER NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS flights ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  code TEXT UNIQUE NOT NULL,"
        "  origin TEXT NOT NULL,"
        "  destination TEXT NOT NULL,"
        "  date TEXT NOT NULL,"
        "  time TEXT NOT NULL,"
        "  price REAL NOT NULL,"
        "  aircraft_id INTEGER NOT NULL,"
        "  FOREIGN KEY (aircraft_id) REFERENCES aircraft(id)"
        ");"
        "CREATE TABLE IF NOT EXISTS reservations ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  flight_id INTEGER NOT NULL,"
        "  seat_code TEXT NOT NULL,"
        "  passenger_name TEXT NOT NULL,"
        "  passenger_document TEXT NOT NULL,"
        "  FOREIGN KEY (flight_id) REFERENCES flights(id)"
        ");";

    char *err_msg = NULL;
    rc = sqlite3_exec(db->db, sql, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao criar tabelas: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }

    // Insert sample data if empty
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db->db, "SELECT COUNT(*) FROM flights", -1, &stmt, NULL);
    if (rc == SQLITE_OK) {
        sqlite3_step(stmt);
        int count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);

        if (count == 0) {
            // Insert aircraft
            sqlite3_exec(db->db, 
                "INSERT INTO aircraft (model, seats_count) VALUES ('Boeing 737', 180);"
                "INSERT INTO aircraft (model, seats_count) VALUES ('Airbus A320', 150);",
                NULL, NULL, NULL);

            // Insert flights
            sqlite3_exec(db->db,
                "INSERT INTO flights (code, origin, destination, date, time, price, aircraft_id) "
                "VALUES ('BR001', 'São Paulo', 'Rio de Janeiro', '2026-05-20', '10:00', 250.00, 1);"
                "INSERT INTO flights (code, origin, destination, date, time, price, aircraft_id) "
                "VALUES ('BR002', 'São Paulo', 'Brasília', '2026-05-20', '14:00', 350.00, 2);",
                NULL, NULL, NULL);
        }
    }

    return 1;
}

void db_close(Database *db) {
    if (db && db->db) {
        sqlite3_close(db->db);
        free(db);
    }
}

Flight* db_get_flights(Database *db, int *count) {
    *count = 0;
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db->db, "SELECT id, code, origin, destination, date, time, price FROM flights", -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) return NULL;

    Flight *flights = (Flight*)malloc(sizeof(Flight) * 100);
    int idx = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        flights[idx].id = sqlite3_column_int(stmt, 0);
        strcpy(flights[idx].code, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(flights[idx].origin, (const char*)sqlite3_column_text(stmt, 2));
        strcpy(flights[idx].destination, (const char*)sqlite3_column_text(stmt, 3));
        strcpy(flights[idx].date, (const char*)sqlite3_column_text(stmt, 4));
        strcpy(flights[idx].time, (const char*)sqlite3_column_text(stmt, 5));
        flights[idx].price = sqlite3_column_double(stmt, 6);
        idx++;
    }

    sqlite3_finalize(stmt);
    *count = idx;
    return flights;
}

int db_add_flight(Database *db, const char *code, const char *origin,
                  const char *destination, const char *date,
                  const char *time, double price) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO flights (code, origin, destination, date, time, price, aircraft_id) VALUES (?, ?, ?, ?, ?, ?, 1)";
    
    int rc = sqlite3_prepare_v2(db->db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) return 0;

    sqlite3_bind_text(stmt, 1, code, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, origin, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, destination, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, time, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 6, price);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 1 : 0;
}

Reservation* db_get_reservations(Database *db, int *count) {
    *count = 0;
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db->db, "SELECT id, flight_id, seat_code, passenger_name, passenger_document FROM reservations", -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) return NULL;

    Reservation *reservations = (Reservation*)malloc(sizeof(Reservation) * 1000);
    int idx = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        reservations[idx].id = sqlite3_column_int(stmt, 0);
        reservations[idx].flight_id = sqlite3_column_int(stmt, 1);
        strcpy(reservations[idx].seat_code, (const char*)sqlite3_column_text(stmt, 2));
        strcpy(reservations[idx].passenger_name, (const char*)sqlite3_column_text(stmt, 3));
        strcpy(reservations[idx].passenger_document, (const char*)sqlite3_column_text(stmt, 4));
        idx++;
    }

    sqlite3_finalize(stmt);
    *count = idx;
    return reservations;
}

int db_add_reservation(Database *db, int flight_id, const char *seat_code,
                       const char *passenger_name, const char *passenger_document) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO reservations (flight_id, seat_code, passenger_name, passenger_document) VALUES (?, ?, ?, ?)";
    
    int rc = sqlite3_prepare_v2(db->db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) return 0;

    sqlite3_bind_int(stmt, 1, flight_id);
    sqlite3_bind_text(stmt, 2, seat_code, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, passenger_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, passenger_document, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 1 : 0;
}

int db_cancel_reservation(Database *db, int reservation_id) {
    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM reservations WHERE id = ?";
    
    int rc = sqlite3_prepare_v2(db->db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) return 0;

    sqlite3_bind_int(stmt, 1, reservation_id);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 1 : 0;
}

void db_free_flights(Flight *flights) {
    if (flights) free(flights);
}

void db_free_reservations(Reservation *reservations) {
    if (reservations) free(reservations);
}
