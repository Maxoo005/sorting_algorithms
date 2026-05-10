import os
import pandas as pd
import matplotlib
matplotlib.use("Agg")  # bez GUI
import matplotlib.pyplot as plt
import numpy as np

# kody (enums z Parameters.h)
ALGO   = {2: "MergeSort",  4: "BucketSort", 5: "QuickSort"}
STRUCT = {0: "Array",      1: "SingleList", 2: "DoubleList", 4: "Stack"}
DIST   = {0: "losowy",     1: "rosnący",    2: "rosnący 50%", 3: "malejący"}
DTYPE  = {0: "int",        1: "float",      2: "double",      3: "char", 5: "uint"}

# kolory spójne na wszystkich wykresach
AC      = {2: "#2196F3", 4: "#E53935", 5: "#43A047"}
SC      = {0: "#2196F3", 1: "#E53935", 2: "#43A047", 4: "#FB8C00"}
PALETTE = ["#2196F3", "#E53935", "#43A047", "#FB8C00", "#9C27B0"]

RESULTS = "./results"
PLOTS   = os.path.join(RESULTS, "plots")
os.makedirs(PLOTS, exist_ok=True)


def load(filename):
    return pd.read_csv(os.path.join(RESULTS, filename), sep=";")


def stats(df, by):
    """Zwraca mean/min/max czasu dla każdej grupy."""
    return df.groupby(by)["time_us"].agg(
        mean_us="mean", min_us="min", max_us="max"
    ).reset_index()


def err_bars(sub, key="mean_us"):
    """Zwraca tablice błędów [dół, góra] dla ax.bar(..., yerr=...)."""
    lo = (sub[key] - sub["min_us"]).values
    hi = (sub["max_us"] - sub[key]).values
    return [lo, hi]


def save(name):
    plt.tight_layout()
    plt.savefig(os.path.join(PLOTS, name), dpi=150, bbox_inches="tight")
    plt.close()
    print(f"  -> {name}")


# A  
def analyze_A():
    print("\n[A] Wpływ rozmiaru zbioru")
    df = load("A_size.csv")
    g  = stats(df, ["algorithm", "structure", "structureSize"])

    algos   = sorted(g["algorithm"].unique())
    structs = sorted(g["structure"].unique())

    # 1  wszystkie kombinacje 
    fig, ax = plt.subplots(figsize=(11, 6))
    for (al, st), sub in g.groupby(["algorithm", "structure"]):
        sub = sub.sort_values("structureSize")
        ls  = "-" if st == 0 else ("--" if st == 1 else ":")
        ax.plot(sub["structureSize"], sub["mean_us"], marker="o",
                label=f"{ALGO[al]}/{STRUCT[st]}", color=AC[al], linestyle=ls)
        ax.fill_between(sub["structureSize"], sub["min_us"], sub["max_us"],
                        alpha=0.12, color=AC[al])
    ax.set(xlabel="n", ylabel="czas [µs]", title="A – wszystkie kombinacje (liniowa)")
    ax.legend(fontsize=7, ncol=3); ax.grid(alpha=0.3)
    save("A_01_all_linear.png")

    # 3  per algorytm 
    for al in algos:
        sub_al = g[g["algorithm"] == al].sort_values("structureSize")
        fig, ax = plt.subplots(figsize=(8, 5))
        for st, sub in sub_al.groupby("structure"):
            sub = sub.sort_values("structureSize")
            ax.plot(sub["structureSize"], sub["mean_us"],
                    marker="o", label=STRUCT[st], color=SC[st])
            ax.fill_between(sub["structureSize"], sub["min_us"], sub["max_us"],
                            alpha=0.15, color=SC[st])
        ax.set(xlabel="n", ylabel="czas [µs]", title=f"A – {ALGO[al]}")
        ax.legend(); ax.grid(alpha=0.3)
        save(f"A_03_{ALGO[al]}_linear.png")

    # 5  per struktura 
    for st in structs:
        sub_st = g[g["structure"] == st].sort_values("structureSize")
        fig, ax = plt.subplots(figsize=(8, 5))
        for al, sub in sub_st.groupby("algorithm"):
            sub = sub.sort_values("structureSize")
            ax.plot(sub["structureSize"], sub["mean_us"],
                    marker="o", label=ALGO[al], color=AC[al])
            ax.fill_between(sub["structureSize"], sub["min_us"], sub["max_us"],
                            alpha=0.15, color=AC[al])
        ax.set(xlabel="n", ylabel="czas [µs]", title=f"A – {STRUCT[st]}")
        ax.legend(); ax.grid(alpha=0.3)
        save(f"A_05_{STRUCT[st]}_algos.png")


