#include <iostream>
#include <string>
#include <sstream>
#include <winsock2.h>
#include <cstring>

#include "SpotParkir.h"

using namespace std;

#pragma comment(lib, "ws2_32.lib")

string sendRequestToServer(string request) {
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (clientSocket == INVALID_SOCKET) {
        return "{\"status\":\"error\",\"message\":\"Gagal membuat socket client.\"}";
    }

    sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    int connectResult = connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));

    if (connectResult == SOCKET_ERROR) {
        closesocket(clientSocket);
        return "{\"status\":\"error\",\"message\":\"Gagal connect ke server. Pastikan server sudah dijalankan.\"}";
    }

    send(clientSocket, request.c_str(), (int)request.length(), 0);

    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));

    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived <= 0) {
        closesocket(clientSocket);
        return "{\"status\":\"error\",\"message\":\"Tidak ada response dari server.\"}";
    }

    buffer[bytesReceived] = '\0';

    string response = string(buffer);

    closesocket(clientSocket);

    return response;
}

int main() {
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Gagal menginisialisasi Winsock." << endl;
        return 1;
    }

    string userName;
    string currentLocation;
    string destination;

    cout << "====================================================" << endl;
    cout << "Welcome to UI Smart Parking! Cari Parkirmu Sekarang!" << endl;
    cout << "====================================================" << endl;

    cout << "Berikut ini adalah database posisi awal dan tujuan:" << endl;
    cout << "PINTU, FH, FPSI, FISIP, FIB, FEB, FT, STADION, FMIPA, FIA, BALAIRUNG, RIK, FKM" << endl;
    cout << endl;

    cout << "Masukkan nama user : ";
    getline(cin, userName);

    cout << "Masukkan posisi Anda sekarang / fakultas terdekat : ";
    cin >> currentLocation;

    cout << "Masukkan tujuan Anda : ";
    cin >> destination;

    currentLocation = toUpperCase(currentLocation);
    destination = toUpperCase(destination);

    stringstream findRequest;

    findRequest << "{";
    findRequest << "\"type\":\"find\",";
    findRequest << "\"current_location\":\"" << currentLocation << "\",";
    findRequest << "\"destination\":\"" << destination << "\"";
    findRequest << "}";

    string response = sendRequestToServer(findRequest.str());

    cout << endl;
    cout << "Hasil pencarian dari server:" << endl;
    cout << response << endl;
    cout << endl;

    string status = getJsonString(response, "status");

    if (status != "success") {
        cout << "Gagal mencari parkir." << endl;
        cout << "Pesan: " << getJsonString(response, "message") << endl;

        WSACleanup();
        return 0;
    }

    string parkingId = getJsonString(response, "parking_id");
    string parkingName = getJsonString(response, "parking_name");
    string jenis = getJsonString(response, "jenis");
    int tersedia = getJsonInt(response, "tersedia");
    double score = getJsonDouble(response, "score");

    cout << "======================================" << endl;
    cout << "Rekomendasi parkir terbaik:" << endl;
    cout << "ID Parkir : " << parkingId << endl;
    cout << "Nama      : " << parkingName << endl;
    cout << "Jenis     : " << jenis << endl;
    cout << "Sisa Slot : " << tersedia << endl;
    cout << "Score     : " << score << endl;
    cout << "======================================" << endl;

    char choice;

    cout << "Apakah ingin reservasi parkir ini? (y/n): ";
    cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        stringstream reserveRequest;

        reserveRequest << "{";
        reserveRequest << "\"type\":\"reserve\",";
        reserveRequest << "\"parking_id\":\"" << parkingId << "\",";
        reserveRequest << "\"user_name\":\"" << userName << "\"";
        reserveRequest << "}";

        string reserveResponse = sendRequestToServer(reserveRequest.str());

        cout << endl;
        cout << "Response reservasi dari server:" << endl;
        cout << reserveResponse << endl;

        string reserveStatus = getJsonString(reserveResponse, "status");

        if (reserveStatus == "success") {
            cout << endl;
            cout << "Reservasi berhasil." << endl;
            cout << "Pesan: " << getJsonString(reserveResponse, "message") << endl;
        } else {
            cout << endl;
            cout << "Reservasi gagal." << endl;
            cout << "Pesan: " << getJsonString(reserveResponse, "message") << endl;
        }
    } else {
        cout << "Reservasi dibatalkan." << endl;
    }

    WSACleanup();

    return 0;
}