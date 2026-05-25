#ifndef SPOT_PARKIR_H
#define SPOT_PARKIR_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cctype>

using namespace std;

const int MAX_PARKIR = 50;
const int MAX_LOKASI = 20;

struct Koordinat {
    double x;
    double y;
};

struct dataLokasi {
    string nama;
    Koordinat koordinat;
};

struct Kandidat {
    int indeks;
    double nilai;
};

class dataJson {
public:
    virtual string toJson() = 0;
    virtual ~dataJson() {}
};

class spotParkir : public dataJson {
private:
    string id;
    string nama;
    double x;
    double y;
    int kapasitas;
    int tersedia;
    string tipe;

public:
    spotParkir() {
        id = "";
        nama = "";
        x = 0;
        y = 0;
        kapasitas = 0;
        tersedia = 0;
        tipe = "";
    }

    spotParkir(string id, string nama, double x, double y, int kapasitas, int tersedia, string tipe) {
        this->id = id;
        this->nama = nama;
        this->x = x;
        this->y = y;
        this->kapasitas = kapasitas;
        this->tersedia = tersedia;
        this->tipe = tipe;
    }

    virtual ~spotParkir() {}

    string getId() {
        return id;
    }

    string getNama() {
        return nama;
    }

    double getX() {
        return x;
    }

    double getY() {
        return y;
    }

    int getKapasitas() {
        return kapasitas;
    }

    int getTersedia() {
        return tersedia;
    }

    string getTipe() {
        return tipe;
    }

    void setTersedia(int tersedia) {
        this->tersedia = tersedia;
    }

    virtual string getJenisLabel() {
        return "Spot Parkir Umum";
    }

    virtual double hitungNilai(Koordinat userLocation, Koordinat destinationLocation) {
        double JT_MAX = 80.0;
        double JP_MAX = 120.0;

        double jt = hitungJarak(x, y, destinationLocation.x, destinationLocation.y);
        double jp = hitungJarak(x, y, userLocation.x, userLocation.y);

        double scoreJT = 100 - ((jt / JT_MAX) * 100);
        double scoreJP = 100 - ((jp / JP_MAX) * 100);

        return scoreJT + scoreJP;
    }

    double hitungJarak(double x1, double y1, double x2, double y2) {
        double dx = x1 - x2;
        double dy = y1 - y2;

        return sqrt((dx * dx) + (dy * dy));
    }

    string toJson() override {
        stringstream ss;

        ss << "{";
        ss << "\"id\":\"" << id << "\",";
        ss << "\"nama\":\"" << nama << "\",";
        ss << "\"x\":" << x << ",";
        ss << "\"y\":" << y << ",";
        ss << "\"kapasitas\":" << kapasitas << ",";
        ss << "\"tersedia\":" << tersedia << ",";
        ss << "\"tipe\":\"" << tipe << "\"";
        ss << "}";

        return ss.str();
    }
};

class spotParkirFakultas : public spotParkir {
public:
    spotParkirFakultas(string id, string nama, double x, double y, int kapasitas, int tersedia)
        : spotParkir(id, nama, x, y, kapasitas, tersedia, "fakultas") {
    }

    string getJenisLabel() override {
        return "Parkir Fakultas";
    }

    double hitungNilai(Koordinat userLocation, Koordinat destinationLocation) override {
        return spotParkir::hitungNilai(userLocation, destinationLocation) + 2;
    }
};

class spotParkirUmum : public spotParkir {
public:
    spotParkirUmum(string id, string nama, double x, double y, int kapasitas, int tersedia)
        : spotParkir(id, nama, x, y, kapasitas, tersedia, "umum") {
    }

    string getJenisLabel() override {
        return "Parkir Umum";
    }

    double hitungNilai(Koordinat userLocation, Koordinat destinationLocation) override {
        return spotParkir::hitungNilai(userLocation, destinationLocation);
    }
};

