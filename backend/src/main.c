#include "database.h"
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define closesocket close
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    typedef int SOCKET;
#endif

Database *db = NULL;

// Simple JSON builders
void flights_to_json(char *buffer, int size) {
    int count = 0;
    Flight *flights = db_get_flights(db, &count);
    
    snprintf(buffer, size, "[");
    for (int i = 0; i < count; i++) {
        char flight_json[512];
        snprintf(flight_json, sizeof(flight_json),
            "{\"id\":%d,\"code\":\"%s\",\"origin\":\"%s\",\"destination\":\"%s\",\"date\":\"%s\",\"time\":\"%s\",\"price\":%.2f}%s",
            flights[i].id, flights[i].code, flights[i].origin, flights[i].destination,
            flights[i].date, flights[i].time, flights[i].price,
            (i < count - 1) ? "," : "");
        strncat(buffer, flight_json, size - strlen(buffer) - 1);
    }
    strncat(buffer, "]", size - strlen(buffer) - 1);
    
    db_free_flights(flights);
}

void reservations_to_json(char *buffer, int size) {
    int count = 0;
    Reservation *reservations = db_get_reservations(db, &count);
    
    snprintf(buffer, size, "[");
    for (int i = 0; i < count; i++) {
        char res_json[512];
        snprintf(res_json, sizeof(res_json),
            "{\"id\":%d,\"flight_id\":%d,\"seat_code\":\"%s\",\"passenger_name\":\"%s\",\"passenger_document\":\"%s\"}%s",
            reservations[i].id, reservations[i].flight_id, reservations[i].seat_code,
            reservations[i].passenger_name, reservations[i].passenger_document,
            (i < count - 1) ? "," : "");
        strncat(buffer, res_json, size - strlen(buffer) - 1);
    }
    strncat(buffer, "]", size - strlen(buffer) - 1);
    
    db_free_reservations(reservations);
}

// HTTP response builder
void send_response(SOCKET client, const char *content_type, const char *body) {
    char response[8192];
    snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %lu\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        content_type, strlen(body), body);
    
    send(client, response, strlen(response), 0);
}

// Parse URL parameters
void parse_params(const char *query, char *key, char *value, int max_len) {
    if (!query) return;
    
    const char *eq = strchr(query, '=');
    if (!eq) return;
    
    int key_len = eq - query;
    strncpy(key, query, key_len);
    key[key_len] = '\0';
    
    const char *val_start = eq + 1;
    const char *amp = strchr(val_start, '&');
    int val_len = amp ? (amp - val_start) : strlen(val_start);
    
    strncpy(value, val_start, val_len < max_len ? val_len : max_len - 1);
    value[val_len < max_len ? val_len : max_len - 1] = '\0';
}

