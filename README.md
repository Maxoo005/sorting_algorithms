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
