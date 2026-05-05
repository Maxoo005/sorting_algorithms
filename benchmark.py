#!/usr/bin/env python3
"""
Skrypt badań sortowania 
Uruchomienie:
    python3 benchmark.py                  # wszystkie badania
    python3 benchmark.py --only A B       # tylko wybrane badania
    python3 benchmark.py --bin ./build/AiZO_Projekt1 --reps 30

Wyniki trafiają do katalogu results/ jako pliki CSV.

Wartości enumów (Parameters.h):
  Algorytmy:    merge=2  bucket=4  quick=5
  Struktury:    array=0  singleList=1  doubleList=2  stack=4
  Dystrybucje:  random=0  ascending=1  ascending50Per=2  descending=3
  Typy danych:  int=0  float=1  char=3  unsigned_int=5
"""

import argparse
import os
import subprocess
import sys
from dataclasses import dataclass
from typing import List, Optional

# Mapowanie nazw na wartości enumów 
ALGO   = {"merge": 2, "bucket": 4, "quick": 5}
STRUCT = {"array": 0, "singleList": 1, "doubleList": 2, "stack": 4}
DIST   = {"random": 0, "ascending": 1, "ascending50Per": 2, "descending": 3}
DTYPE  = {"int": 0, "float": 1, "char": 3, "uint": 5}


@dataclass
class Job:
    """Pojedyncze wywołanie benchmarku."""
    algo:   int
    struct: int
    size:   int
    reps:   int
    dist:   int = 0
    dtype:  int = 0
    csv:    str = ""

    def to_args(self, bin_path: str) -> List[str]:
        args = [
            bin_path, "-b",
            "-a", str(self.algo),
            "-s", str(self.struct),
            "-l", str(self.size),
            "-n", str(self.reps),
            "-d", str(self.dist),
            "-t", str(self.dtype),
        ]
        if self.csv:
            args += ["-r", self.csv]
        return args


def run_job(job: Job, bin_path: str, idx: int, total: int) -> bool:
    """Uruchamia jedno zadanie. Zwraca True przy sukcesie."""
    args = job.to_args(bin_path)
    label = (
        f"[{idx:>3}/{total}]  "
        f"algo={job.algo}  struct={job.struct}  "
        f"size={job.size:>6}  dist={job.dist}  dtype={job.dtype}"
    )
    print(label, end="  ", flush=True)

    result = subprocess.run(args, capture_output=True, text=True)

    # wynik programu (min/avg/max) wypisany inline
    out = result.stdout.strip()
    if out:
        print(out)
    else:
        print("(brak outputu)")

    if result.returncode != 0:
        print(f"      BŁĄD (kod {result.returncode}): {result.stderr.strip()}", file=sys.stderr)
        return False
    return True


def section(title: str) -> None:
    width = 62
    print()
    print("=" * width)
    print(f"  {title}")
    print("=" * width)


def build_jobs_A(reps: int, csv: str) -> List[Job]:
    """Badanie A – każda kombinacja algorytm × struktura × rozmiar."""
    jobs = []
    for algo in [ALGO["merge"], ALGO["bucket"], ALGO["quick"]]:
        for struct in [STRUCT["array"], STRUCT["singleList"], STRUCT["doubleList"]]:
            for size in [5_000, 10_000, 25_000, 50_000]:
                jobs.append(Job(algo=algo, struct=struct, size=size, reps=reps, csv=csv))
    return jobs


def build_jobs_B(reps: int, csv: str) -> List[Job]:
    """Badanie B – merge, array + singleList, 4 rozkłady, rozmiar 25k."""
    jobs = []
    for struct in [STRUCT["array"], STRUCT["singleList"]]:
        for dist in [DIST["random"], DIST["ascending"], DIST["ascending50Per"], DIST["descending"]]:
            jobs.append(Job(
                algo=ALGO["merge"], struct=struct,
                size=25_000, reps=reps, dist=dist, csv=csv
            ))
    return jobs


