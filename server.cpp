#include <iostream>
#include <string>
#include <sstream>
#include <winsock2.h>
#include <cstring>

#include "SpotParkir.h"

using namespace std;

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Gagal menginisialisasi Winsock." << endl;
        return 1;
    }

    spotParkir* parkingList[MAX_PARKIR];
    int parkingCount = loadParkingData("parking_data.json", parkingList);

    if (parkingCount == 0) {
        cout << "Data parkir tidak tersedia." << endl;
        WSACleanup();
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket == INVALID_SOCKET) {
        cout << "Gagal membuat server socket." << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    int bindResult = bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));

    if (bindResult == SOCKET_ERROR) {
        cout << "Bind gagal. Cek apakah port 8080 sedang dipakai." << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    int listenResult = listen(serverSocket, 5);

    if (listenResult == SOCKET_ERROR) {
        cout << "Listen gagal." << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "======================================" << endl;
    cout << "        SERVER PARKIR UI Ver. 6767     " << endl;
    cout << "======================================" << endl;
    cout << "Data parkir terbaca: " << parkingCount << " tempat" << endl;
    cout << "Server berjalan di port 8080..." << endl;
    cout << "Menunggu client..." << endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);

        if (clientSocket == INVALID_SOCKET) {
            cout << "Gagal menerima client." << endl;
            continue;
        }

        char buffer[4096];
        memset(buffer, 0, sizeof(buffer));

        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived <= 0) {
            closesocket(clientSocket);
            continue;
        }

        buffer[bytesReceived] = '\0';

        string request = string(buffer);

        cout << endl;
        cout << "Request diterima:" << endl;
        cout << request << endl;

        string type = getJsonString(request, "type");
        string response;

        if (type == "find") {
            string currentLocation = getJsonString(request, "current_location");
            string destination = getJsonString(request, "destination");

            spotParkir* bestParking = NULL;
            double bestScore = 0;
            string message;

            bool found = findBestParking(
                parkingList,
                parkingCount,
                currentLocation,
                destination,
                bestParking,
                bestScore,
                message
            );

            if (found) {
                stringstream ss;

                ss << "{";
                ss << "\"status\":\"success\",";
                ss << "\"message\":\"" << message << "\",";
                ss << "\"parking_id\":\"" << bestParking->getId() << "\",";
                ss << "\"parking_name\":\"" << bestParking->getNama() << "\",";
                ss << "\"jenis\":\"" << bestParking->getJenisLabel() << "\",";
                ss << "\"tersedia\":" << bestParking->getTersedia() << ",";
                ss << "\"score\":" << bestScore;
                ss << "}";

                response = ss.str();
            } else {
                response = makeErrorResponse(message);
            }
        }
        else if (type == "reserve") {
            string parkingId = getJsonString(request, "parking_id");

            string message;

            bool success = reserveParking(
                parkingList,
                parkingCount,
                parkingId,
                message
            );

            if (success) {
                saveParkingData("parking_data.json", parkingList, parkingCount);

                stringstream ss;

                ss << "{";
                ss << "\"status\":\"success\",";
                ss << "\"message\":\"" << message << "\"";
                ss << "}";

                response = ss.str();
            } else {
                response = makeErrorResponse(message);
            }
        }
        else {
            response = makeErrorResponse("Tipe request tidak dikenali.");
        }

        send(clientSocket, response.c_str(), (int)response.length(), 0);

        cout << "Response dikirim:" << endl;
        cout << response << endl;

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}