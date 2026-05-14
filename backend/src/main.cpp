#include "database.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

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

Database db;

// Função para converter struct para JSON string
std::string flightToJson(const Flight& f) {
    std::stringstream ss;
    ss << "{\"id\":" << f.id 
       << ",\"code\":\"" << f.code 
       << "\",\"origin\":\"" << f.origin 
       << "\",\"destination\":\"" << f.destination 
       << "\",\"date\":\"" << f.date 
       << "\",\"time\":\"" << f.time 
       << "\",\"price\":" << f.price 
       << ",\"reserved_seats\":[";
    
    for (size_t i = 0; i < f.reserved_seats.size(); ++i) {
        ss << "\"" << f.reserved_seats[i] << "\"";
        if (i < f.reserved_seats.size() - 1) ss << ",";
    }
    ss << "]}";
    return ss.str();
}

std::string reservationToJson(const Reservation& r) {
    std::stringstream ss;
    ss << "{\"id\":" << r.id 
       << ",\"flight_id\":" << r.flight_id 
       << ",\"seat_code\":\"" << r.seat_code 
       << "\",\"passenger_name\":\"" << r.passenger_name 
       << "\",\"passenger_document\":\"" << r.passenger_document 
       << "\"}";
    return ss.str();
}

// Função para fazer parse de query string
std::map<std::string, std::string> parseQueryString(const std::string& query) {
    std::map<std::string, std::string> params;
    std::stringstream ss(query);
    std::string pair;
    
    while (std::getline(ss, pair, '&')) {
        size_t pos = pair.find('=');
        if (pos != std::string::npos) {
            std::string key = pair.substr(0, pos);
            std::string value = pair.substr(pos + 1);
            params[key] = value;
        }
    }
    return params;
}

// Função para fazer parse de JSON simples
std::map<std::string, std::string> parseJson(const std::string& json) {
    std::map<std::string, std::string> result;
    std::string temp = json;
    
    size_t pos = 0;
    while ((pos = temp.find("\"", pos)) != std::string::npos) {
        size_t keyStart = pos + 1;
        size_t keyEnd = temp.find("\"", keyStart);
        if (keyEnd == std::string::npos) break;
        
        std::string key = temp.substr(keyStart, keyEnd - keyStart);
        
        size_t colonPos = temp.find(":", keyEnd);
        if (colonPos == std::string::npos) break;
        
        size_t valueStart = colonPos + 1;
        while (valueStart < temp.length() && (temp[valueStart] == ' ' || temp[valueStart] == ':')) {
            valueStart++;
        }
        
        std::string value;
        if (temp[valueStart] == '"') {
            valueStart++;
            size_t valueEnd = temp.find("\"", valueStart);
            if (valueEnd == std::string::npos) break;
            value = temp.substr(valueStart, valueEnd - valueStart);
        } else {
            size_t valueEnd = temp.find(",", valueStart);
            if (valueEnd == std::string::npos) {
                valueEnd = temp.find("}", valueStart);
            }
            value = temp.substr(valueStart, valueEnd - valueStart);
            // Remove espaços
            value.erase(0, value.find_first_not_of(" "));
            value.erase(value.find_last_not_of(" ") + 1);
        }
        
        result[key] = value;
        pos = keyEnd + 1;
    }
    
    return result;
}

// Função para enviar resposta HTTP
void sendResponse(SOCKET client, int status, const std::string& body, const std::string& contentType = "application/json") {
    std::stringstream response;
    response << "HTTP/1.1 " << status << " OK\r\n";
    response << "Content-Type: " << contentType << "\r\n";
    response << "Access-Control-Allow-Origin: *\r\n";
    response << "Access-Control-Allow-Methods: GET, POST, DELETE, OPTIONS\r\n";
    response << "Access-Control-Allow-Headers: Content-Type\r\n";
    response << "Content-Length: " << body.length() << "\r\n";
    response << "Connection: close\r\n\r\n";
    response << body;
    
    std::string responseStr = response.str();
    send(client, responseStr.c_str(), responseStr.length(), 0);
}