#B  
def analyze_B():
    print("\n[B] Wpływ rozkładu elementów")
    df = load("B_distribution.csv")
    g  = stats(df, ["algorithm", "structure", "distribution"])

    algos       = sorted(g["algorithm"].unique())
    structs     = sorted(g["structure"].unique())
    dists       = sorted(g["distribution"].unique())
    dist_labels = [DIST[d] for d in dists]

    # 1  per algo per struktura 
    for al in algos:
        for st in structs:
            sub = g[(g["algorithm"] == al) & (g["structure"] == st)].sort_values("distribution")
            if sub.empty:
                continue
            fig, ax = plt.subplots(figsize=(7, 4))
            ax.bar([DIST[d] for d in sub["distribution"]], sub["mean_us"],
                   yerr=err_bars(sub), capsize=5,
                   error_kw={"elinewidth": 1.2, "ecolor": "black"},
                   color=SC[st])
            ax.set(xlabel="rozkład", ylabel="czas [µs]",
                   title=f"B – {ALGO[al]} / {STRUCT[st]}")
            ax.grid(alpha=0.3, axis="y")
            save(f"B_01_{ALGO[al]}_{STRUCT[st]}.png")

    # 2  per algo 
    width = 0.35
    x     = np.arange(len(dists))
    for al in algos:
        sub_al = g[g["algorithm"] == al]
        avail  = sorted(sub_al["structure"].unique())
        if not avail:
            continue
        fig, ax = plt.subplots(figsize=(9, 5))
        for i, st in enumerate(avail):
            sub = sub_al[sub_al["structure"] == st].sort_values("distribution")
            vals = [sub[sub["distribution"] == d]["mean_us"].values[0]
                    if not sub[sub["distribution"] == d].empty else 0 for d in dists]
            lo   = [sub[sub["distribution"] == d]["min_us"].values[0]
                    if not sub[sub["distribution"] == d].empty else 0 for d in dists]
            hi   = [sub[sub["distribution"] == d]["max_us"].values[0]
                    if not sub[sub["distribution"] == d].empty else 0 for d in dists]
            yerr = [np.array(vals) - np.array(lo), np.array(hi) - np.array(vals)]
            offset = (i - len(avail) / 2 + 0.5) * width
            ax.bar(x + offset, vals, width, label=STRUCT[st], color=SC[st],
                   yerr=yerr, capsize=4, error_kw={"elinewidth": 1, "ecolor": "black"})
        ax.set_xticks(x); ax.set_xticklabels(dist_labels)
        ax.set_yscale("log")
        ax.set(xlabel="rozkład", ylabel="czas [µs] (log)",
               title=f"B – {ALGO[al]} – struktury vs rozkład")
        ax.legend(); ax.grid(alpha=0.3, axis="y", which="both")
        save(f"B_02_{ALGO[al]}_structs_vs_dist.png")

    # 3  per struktura
    width = 0.25
    for st in structs:
        sub_st = g[g["structure"] == st]
        if sub_st.empty:
            continue
        fig, ax = plt.subplots(figsize=(9, 5))
        for i, al in enumerate(algos):
            sub = sub_st[sub_st["algorithm"] == al].sort_values("distribution")
            if sub.empty:
                continue
            vals = [sub[sub["distribution"] == d]["mean_us"].values[0]
                    if not sub[sub["distribution"] == d].empty else 0 for d in dists]
            lo   = [sub[sub["distribution"] == d]["min_us"].values[0]
                    if not sub[sub["distribution"] == d].empty else 0 for d in dists]
            hi   = [sub[sub["distribution"] == d]["max_us"].values[0]
                    if not sub[sub["distribution"] == d].empty else 0 for d in dists]
            yerr = [np.array(vals) - np.array(lo), np.array(hi) - np.array(vals)]
            offset = (i - len(algos) / 2 + 0.5) * width
            ax.bar(x + offset, vals, width, label=ALGO[al], color=AC[al],
                   yerr=yerr, capsize=4, error_kw={"elinewidth": 1, "ecolor": "black"})
        ax.set_xticks(x); ax.set_xticklabels(dist_labels)
        ax.set(xlabel="rozkład", ylabel="czas [µs]",
               title=f"B – {STRUCT[st]} – algorytmy vs rozkład")
        ax.legend(); ax.grid(alpha=0.3, axis="y")
        save(f"B_03_{STRUCT[st]}_algos_vs_dist.png")


