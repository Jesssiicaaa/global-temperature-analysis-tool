# 🌡️ Global Temperature Analysis

<img width="1000" height="600" alt="image" src="https://github.com/user-attachments/assets/7ef9071f-1027-478c-9251-8f62b13c5e21" />
<img width="1000" height="600" alt="image" src="https://github.com/user-attachments/assets/fad66449-9343-459a-8cbd-d0906ff0fd54" />
<img width="1000" height="600" alt="image" src="https://github.com/user-attachments/assets/c51c00a8-f5a7-4c8c-935c-3f10b4932157" />
<img width="1200" height="900" alt="image" src="https://github.com/user-attachments/assets/f39f7ffc-719b-4529-9d23-8c20723473b1" />
<img width="1000" height="600" alt="image" src="https://github.com/user-attachments/assets/b68b4539-1c1c-4515-b0c8-4504068e1b9f" />
<img width="1000" height="600" alt="image" src="https://github.com/user-attachments/assets/f317cccc-840c-4b0e-8a53-c37f19bc81ea" />

> Analysis of global land and ocean temperatures from **1750 to 2015** using C and GNUPlot. Built as a term project for CPS188 at Toronto Metropolitan University (Winter 2024).

---

## 📁 Project Structure

```
CPS188_TermProject/
├── src/
│   └── project.c               # Main C program — all 11 questions
├── scripts/
│   ├── q6_plot.gnu             # Yearly avg line plot
│   ├── q7_plot.gnu             # 19th vs 20th century comparison
│   ├── q8_plot.gnu             # Avg / Max / Min line plot
│   ├── q9_plot.gnu             # Bar subplots by century
│   ├── q10_plot.gnu            # Error bar plot (2000–2015)
│   └── q11_plot.gnu            # Land vs Land+Ocean
├── data/
│   └── GlobalTemperatures.csv  # Source dataset (not tracked by git)
└── README.md
```

---

## 🚀 Getting Started

### Prerequisites

- GCC or any C99-compatible compiler
- [GNUPlot](http://www.gnuplot.info/) installed and on your `PATH`
- `GlobalTemperatures.csv` — place it inside `data/`

### Build & Run

```bash
# 1. Compile
gcc -o project src/project.c -lm

# 2. Run — prints Q1–Q5 results and generates all .dat and .gnu files
./project

# 3. Render graphs
gnuplot scripts/q6_plot.gnu
gnuplot scripts/q7_plot.gnu
gnuplot scripts/q8_plot.gnu
gnuplot scripts/q9_plot.gnu
gnuplot scripts/q10_plot.gnu
gnuplot scripts/q11_plot.gnu
```

---

## 📊 Questions & Outputs

| # | Description | Output |
|---|---|---|
| Q1 | Yearly averages — 1760 to 2015 | Terminal |
| Q2 | Average temperature per century | Terminal |
| Q3 | Monthly averages across 1900–2015 | Terminal |
| Q4 | Hottest and coldest month ever recorded | Terminal |
| Q5 | Hottest and coldest year | Terminal |
| Q6 | Line plot — yearly avg temperature | `q6_yearly_avg.png` |
| Q7 | Line plot — 19th vs 20th century | `q7_century_comparison.png` |
| Q8 | Line plot — avg / max / min (1850–2015) | `q8_avg_max_min.png` |
| Q9 | Bar subplots — avg / max / min by century | `q9_bar_plots.png` |
| Q10 | Error bar plot — monthly avg with uncertainty | `q10_errorbars.png` |
| Q11 | Line plot — land vs land+ocean (1850–2015) | `q11_land_vs_ocean.png` |

---

## 🔍 Key Findings

| | |
|---|---|
| 🔥 Hottest month | July 1761 — **19.02 °C** |
| 🧊 Coldest month | January 1768 — **−2.08 °C** |
| 📈 Hottest year | **2015** — 9.83 °C avg |
| 📉 Coldest year | **1768** — 6.78 °C avg |
| 18th century avg | 8.22 °C |
| 19th century avg | 8.01 °C |
| 20th century avg | 8.64 °C |
| 21st century avg (2000–2015) | **9.54 °C** |

---

## 🛠️ Built With

- **C** — data loading, parsing, and all computations
- **GNUPlot** — all graphs and visualisations

---

## 👥 Authors

- Jessica Olaniyi

---

## 📄 License

This project was submitted for academic purposes at TMU. Not intended for redistribution.