// Função principal do servidor
void handleRequest(SOCKET client, const std::string& request) {
    std::stringstream ss(request);
    std::string method, path, version;
    ss >> method >> path >> version;
    
    // Parse do body (para POST)
    std::string body;
    size_t bodyStart = request.find("\r\n\r\n");
    if (bodyStart != std::string::npos) {
        body = request.substr(bodyStart + 4);
    }
    
    // GET /api/flights
    if (method == "GET" && path == "/api/flights") {
        auto flights = db.getFlights();
        std::stringstream response;
        response << "[";
        for (size_t i = 0; i < flights.size(); ++i) {
            response << flightToJson(flights[i]);
            if (i < flights.size() - 1) response << ",";
        }
        response << "]";
        sendResponse(client, 200, response.str());
    }
    
    // GET /api/flights/<id>
    else if (method == "GET" && path.find("/api/flights/") == 0) {
        int id = std::stoi(path.substr(13));
        auto flight = db.getFlightById(id);
        if (flight.id == 0) {
            sendResponse(client, 404, "{\"error\":\"Voo não encontrado\"}");
        } else {
            sendResponse(client, 200, flightToJson(flight));
        }
    }
    
    // POST /api/flights
    else if (method == "POST" && path == "/api/flights") {
        auto params = parseJson(body);
        bool success = db.addFlight(
            params["code"],
            params["origin"],
            params["destination"],
            params["date"],
            params["time"],
            std::stod(params["price"])
        );
        
        if (success) {
            sendResponse(client, 201, "{\"success\":true,\"message\":\"Voo adicionado com sucesso\"}");
        } else {
            sendResponse(client, 400, "{\"error\":\"Erro ao adicionar voo\"}");
        }
    }
    
    // GET /api/reservations
    else if (method == "GET" && path == "/api/reservations") {
        auto reservations = db.getReservations();
        std::stringstream response;
        response << "[";
        for (size_t i = 0; i < reservations.size(); ++i) {
            response << reservationToJson(reservations[i]);
            if (i < reservations.size() - 1) response << ",";
        }
        response << "]";
        sendResponse(client, 200, response.str());
    }
    
    // GET /api/reservations/flight/<id>
    else if (method == "GET" && path.find("/api/reservations/flight/") == 0) {
        int flight_id = std::stoi(path.substr(25));
        auto reservations = db.getReservationsByFlight(flight_id);
        std::stringstream response;
        response << "[";
        for (size_t i = 0; i < reservations.size(); ++i) {
            response << reservationToJson(reservations[i]);
            if (i < reservations.size() - 1) response << ",";
        }
        response << "]";
        sendResponse(client, 200, response.str());
    }
    
    // POST /api/reservations
    else if (method == "POST" && path == "/api/reservations") {
        auto params = parseJson(body);
        bool success = db.addReservation(
            std::stoi(params["flight_id"]),
            params["seat_code"],
            params["passenger_name"],
            params["passenger_document"]
        );
        
        if (success) {
            sendResponse(client, 201, "{\"success\":true,\"message\":\"Reserva criada com sucesso\"}");
        } else {
            sendResponse(client, 400, "{\"error\":\"Erro ao criar reserva\"}");
        }
    }
    
    // DELETE /api/reservations/<id>
    else if (method == "DELETE" && path.find("/api/reservations/") == 0) {
        int id = std::stoi(path.substr(18));
        bool success = db.cancelReservation(id);
        
        if (success) {
            sendResponse(client, 200, "{\"success\":true,\"message\":\"Reserva cancelada com sucesso\"}");
        } else {
            sendResponse(client, 400, "{\"error\":\"Erro ao cancelar reserva\"}");
        }
    }
    
    // OPTIONS (CORS)
    else if (method == "OPTIONS") {
        sendResponse(client, 200, "");
    }
    
    // 404
    else {
        sendResponse(client, 404, "{\"error\":\"Endpoint não encontrado\"}");
    }
}

int main() {
    #ifdef _WIN32
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
            std::cerr << "Erro ao inicializar Winsock" << std::endl;
            return 1;
        }
    #endif
    
    // Inicializar banco de dados
    if (!db.init()) {
        std::cerr << "Falha ao inicializar banco de dados!" << std::endl;
        return 1;
    }
    
    std::cout << "Banco de dados inicializado com sucesso!" << std::endl;
    
    // Criar socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Erro ao criar socket" << std::endl;
        return 1;
    }
    
    // Configurar endereço
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(8080);
    
    // Bind
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Erro ao fazer bind na porta 8080" << std::endl;
        closesocket(serverSocket);
        return 1;
    }
    
    // Listen
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Erro ao fazer listen" << std::endl;
        closesocket(serverSocket);
        return 1;
    }
    
    std::cout << "Servidor iniciado em http://localhost:8080" << std::endl;
    std::cout << "Aguardando conexões..." << std::endl;
    
    // Loop de aceitação de conexões
    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Erro ao aceitar conexão" << std::endl;
            continue;
        }
        
        // Receber dados
        char buffer[4096] = {0};
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::string request(buffer);
            handleRequest(clientSocket, request);
        }
        
        closesocket(clientSocket);
    }
    
    closesocket(serverSocket);
    
    #ifdef _WIN32
        WSACleanup();
    #endif
    
    return 0;
}