string toUpperCase(string text) {
    for (int i = 0; i < (int)text.length(); i++) {
        text[i] = toupper(text[i]);
    }

    return text;
}

string getJsonString(string jsonText, string key) {
    string pattern = "\"" + key + "\"";

    size_t keyPosition = jsonText.find(pattern);
    if (keyPosition == string::npos) return "";

    size_t colonPosition = jsonText.find(":", keyPosition);
    if (colonPosition == string::npos) return "";

    size_t firstQuote = jsonText.find("\"", colonPosition + 1);
    if (firstQuote == string::npos) return "";

    size_t secondQuote = jsonText.find("\"", firstQuote + 1);
    if (secondQuote == string::npos) return "";

    return jsonText.substr(firstQuote + 1, secondQuote - firstQuote - 1);
}

int getJsonInt(string jsonText, string key) {
    string pattern = "\"" + key + "\"";

    size_t keyPosition = jsonText.find(pattern);
    if (keyPosition == string::npos) return 0;

    size_t colonPosition = jsonText.find(":", keyPosition);
    if (colonPosition == string::npos) return 0;

    size_t start = jsonText.find_first_of("-0123456789", colonPosition + 1);
    if (start == string::npos) return 0;

    size_t end = jsonText.find_first_not_of("0123456789", start);

    string numberText = jsonText.substr(start, end - start);

    return stoi(numberText);
}

double getJsonDouble(string jsonText, string key) {
    string pattern = "\"" + key + "\"";

    size_t keyPosition = jsonText.find(pattern);
    if (keyPosition == string::npos) return 0;

    size_t colonPosition = jsonText.find(":", keyPosition);
    if (colonPosition == string::npos) return 0;

    size_t start = jsonText.find_first_of("-0123456789.", colonPosition + 1);
    if (start == string::npos) return 0;

    size_t end = jsonText.find_first_not_of("0123456789.-", start);

    string numberText = jsonText.substr(start, end - start);

    return stod(numberText);
}

int loadDefaultLocations(dataLokasi locations[]) {
    locations[0] = {"PINTU", {40, 60}};
    locations[1] = {"FH", {35, 45}};
    locations[2] = {"FPSI", {30, 30}};
    locations[3] = {"FISIP", {20, 30}};
    locations[4] = {"FIB", {10, 30}};
    locations[5] = {"FEB", {-10, 30}};
    locations[6] = {"FT", {-30, 30}};
    locations[7] = {"STADION", {-30, 0}};
    locations[8] = {"FMIPA", {0, -20}};
    locations[9] = {"FIA", {-10, -8}};
    locations[10] = {"BALAIRUNG", {30, 10}};
    locations[11] = {"RIK", {30, -20}};
    locations[12] = {"FKM", {20, -20}};

    return 13;
}

bool findLocation(dataLokasi locations[], int locationCount, string name, Koordinat &result) {
    for (int i = 0; i < locationCount; i++) {
        if (locations[i].nama == name) {
            result = locations[i].koordinat;
            return true;
        }
    }

    return false;
}

void bubbleSortCandidates(Kandidat candidates[], int candidateCount) {
    for (int i = 0; i < candidateCount - 1; i++) {
        for (int j = 0; j < candidateCount - i - 1; j++) {
            if (candidates[j].nilai < candidates[j + 1].nilai) {
                Kandidat temp = candidates[j];
                candidates[j] = candidates[j + 1];
                candidates[j + 1] = temp;
            }
        }
    }
}

