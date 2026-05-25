# Sistem Parkir Cerdas UI

Program ini merupakan sistem rekomendasi dan reservasi parkir sederhana berbasis client-server menggunakan C++.

## Struktur Folder
Sistem Parkir Cerdas UI
`client.cpp`
`client.exe`
`json.hpp`
`parking_data.json`
`server.cpp`
`server.exe`
`SpotParkir.h`
`README.md`

## Cara Compile

Server:

```bash
g++ src/server.cpp -Iinclude -o server.exe -lws2_32
```

Client:

```bash
g++ src/client.cpp -Iinclude -o client.exe -lws2_32
```

## Menjalankan Program

Pada terminal:
```bash
./server.exe
```

Pada terminal lain:
```bash
./client.exe
```
