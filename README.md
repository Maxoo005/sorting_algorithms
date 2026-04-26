## Projekt1/src/utils
### CsvLogger
  Dopisuje wyniki pomiarów (min/avg/max) do pliku CSV
#### Namespace CsvLogger, wewnętrzny detail dla helpera timestamp() i writeHeader()
#### timestamp() — pobiera aktualny czas systemowy, formatuje YYYY-MM-DD HH:MM:SS, obsługuje Windows (localtime_s) i Linux (localtime_r) przez #ifdef _WIN32
#### append() — sprawdza czy plik istnieje (ifstream check), jeśli nie — pisze nagłówek; zawsze otwiera w trybie ios::app (dopisywanie)
#### Kolumny: timestamp;runMode;algorithm;structure;dataType;distribution;structureSize;iterations;pivot;shellParam;min_us;avg_us;max_us
#### Czyta z globalnego Parameters:: (tryb, algorytm, struktura, rozmiar itd.)

Separator ; (dla Exela) 
Nagłówek 
### FielO.hpp
### Timer.hpp
### Verify.hpp