int loadParkingData(string fileName, spotParkir* parkingList[]) {
    ifstream file(fileName);

    if (!file.is_open()) {
        cout << "Gagal membuka file " << fileName << endl;
        return 0;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string jsonText = buffer.str();

    file.close();

    size_t position = 0;
    int parkingCount = 0;

    while (true) {
        size_t objectStart = jsonText.find("{", position);
        if (objectStart == string::npos) break;

        size_t objectEnd = jsonText.find("}", objectStart);
        if (objectEnd == string::npos) break;

        string objectText = jsonText.substr(objectStart, objectEnd - objectStart + 1);

        string id = getJsonString(objectText, "id");
        string nama = getJsonString(objectText, "nama");
        double x = getJsonDouble(objectText, "x");
        double y = getJsonDouble(objectText, "y");
        int kapasitas = getJsonInt(objectText, "kapasitas");
        int tersedia = getJsonInt(objectText, "tersedia");
        string tipe = getJsonString(objectText, "tipe");

        if (tipe == "umum") {
            parkingList[parkingCount] = new spotParkirUmum(id, nama, x, y, kapasitas, tersedia);
        } else {
            parkingList[parkingCount] = new spotParkirFakultas(id, nama, x, y, kapasitas, tersedia);
        }

        parkingCount++;

        if (parkingCount >= MAX_PARKIR) {
            break;
        }

        position = objectEnd + 1;
    }

    return parkingCount;
}

void saveParkingData(string fileName, spotParkir* parkingList[], int parkingCount) {
    ofstream file(fileName);

    file << "[\n";

    for (int i = 0; i < parkingCount; i++) {
        file << "  " << parkingList[i]->toJson();

        if (i != parkingCount - 1) {
            file << ",";
        }

        file << "\n";
    }

    file << "]\n";

    file.close();
}

bool findBestParking(
    spotParkir* parkingList[],
    int parkingCount,
    string currentLocation,
    string destination,
    spotParkir* &bestParking,
    double &bestScore,
    string &message
) {
    dataLokasi locations[MAX_LOKASI];
    int locationCount = loadDefaultLocations(locations);

    Koordinat userCoordinate;
    Koordinat destinationCoordinate;

    currentLocation = toUpperCase(currentLocation);
    destination = toUpperCase(destination);

    bool userFound = findLocation(locations, locationCount, currentLocation, userCoordinate);
    bool destinationFound = findLocation(locations, locationCount, destination, destinationCoordinate);

    if (!userFound) {
        message = "Lokasi user tidak ditemukan.";
        return false;
    }

    if (!destinationFound) {
        message = "Tujuan tidak ditemukan.";
        return false;
    }

    Kandidat candidates[MAX_PARKIR];
    int candidateCount = 0;

    for (int i = 0; i < parkingCount; i++) {
        if (parkingList[i]->getTersedia() > 0) {
            double nilai = parkingList[i]->hitungNilai(userCoordinate, destinationCoordinate);

            candidates[candidateCount].indeks = i;
            candidates[candidateCount].nilai = nilai;
            candidateCount++;
        }
    }

    if (candidateCount == 0) {
        message = "Semua parkir penuh.";
        return false;
    }

    bubbleSortCandidates(candidates, candidateCount);

    int bestIndex = candidates[0].indeks;

    bestParking = parkingList[bestIndex];
    bestScore = candidates[0].nilai;
    message = "Parkir terbaik ditemukan.";

    return true;
}

bool reserveParking(
    spotParkir* parkingList[],
    int parkingCount,
    string parkingId,
    string &message
) {
    int foundIndex = -1;

    for (int i = 0; i < parkingCount; i++) {
        if (parkingList[i]->getId() == parkingId) {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex == -1) {
        message = "ID parkir tidak ditemukan.";
        return false;
    }

    if (parkingList[foundIndex]->getTersedia() <= 0) {
        message = "Parkir sudah penuh.";
        return false;
    }

    parkingList[foundIndex]->setTersedia(parkingList[foundIndex]->getTersedia() - 1);

    message = "Reservasi berhasil. Slot parkir telah dikurangi.";
    return true;
}

string makeErrorResponse(string message) {
    string response = "{";
    response += "\"status\":\"error\",";
    response += "\"message\":\"" + message + "\"";
    response += "}";

    return response;
}

#endif