# C  
def analyze_C():
    print("\n[C] Wpływ typu danych")
    df = load("C_datatype.csv")
    g  = stats(df, ["algorithm", "structure", "dataType"])

    algos        = sorted(g["algorithm"].unique())
    dtypes       = sorted(g["dataType"].unique())
    dtype_labels = [DTYPE[d] for d in dtypes]

    # 1  per algorytm 
    for al in algos:
        sub = g[g["algorithm"] == al].sort_values("dataType")
        fig, ax = plt.subplots(figsize=(7, 4))
        colors = [PALETTE[i % len(PALETTE)] for i in range(len(sub))]
        ax.bar([DTYPE[d] for d in sub["dataType"]], sub["mean_us"],
               yerr=err_bars(sub), capsize=5,
               error_kw={"elinewidth": 1.2, "ecolor": "black"},
               color=colors)
        ax.set(xlabel="typ danych", ylabel="czas [µs]", title=f"C – {ALGO[al]}")
        ax.grid(alpha=0.3, axis="y")
        save(f"C_01_{ALGO[al]}.png")

    # 2  wszystkie algorytmy –
    width = 0.25
    x     = np.arange(len(dtypes))
    fig, ax = plt.subplots(figsize=(10, 5))
    for i, al in enumerate(algos):
        sub  = g[g["algorithm"] == al].sort_values("dataType")
        vals = [sub[sub["dataType"] == d]["mean_us"].values[0]
                if not sub[sub["dataType"] == d].empty else 0 for d in dtypes]
        lo   = [sub[sub["dataType"] == d]["min_us"].values[0]
                if not sub[sub["dataType"] == d].empty else 0 for d in dtypes]
        hi   = [sub[sub["dataType"] == d]["max_us"].values[0]
                if not sub[sub["dataType"] == d].empty else 0 for d in dtypes]
        yerr = [np.array(vals) - np.array(lo), np.array(hi) - np.array(vals)]
        offset = (i - len(algos) / 2 + 0.5) * width
        ax.bar(x + offset, vals, width, label=ALGO[al], color=AC[al],
               yerr=yerr, capsize=4, error_kw={"elinewidth": 1, "ecolor": "black"})
    ax.set_xticks(x); ax.set_xticklabels(dtype_labels)
    ax.set(xlabel="typ danych", ylabel="czas [µs]",
           title="C – wszystkie algorytmy vs typ danych")
    ax.legend(); ax.grid(alpha=0.3, axis="y")
    save("C_02_all_algos.png")


# Ω 
def analyze_Omega():
    print("\n[Ω] Porównanie struktur")
    df = load("Omega_structures.csv")
    g  = stats(df, ["algorithm", "structure"])

    algos         = sorted(g["algorithm"].unique())
    structs       = sorted(g["structure"].unique())
    struct_labels = [STRUCT[s] for s in structs]

    # 2  per algorytm 
    for al in algos:
        sub = g[g["algorithm"] == al].sort_values("structure")
        fig, ax = plt.subplots(figsize=(7, 4))
        ax.bar([STRUCT[s] for s in sub["structure"]], sub["mean_us"],
               yerr=err_bars(sub), capsize=5,
               error_kw={"elinewidth": 1.2, "ecolor": "black"},
               color=[SC[s] for s in sub["structure"]])
        ax.set_yscale("log")
        ax.set(xlabel="struktura", ylabel="czas [µs] (log)", title=f"Ω – {ALGO[al]} (log)")
        ax.grid(alpha=0.3, axis="y", which="both")
        save(f"Omega_02_{ALGO[al]}_log.png")

    # 4  wszystkie algorytmy –
    width = 0.25
    x     = np.arange(len(structs))
    fig, ax = plt.subplots(figsize=(10, 5))
    for i, al in enumerate(algos):
        sub  = g[g["algorithm"] == al].sort_values("structure")
        vals = [sub[sub["structure"] == s]["mean_us"].values[0]
                if not sub[sub["structure"] == s].empty else 0 for s in structs]
        lo   = [sub[sub["structure"] == s]["min_us"].values[0]
                if not sub[sub["structure"] == s].empty else 0 for s in structs]
        hi   = [sub[sub["structure"] == s]["max_us"].values[0]
                if not sub[sub["structure"] == s].empty else 0 for s in structs]
        yerr = [np.array(vals) - np.array(lo), np.array(hi) - np.array(vals)]
        offset = (i - len(algos) / 2 + 0.5) * width
        ax.bar(x + offset, vals, width, label=ALGO[al], color=AC[al],
               yerr=yerr, capsize=4, error_kw={"elinewidth": 1, "ecolor": "black"})
    ax.set_xticks(x); ax.set_xticklabels(struct_labels)
    ax.set_yscale("log")
    ax.set(xlabel="struktura", ylabel="czas [µs] (log)", title="Ω – algorytmy vs struktury (log)")
    ax.legend(); ax.grid(alpha=0.3, axis="y", which="both")
    save("Omega_04_all_algos_log.png")


if __name__ == "__main__":
    print(f"Wykresy zapisywane do: {PLOTS}")
    analyze_A()
    analyze_B()
    analyze_C()
    analyze_Omega()

    n = len([f for f in os.listdir(PLOTS) if f.endswith(".png")])
    print(f"\nGotowe!  {n} wykresów w {PLOTS}")
