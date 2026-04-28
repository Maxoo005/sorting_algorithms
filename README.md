# Uruchamianie i testowanie ściąga
```powershell
docker run -it --rm -v "${PWD}:/workspace" aizo-projekt1 bash
```

## Kompilacja 
```bash
rm -rf build/
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
export LD_LIBRARY_PATH=./AiZO-P1-sortingAlgorithms/release:$LD_LIBRARY_PATH
```

## Help
```bash
./build/AiZO_Projekt1 -h
```

## Tryb -f — sortowanie z pliku
```bash
printf "10\n42\n-7\n100\n0\n-99\n13\n55\n-3\n28\n1\n" > /tmp/rand.txt
printf "5\n1\n2\n3\n4\n5\n" > /tmp/asc.txt
printf "5\n5\n4\n3\n2\n1\n" > /tmp/desc.txt

./build/AiZO_Projekt1 -f -i /tmp/rand.txt -o /tmp/out_arr.txt -a 2 -s 0
cat /tmp/out_arr.txt

./build/AiZO_Projekt1 -f -i /tmp/rand.txt -o /tmp/out_sl.txt  -a 2 -s 1
cat /tmp/out_sl.txt

./build/AiZO_Projekt1 -f -i /tmp/rand.txt -o /tmp/out_dl.txt  -a 2 -s 2
cat /tmp/out_dl.txt

./build/AiZO_Projekt1 -f -i /tmp/asc.txt  -a 2 -s 0
./build/AiZO_Projekt1 -f -i /tmp/desc.txt -a 2 -s 0
```

## Tryb -b — benchmark
```bash
./build/AiZO_Projekt1 -b -l 1000  -n 50 -a 2 -s 0
./build/AiZO_Projekt1 -b -l 10000 -n 50 -a 2 -s 0
./build/AiZO_Projekt1 -b -l 1000  -n 50 -a 2 -s 1
./build/AiZO_Projekt1 -b -l 1000  -n 50 -a 2 -s 2

./build/AiZO_Projekt1 -b -l 1000 -n 50 -a 2 -s 0 -d 0
./build/AiZO_Projekt1 -b -l 1000 -n 50 -a 2 -s 0 -d 1
./build/AiZO_Projekt1 -b -l 1000 -n 50 -a 2 -s 0 -d 2
./build/AiZO_Projekt1 -b -l 1000 -n 50 -a 2 -s 0 -d 3

./build/AiZO_Projekt1 -b -l 1000 -n 50 -a 2 -s 0 -r /tmp/wyniki.csv
./build/AiZO_Projekt1 -b -l 5000 -n 50 -a 2 -s 0 -r /tmp/wyniki.csv
cat /tmp/wyniki.csv
```

## Obsługa błędów
```bash
./build/AiZO_Projekt1
./build/AiZO_Projekt1 -f -i /tmp/rand.txt -a 0 -s 0
./build/AiZO_Projekt1 -f -i /tmp/rand.txt -a 2 -s 3
./build/AiZO_Projekt1 -b -n 10 -a 2 -s 0
./build/AiZO_Projekt1 -f -i /tmp/nieistniejacy.txt -a 2 -s 0
```

---

##  Moduły pomocnicze 

Folder `utils/` zawiera zestaw lekkich narzędzi wspierających proces pomiarowy i diagnostyczny. Projekt został zrealizowany zgodnie z zasadą **Single Responsibility Principle (SRP)** – każdy moduł odpowiada za jedno konkretne zadanie.

### Główne cechy architektury:
* **Header-only:** Wszystkie narzędzia znajdują się w plikach `.hpp`, co eliminuje potrzebę osobnej kompilacji plików `.cpp` i sprzyja optymalizacji typu *inlining*.
* **Zero dependencies:** Wykorzystuję wyłącznie bibliotekę standardową C++ (STL).
* **Efektywność:** Brak narzutu dynamicznej alokacji w kluczowych sekcjach pomiarowych.

### Przegląd modułów:

#### Timer (`Timer.hpp`)
Mierzy czas wykonania algorytmów z precyzją do mikrosekund ($\mu s$).
* Wykorzystuje `std::chrono::high_resolution_clock` dla najwyższej dostępnej rozdzielczości.
* Rozdzielczość mikrosekundowa została wybrana celowo – milisekundy są zbyt mało dokładne dla małych zbiorów danych (np. $n=100$), gdzie algorytmy kończą pracę w ułamkach ms.

#### CsvLogger (`CsvLogger.hpp`)
Odpowiada za zapis wyników pomiarów (min/avg/max) do plików tekstowych.
* **Formatowanie:** Używa separatora `;` (średnik), aby zapewnić natywną kompatybilność z polską lokalizacją MS Excel.
* **Cross-platform:** Obsługuje bezpieczne pobieranie czasu systemowego (`localtime_s` na Windows / `localtime_r` na Linux) za pomocą dyrektyw preprocesora.
* **Automatyzacja:** Samodzielnie sprawdza istnienie pliku i dopisuje nagłówek tylko przy pierwszym uruchomieniu.

#### FileIO (`FileIO.hpp`)
Uniwersalny moduł do wczytywania i zapisywania danych.
* **Szablony:** Dzięki zastosowaniu `template <typename T>` obsługuje typy `int`, `float` oraz `double`.
* **Determinizm:** Pierwsza linia pliku zawsze zawiera rozmiar tablicy, co pozwala na natychmiastową alokację bez konieczności skanowania całego pliku.
* **Bezpieczeństwo:** Posiada wbudowaną obsługę błędów odczytu, zwalniającą pamięć w przypadku awarii (`delete[]`).

#### Verify (`Verify.hpp`)
Proste narzędzie diagnostyczne sprawdzające poprawność operacji.
* Weryfikuje, czy tablica jest posortowana w czasie $O(n)$.
* Wywoływane wyłącznie poza blokiem pomiarowym, dzięki czemu nie wpływa na wyniki wydajnościowe algorytmów.