def build_jobs_C(reps: int, csv: str) -> List[Job]:
    """Badanie C – merge, array, 4 typy danych, rozmiar 25k."""
    jobs = []
    for dtype in [DTYPE["int"], DTYPE["float"], DTYPE["char"], DTYPE["uint"]]:
        jobs.append(Job(
            algo=ALGO["merge"], struct=STRUCT["array"],
            size=25_000, reps=reps, dtype=dtype, csv=csv
        ))
    return jobs


def build_jobs_Omega(reps: int, csv: str) -> List[Job]:
    """Badanie Ω – merge, wszystkie struktury liniowe + stos, rozmiar 25k."""
    jobs = []
    for struct in [STRUCT["array"], STRUCT["singleList"], STRUCT["doubleList"], STRUCT["stack"]]:
        jobs.append(Job(
            algo=ALGO["merge"], struct=struct,
            size=25_000, reps=reps, csv=csv
        ))
    return jobs


def run_study(title: str, jobs: List[Job], bin_path: str) -> int:
    """Uruchamia zestaw zadań. Zwraca liczbę błędów."""
    section(title)
    errors = 0
    total = len(jobs)
    for i, job in enumerate(jobs, start=1):
        if not run_job(job, bin_path, i, total):
            errors += 1
    return errors


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Skrypt badań sortowania dla oceny 4.0"
    )
    parser.add_argument(
        "--bin",
        default="./build/AiZO_Projekt1",
        help="Ścieżka do skompilowanego programu (domyślnie: ./build/AiZO_Projekt1)"
    )
    parser.add_argument(
        "--reps", "-n",
        type=int, default=50,
        help="Liczba powtórzeń każdego pomiaru (domyślnie: 50)"
    )
    parser.add_argument(
        "--results-dir",
        default="./results",
        help="Katalog wynikowy (domyślnie: ./results)"
    )
    parser.add_argument(
        "--only",
        nargs="+",
        choices=["A", "B", "C", "Omega"],
        help="Uruchom tylko wskazane badania (domyślnie: wszystkie)"
    )
    args = parser.parse_args()

    # Walidacja binarka
    if not os.path.isfile(args.bin):
        print(f"BŁĄD: nie znaleziono programu: {args.bin}", file=sys.stderr)
        print("Skompiluj projekt: cmake -S . -B build && cmake --build build", file=sys.stderr)
        sys.exit(1)

    os.makedirs(args.results_dir, exist_ok=True)

    csv = lambda name: os.path.join(args.results_dir, name)

    studies = {
        "A":     ("Badanie A – Wpływ liczebności zbioru",      build_jobs_A(args.reps, csv("A_size.csv"))),
        "B":     ("Badanie B – Wpływ rozkładu elementów",      build_jobs_B(args.reps, csv("B_distribution.csv"))),
        "C":     ("Badanie C – Wpływ typu danych",             build_jobs_C(args.reps, csv("C_datatype.csv"))),
        "Omega": ("Badanie Ω – Porównanie struktur + stos",    build_jobs_Omega(args.reps, csv("Omega_structures.csv"))),
    }

    selected = args.only if args.only else list(studies.keys())

    print(f"Binarny: {args.bin}")
    print(f"Powtórzenia: {args.reps}")
    print(f"Katalog wyników: {args.results_dir}")
    print(f"Badania do uruchomienia: {', '.join(selected)}")

    total_errors = 0
    for key in selected:
        title, jobs = studies[key]
        total_errors += run_study(title, jobs, args.bin)

    # Podsumowanie
    section("PODSUMOWANIE")
    csv_files = [f for f in os.listdir(args.results_dir) if f.endswith(".csv")]
    print(f"Pliki CSV w {args.results_dir}/:")
    for f in sorted(csv_files):
        path = os.path.join(args.results_dir, f)
        lines = sum(1 for _ in open(path)) - 1  # odejmij nagłówek
        print(f"  {f:<30}  {lines:>5} wierszy pomiarów")

    if total_errors:
        print(f"\nUWAGA: {total_errors} błędów podczas badań – sprawdź stderr powyżej.")
        sys.exit(1)
    else:
        print("\nWszystkie badania zakończone pomyślnie.")


if __name__ == "__main__":
    main()