// Handle client connection
void handle_client(SOCKET client) {
    char buffer[4096];
    int bytes_received = recv(client, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received <= 0) {
        closesocket(client);
        return;
    }
    
    buffer[bytes_received] = '\0';
    
    // Parse HTTP request
    char method[10], path[256], version[10];
    sscanf(buffer, "%s %s %s", method, path, version);
    
    char response_body[8192] = {0};
    
    // Route handling
    if (strcmp(method, "GET") == 0) {
        if (strncmp(path, "/api/flights", 12) == 0) {
            flights_to_json(response_body, sizeof(response_body));
            send_response(client, "application/json", response_body);
        }
        else if (strncmp(path, "/api/reservations", 17) == 0) {
            reservations_to_json(response_body, sizeof(response_body));
            send_response(client, "application/json", response_body);
        }
        else if (strncmp(path, "/", 1) == 0 && strcmp(path, "/") == 0) {
            // Serve index.html
            FILE *file = fopen("frontend/index.html", "r");
            if (file) {
                fread(response_body, 1, sizeof(response_body) - 1, file);
                fclose(file);
                send_response(client, "text/html", response_body);
            } else {
                send_response(client, "text/plain", "404 Not Found");
            }
        }
        else if (strstr(path, ".css")) {
            char filepath[512];
            snprintf(filepath, sizeof(filepath), "frontend%s", path);
            FILE *file = fopen(filepath, "r");
            if (file) {
                fread(response_body, 1, sizeof(response_body) - 1, file);
                fclose(file);
                send_response(client, "text/css", response_body);
            } else {
                send_response(client, "text/plain", "404 Not Found");
            }
        }
        else if (strstr(path, ".js")) {
            char filepath[512];
            snprintf(filepath, sizeof(filepath), "frontend%s", path);
            FILE *file = fopen(filepath, "r");
            if (file) {
                fread(response_body, 1, sizeof(response_body) - 1, file);
                fclose(file);
                send_response(client, "application/javascript", response_body);
            } else {
                send_response(client, "text/plain", "404 Not Found");
            }
        }
    }
    else if (strcmp(method, "POST") == 0) {
        if (strncmp(path, "/api/flights", 12) == 0) {
            // Parse POST body for flight data
            char *body = strstr(buffer, "\r\n\r\n");
            if (body) {
                body += 4;
                // Parse JSON: {"code":"...","origin":"...","destination":"...","date":"...","time":"...","price":...}
                char code[50] = {0}, origin[100] = {0}, dest[100] = {0}, date[20] = {0}, time[20] = {0};
                double price = 0;
                
                // Simple JSON parsing - skip whitespace and find values
                char *p;
                if ((p = strstr(body, "code")) != NULL) {
                    p = strchr(p, '"');
                    if (p) {
                        p++;
                        sscanf(p, "%49[^\"]", code);
                    }
                }
                if ((p = strstr(body, "origin")) != NULL) {
                    p = strchr(p, '"');
                    if (p) {
                        p++;
                        sscanf(p, "%99[^\"]", origin);
                    }
                }
                if ((p = strstr(body, "destination")) != NULL) {
                    p = strchr(p, '"');
                    if (p) {
                        p++;
                        sscanf(p, "%99[^\"]", dest);
                    }
                }
                if ((p = strstr(body, "date")) != NULL) {
                    p = strchr(p, '"');
                    if (p) {
                        p++;
                        sscanf(p, "%19[^\"]", date);
                    }
                }
                if ((p = strstr(body, "time")) != NULL) {
                    p = strchr(p, '"');
                    if (p) {
                        p++;
                        sscanf(p, "%19[^\"]", time);
                    }
                }
                if ((p = strstr(body, "price")) != NULL) {
                    p = strchr(p, ':');
                    if (p) {
                        sscanf(p + 1, "%lf", &price);
                    }
                }
                
                if (code[0] && origin[0] && dest[0] && date[0] && time[0] && price > 0) {
                    if (db_add_flight(db, code, origin, dest, date, time, price)) {
                        send_response(client, "application/json", "{\"success\":true}");
                    } else {
                        send_response(client, "application/json", "{\"success\":false}");
                    }
                } else {
                    send_response(client, "application/json", "{\"success\":false,\"error\":\"Invalid data\"}");
                }
            }
        }
        else if (strncmp(path, "/api/reservations", 17) == 0) {
            char *body = strstr(buffer, "\r\n\r\n");
            if (body) {
                body += 4;
                int flight_id = 0;
                char seat[10] = {0}, name[100] = {0}, doc[20] = {0};
                
                // Simple JSON parsing - skip whitespace and find values
                char *p;
                if ((p = strstr(body, "flight_id")) != NULL) {
                    p = strchr(p, ':');
                    if (p) {
                        sscanf(p + 1, "%d", &flight_id);
                    }
                }
                if ((p = strstr(body, "seat_code")) != NULL) {
                    p = strchr(p, '"');
                    if (p) {
                        p++;
                        sscanf(p, "%9[^\"]", seat);
                    }
                }
                if ((p = strstr(body, "passenger_name")) != NULL) {
                    p = strchr(p, '"');
                    if (p) {
                        p++;
                        sscanf(p, "%99[^\"]", name);
                    }
                }
                if ((p = strstr(body, "passenger_document")) != NULL) {
                    p = strchr(p, '"');
                    if (p) {
                        p++;
                        sscanf(p, "%19[^\"]", doc);
                    }
                }
                
                if (flight_id > 0 && seat[0] && name[0] && doc[0]) {
                    if (db_add_reservation(db, flight_id, seat, name, doc)) {
                        send_response(client, "application/json", "{\"success\":true}");
                    } else {
                        send_response(client, "application/json", "{\"success\":false}");
                    }
                } else {
                    send_response(client, "application/json", "{\"success\":false,\"error\":\"Invalid data\"}");
                }
            }
        }
    }
    else if (strcmp(method, "OPTIONS") == 0) {
        send_response(client, "text/plain", "");
    }
    
    closesocket(client);
}

int main() {
#ifdef _WIN32
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2, 2), &wsa_data);
#endif

    // Initialize database
    db = db_create();
    if (!db) {
        fprintf(stderr, "Erro ao criar banco de dados\n");
        return 1;
    }
    db_init(db);

    // Create server socket
    SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == INVALID_SOCKET) {
        fprintf(stderr, "Erro ao criar socket\n");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(8080);

    // Allow reusing the address
    int reuse = 1;
    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0) {
        fprintf(stderr, "Erro ao configurar socket\n");
        closesocket(server);
        return 1;
    }

    if (bind(server, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        fprintf(stderr, "Erro ao fazer bind na porta 8080\n");
        perror("bind");
        closesocket(server);
        return 1;
    }

    if (listen(server, 5) == SOCKET_ERROR) {
        fprintf(stderr, "Erro ao fazer listen\n");
        closesocket(server);
        return 1;
    }

    printf("Servidor iniciado em http://localhost:8080\n");

    // Accept connections
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        
        SOCKET client = accept(server, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client != INVALID_SOCKET) {
            handle_client(client);
        }
    }

    closesocket(server);
    db_close(db);

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
