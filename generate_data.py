#!/usr/bin/env python3
"""
Generator plików z danymi wejściowymi dla AiZO_Projekt1
=========================================================
Format pliku (zgodnie z treścią projektu):
    <liczba elementów>
    <wartość 1>
    <wartość 2>
    ...

Uruchomienie:
    python3 generate_data.py -n 1000 -o data/rand.txt
    python3 generate_data.py -n 1000 -o data/asc.txt  --dist ascending
    python3 generate_data.py -n 1000 -o data/desc.txt --dist descending
    python3 generate_data.py -n 1000 -o data/a50.txt  --dist ascending50
    python3 generate_data.py -n 1000 -o data/chars.txt --type char
    python3 generate_data.py --batch data/                  # generuje cały zestaw naraz
"""

import argparse
import os
import random
import struct
import sys

# ── zakresy typów ──────────────────────────────────────────────────────────────
INT_MIN  = -(2 ** 31)
INT_MAX  =   2 ** 31 - 1
UINT_MAX =   2 ** 32 - 1
# float: używamy zakresu ±1e6 (pełny ±3.4e38 jest numerycznie problematyczny)
FLOAT_MIN = -1_000_000.0
FLOAT_MAX =  1_000_000.0
# char: printable ASCII
CHAR_MIN  = 32
CHAR_MAX  = 126


def generate(n: int, dtype: str, dist: str) -> list:
    """Generuje listę n wartości danego typu i rozkładu."""

    # 1. Generuj losowe wartości
    if dtype == "int":
        data = [random.randint(INT_MIN, INT_MAX) for _ in range(n)]
    elif dtype == "uint":
        data = [random.randint(0, UINT_MAX) for _ in range(n)]
    elif dtype == "float":
        data = [random.uniform(FLOAT_MIN, FLOAT_MAX) for _ in range(n)]
    elif dtype == "char":
        data = [random.randint(CHAR_MIN, CHAR_MAX) for _ in range(n)]
    else:
        print(f"BŁĄD: nieznany typ '{dtype}'", file=sys.stderr)
        sys.exit(1)

    # 2. Zastosuj rozkład
    if dist == "random":
        pass  # już losowe
    elif dist == "ascending":
        data.sort()
    elif dist == "descending":
        data.sort(reverse=True)
    elif dist == "ascending50":
        half = n // 2
        data[:half] = sorted(data[:half])
        # druga połowa pozostaje losowa
    else:
        print(f"BŁĄD: nieznany rozkład '{dist}'", file=sys.stderr)
        sys.exit(1)

    return data


def write_file(path: str, data: list, dtype: str) -> None:
    """Zapisuje dane do pliku w formacie projektu."""
    os.makedirs(os.path.dirname(os.path.abspath(path)), exist_ok=True)

    with open(path, "w") as f:
        f.write(f"{len(data)}\n")
        for val in data:
            if dtype == "char":
                f.write(f"{chr(val)}\n")   # char jako znak, nie liczba
            elif dtype == "float":
                f.write(f"{val:.6f}\n")    # 6 miejsc po przecinku
            else:
                f.write(f"{val}\n")

    print(f"  zapisano: {path}  ({len(data)} elementów, typ={dtype}, rozkład widoczny w nazwie)")


def batch_generate(output_dir: str, sizes: list, reps: int) -> None:
    """
    Generuje kompletny zestaw plików dla wszystkich badań:
      - 4 rozmiary × 4 rozkłady × int  (Badanie A/B)
      - 1 rozmiar  × random × 4 typy   (Badanie C)
    Każdy przypadek generowany reps razy (rand_001.txt … rand_050.txt).
    """
    DISTS  = ["random", "ascending", "ascending50", "descending"]
    DTYPES = ["int", "float", "char", "uint"]
    SIZE_C = sizes[len(sizes) // 2]   # środkowy rozmiar jak w badaniu B/C

    print(f"\nBatch: {len(sizes)} rozmiarów × {len(DISTS)} rozkładów × int  +  typy dla n={SIZE_C}")
    print(f"Katalog wyjściowy: {output_dir}\n")

    # Badanie A/B – int, wszystkie rozmiary, wszystkie rozkłady
    for n in sizes:
        for dist in DISTS:
            dist_dir = os.path.join(output_dir, f"n{n}", dist)
            for rep in range(1, reps + 1):
                path = os.path.join(dist_dir, f"{rep:03d}.txt")
                data = generate(n, "int", dist)
                write_file(path, data, "int")

    # Badanie C – różne typy, środkowy rozmiar, random
    for dtype in DTYPES:
        dtype_dir = os.path.join(output_dir, f"n{SIZE_C}", "random_types", dtype)
        for rep in range(1, reps + 1):
            path = os.path.join(dtype_dir, f"{rep:03d}.txt")
            data = generate(SIZE_C, dtype, "random")
            write_file(path, data, dtype)


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Generator plików wejściowych dla AiZO_Projekt1"
    )
    parser.add_argument("-n", "--count",  type=int, default=1000,
                        help="Liczba elementów (domyślnie: 1000)")
    parser.add_argument("-o", "--output", default="data/input.txt",
                        help="Ścieżka pliku wyjściowego (domyślnie: data/input.txt)")
    parser.add_argument("--type", dest="dtype",
                        choices=["int", "uint", "float", "char"], default="int",
                        help="Typ danych (domyślnie: int)")
    parser.add_argument("--dist",
                        choices=["random", "ascending", "descending", "ascending50"],
                        default="random",
                        help="Rozkład danych (domyślnie: random)")
    parser.add_argument("--seed", type=int, default=None,
                        help="Ziarno generatora losowego (dla powtarzalności)")
    parser.add_argument("--batch", metavar="DIR", default=None,
                        help="Tryb wsadowy: generuje pełny zestaw do podanego katalogu")
    parser.add_argument("--batch-sizes", nargs="+", type=int,
                        default=[5000, 10000, 25000, 50000],
                        help="Rozmiary dla trybu wsadowego (domyślnie: 5000 10000 25000 50000)")
    parser.add_argument("--batch-reps", type=int, default=1,
                        help="Ile plików na przypadek w trybie wsadowym (domyślnie: 1)")
    args = parser.parse_args()

    if args.seed is not None:
        random.seed(args.seed)

    if args.batch:
        batch_generate(args.batch, args.batch_sizes, args.batch_reps)
    else:
        data = generate(args.count, args.dtype, args.dist)
        write_file(args.output, data, args.dtype)


if __name__ == "__main__":
    main()
