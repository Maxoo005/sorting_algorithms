#!/usr/bin/env python3
# skrypt do odpalania badan sortowania
# python3 benchmark.py                 odpala wszystko
# python3 benchmark.py --only A B      tylko wybrane
# python3 benchmark.py --reps 30       mniej powtorzeń jak za wolno
#
# enums z Parameters.h bo nie moge importowac biblioteki cpp
#   algo:   merge=2  bucket=4  quick=5
#   struct: array=0  singleList=1  doubleList=2  stack=4
#   dist:   random=0  ascending=1  ascending50Per=2  descending=3
#   dtype:  int=0  float=1  double=2  char=3  uint=5

import argparse
import os
import subprocess
import sys
from dataclasses import dataclass
from typing import List, Optional

# slowniki na enums
ALGO   = {"merge": 2, "bucket": 4, "quick": 5}
STRUCT = {"array": 0, "singleList": 1, "doubleList": 2, "stack": 4}
DIST   = {"random": 0, "ascending": 1, "ascending50Per": 2, "descending": 3}
DTYPE  = {"int": 0, "float": 1, "double": 2, "char": 3, "uint": 5}


# jedno wywolanie programu
@dataclass
class Job:
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
    # odpala jeden job  True=ok False=blad
    args = job.to_args(bin_path)
    label = (
        f"[{idx:>3}/{total}]  "
        f"algo={job.algo}  struct={job.struct}  "
        f"size={job.size:>6}  dist={job.dist}  dtype={job.dtype}"
    )
    print(label, end="  ", flush=True)

    result = subprocess.run(args, capture_output=True, text=True)

    # wypisuje min avg max inline
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


def build_jobs_A(reps: int, reps_list: int, csv: str) -> List[Job]:
    # A  3 algo x 3 strukt x 4 rozmiary  listy dostaja mniej powtorzen bo wolne
    jobs = []
    for algo in [ALGO["merge"], ALGO["bucket"], ALGO["quick"]]:
        for struct in [STRUCT["array"], STRUCT["singleList"], STRUCT["doubleList"]]:
            is_list = struct != STRUCT["array"]
            r       = reps_list if is_list else reps
            for size in [10_000, 25_000, 50_000, 100_000]:
                jobs.append(Job(algo=algo, struct=struct, size=size, reps=r, csv=csv))
    return jobs


def build_jobs_B(reps: int, reps_list: int, csv: str) -> List[Job]:
    # B  4 rozklady x 3 algo x 2 strukt  wymagane 2 struktury na 4.0
    jobs = []
    for algo in [ALGO["merge"], ALGO["bucket"], ALGO["quick"]]:
        for struct in [STRUCT["array"], STRUCT["doubleList"]]:
            is_list = struct != STRUCT["array"]
            r = reps_list if is_list else reps
            for dist in [DIST["random"], DIST["ascending"], DIST["ascending50Per"], DIST["descending"]]:
                jobs.append(Job(
                    algo=algo, struct=struct,
                    size=25_000, reps=r, dist=dist, csv=csv
                ))
    return jobs


def build_jobs_C(reps: int, csv: str) -> List[Job]:
    # C  5 typow danych  tylko tablica zeby nie mieszac zmiennych
    jobs = []
    for algo in [ALGO["merge"], ALGO["bucket"], ALGO["quick"]]:
        for dtype in [DTYPE["int"], DTYPE["float"], DTYPE["double"], DTYPE["char"], DTYPE["uint"]]:
            jobs.append(Job(
                algo=algo, struct=STRUCT["array"],
                size=25_000, reps=reps, dtype=dtype, csv=csv
            ))
    return jobs


def build_jobs_Omega(reps: int, reps_list: int, csv: str) -> List[Job]:
    # Omega  wszystkie struktury  25k
    jobs = []
    for algo in [ALGO["merge"], ALGO["bucket"], ALGO["quick"]]:
        for struct in [STRUCT["array"], STRUCT["singleList"], STRUCT["doubleList"], STRUCT["stack"]]:
            is_list = struct != STRUCT["array"]
            r       = reps_list if is_list else reps
            jobs.append(Job(
                algo=algo, struct=struct,
                size=25_000, reps=r, csv=csv
            ))
    return jobs


def run_study(title: str, jobs: List[Job], bin_path: str) -> int:
    section(title)
    # czyszcze stary csv przed startem
    csv_files = set(job.csv for job in jobs if job.csv)
    for csv_path in csv_files:
        if os.path.exists(csv_path):
            os.remove(csv_path)
            print(f"  (wyczyszczono poprzednie wyniki: {csv_path})")
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
        help="Liczba powtórzeń dla tablic (domyślnie: 50)"
    )
    parser.add_argument(
        "--reps-list",
        type=int, default=10,
        help="Liczba powtórzeń dla struktur listowych (domyślnie: 10; listy są wolne)"
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

    # czy binarny istnieje
    if not os.path.isfile(args.bin):
        print(f"BŁĄD: nie znaleziono programu: {args.bin}", file=sys.stderr)
        print("Skompiluj projekt: cmake -S . -B build && cmake --build build", file=sys.stderr)
        sys.exit(1)

    os.makedirs(args.results_dir, exist_ok=True)

    csv = lambda name: os.path.join(args.results_dir, name)

    rl = args.reps_list
    studies = {
        "A":     ("Badanie A – Wpływ liczebności zbioru",      build_jobs_A(args.reps, rl, csv("A_size.csv"))),
        "B":     ("Badanie B – Wpływ rozkładu elementów",      build_jobs_B(args.reps, rl, csv("B_distribution.csv"))),
        "C":     ("Badanie C – Wpływ typu danych",             build_jobs_C(args.reps,     csv("C_datatype.csv"))),
        "Omega": ("Badanie Ω – Porównanie struktur + stos",    build_jobs_Omega(args.reps, rl, csv("Omega_structures.csv"))),
    }

    selected = args.only if args.only else list(studies.keys())

    print(f"Binarny: {args.bin}")
    print(f"Powtórzenia (tablice): {args.reps}")
    print(f"Powtórzenia (listy):   {args.reps_list}")
    print(f"Katalog wyników: {args.results_dir}")
    print(f"Badania do uruchomienia: {', '.join(selected)}")

    total_errors = 0
    for key in selected:
        title, jobs = studies[key]
        total_errors += run_study(title, jobs, args.bin)

    section("PODSUMOWANIE")
    csv_files = [f for f in os.listdir(args.results_dir) if f.endswith(".csv")]
    print(f"Pliki CSV w {args.results_dir}/:")
    for f in sorted(csv_files):
        path = os.path.join(args.results_dir, f)
        lines = sum(1 for _ in open(path)) - 1  # -1 naglowek
        print(f"  {f:<30}  {lines:>5} wierszy pomiarów")

    if total_errors:
        print(f"\nUWAGA: {total_errors} bledow  sprawdz co sie wysypalo")
        sys.exit(1)
    else:
        print("\nwszystko ok")


if __name__ == "__main__":
    main()
