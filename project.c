/*
 * CPS188 Term Project - Winter 2024
 * GlobalTemperatures Analysis
 *
 * This program reads the GlobalTemperatures.csv file and answers all 11 project questions using C and GNUPlot.
 *
 * Columns in CSV:
 *   0: dt (YYYY-MM-DD)
 *   1: LandAverageTemperature
 *   2: LandAverageTemperatureUncertainty
 *   3: LandMaxTemperature
 *   4: LandMaxTemperatureUncertainty
 *   5: LandMinTemperature
 *   6: LandMinTemperatureUncertainty
 *   7: LandAndOceanAverageTemperature
 *   8: LandAndOceanAverageTemperatureUncertainty
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ------------------------------------------------------------------ */
/*  Data structures                                                     */
/* ------------------------------------------------------------------ */

#define MAX_ROWS 3200

typedef struct {
    int  year;
    int  month;
    double landAvg;
    double landUncertainty;
    double landMax;
    double landMaxUnc;
    double landMin;
    double landMinUnc;
    double landOceanAvg;
    double landOceanUnc;
    int  hasLandAvg;      /* 1 if landAvg field is present, 0 if empty */
    int  hasLandMax;
    int  hasLandMin;
    int  hasLandOcean;
} Record;

Record data[MAX_ROWS];
int    nRecords = 0;

/* ------------------------------------------------------------------ */
/*  Helper: parse a possibly-empty double field                        */
/* ------------------------------------------------------------------ */
static int parseDouble(const char *s, double *out) {
    if (s == NULL || s[0] == '\0') return 0;
    char *end;
    *out = strtod(s, &end);
    return (end != s);  /* 1 = success */
}

/* ------------------------------------------------------------------ */
/*  Load CSV                                                            */
/* ------------------------------------------------------------------ */
void loadCSV(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "ERROR: Cannot open %s\n", filename);
        exit(1);
    }

    char line[1024];
    /* skip header */
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        /* Remove trailing newline / carriage return */
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) < 5) continue;

        /* Tokenise by comma */
        char *fields[9];
        int   nf = 0;
        char  buf[1024];
        strncpy(buf, line, sizeof(buf)-1);
        char *tok = strtok(buf, ",");
        while (tok && nf < 9) {
            fields[nf++] = tok;
            tok = strtok(NULL, ",");
        }
        while (nf < 9) fields[nf++] = "";

        /* Parse date: YYYY-MM-DD */
        int year, month;
        if (sscanf(fields[0], "%d-%d", &year, &month) != 2) continue;

        Record r;
        r.year  = year;
        r.month = month;
        r.hasLandAvg   = parseDouble(fields[1], &r.landAvg);
        r.hasLandMax   = parseDouble(fields[3], &r.landMax);
        r.hasLandMin   = parseDouble(fields[5], &r.landMin);
        r.hasLandOcean = parseDouble(fields[7], &r.landOceanAvg);

        if (!parseDouble(fields[2], &r.landUncertainty))  r.landUncertainty = 0.0;
        if (!parseDouble(fields[4], &r.landMaxUnc))       r.landMaxUnc      = 0.0;
        if (!parseDouble(fields[6], &r.landMinUnc))       r.landMinUnc      = 0.0;
        if (!parseDouble(fields[8], &r.landOceanUnc))     r.landOceanUnc    = 0.0;

        data[nRecords++] = r;
        if (nRecords >= MAX_ROWS) break;
    }
    fclose(fp);
    printf("Loaded %d records from %s\n\n", nRecords, filename);
}

/* ================================================================== */
/*  QUESTION 1                                                          */
/*  Yearly averages of LandAverageTemperature, years 1760-2015         */
/* ================================================================== */
void question1(void) {
    printf("=======================================================\n");
    printf("QUESTION 1: Yearly averages (1760-2015)\n");
    printf("=======================================================\n");

    for (int y = 1760; y <= 2015; y++) {
        double sum = 0.0;
        int    cnt = 0;
        for (int i = 0; i < nRecords; i++) {
            if (data[i].year == y && data[i].hasLandAvg) {
                sum += data[i].landAvg;
                cnt++;
            }
        }
        if (cnt > 0)
            printf("Year %d : %.4f C  (%d months)\n", y, sum/cnt, cnt);
        else
            printf("Year %d : no data\n", y);
    }
    printf("\n");
}

/* ================================================================== */
/*  QUESTION 2                                                          */
/*  Average per century (18th: 1760-1799, 19th: 1800-1899,             */
/*                        20th: 1900-1999, 21st: 2000-2015)            */
/* ================================================================== */
void question2(void) {
    printf("=======================================================\n");
    printf("QUESTION 2: Average land temperature per century\n");
    printf("=======================================================\n");

    int ranges[4][2] = {{1760,1799},{1800,1899},{1900,1999},{2000,2015}};
    const char *labels[] = {"18th century (1760-1799)",
                            "19th century (1800-1899)",
                            "20th century (1900-1999)",
                            "21st century (2000-2015)"};

    for (int c = 0; c < 4; c++) {
        double sum = 0.0;
        int    cnt = 0;
        for (int i = 0; i < nRecords; i++) {
            if (data[i].year >= ranges[c][0] &&
                data[i].year <= ranges[c][1] &&
                data[i].hasLandAvg) {
                sum += data[i].landAvg;
                cnt++;
            }
        }
        if (cnt > 0)
            printf("%s : %.4f C\n", labels[c], sum/cnt);
        else
            printf("%s : no data\n", labels[c]);
    }
    printf("\n");
}

/* ================================================================== */
/*  QUESTION 3                                                          */
/*  Monthly averages across all years 1900-2015                        */
/* ================================================================== */
void question3(void) {
    printf("=======================================================\n");
    printf("QUESTION 3: Monthly averages (1900-2015)\n");
    printf("=======================================================\n");

    const char *monthNames[] = {"January","February","March","April",
                                "May","June","July","August",
                                "September","October","November","December"};

    for (int m = 1; m <= 12; m++) {
        double sum = 0.0;
        int    cnt = 0;
        for (int i = 0; i < nRecords; i++) {
            if (data[i].month == m &&
                data[i].year >= 1900 && data[i].year <= 2015 &&
                data[i].hasLandAvg) {
                sum += data[i].landAvg;
                cnt++;
            }
        }
        if (cnt > 0)
            printf("%-10s (month %2d): %.4f C\n", monthNames[m-1], m, sum/cnt);
        else
            printf("%-10s (month %2d): no data\n", monthNames[m-1], m);
    }
    printf("\n");
}

/* ================================================================== */
/*  QUESTION 4                                                          */
/*  Hottest and coldest individual month recorded                       */
/* ================================================================== */
void question4(void) {
    printf("=======================================================\n");
    printf("QUESTION 4: Hottest and coldest month recorded\n");
    printf("=======================================================\n");

    const char *monthNames[] = {"Jan","Feb","Mar","Apr","May","Jun",
                                "Jul","Aug","Sep","Oct","Nov","Dec"};

    double maxT = -9999, minT = 9999;
    int    maxYear = 0, maxMonth = 0;
    int    minYear = 0, minMonth = 0;

    for (int i = 0; i < nRecords; i++) {
        if (!data[i].hasLandAvg) continue;
        if (data[i].landAvg > maxT) {
            maxT     = data[i].landAvg;
            maxYear  = data[i].year;
            maxMonth = data[i].month;
        }
        if (data[i].landAvg < minT) {
            minT     = data[i].landAvg;
            minYear  = data[i].year;
            minMonth = data[i].month;
        }
    }
    printf("Hottest month : %s %d  (%.4f C)\n",
           monthNames[maxMonth-1], maxYear, maxT);
    printf("Coldest month : %s %d  (%.4f C)\n",
           monthNames[minMonth-1], minYear, minT);
    printf("\n");
}

/* ================================================================== */
/*  QUESTION 5                                                          */
/*  Hottest and coldest YEAR (from yearly averages computed in Q1)     */
/* ================================================================== */
void question5(void) {
    printf("=======================================================\n");
    printf("QUESTION 5: Hottest and coldest year (1760-2015)\n");
    printf("=======================================================\n");

    double maxAvg = -9999, minAvg = 9999;
    int    maxY = 0, minY = 0;

    for (int y = 1760; y <= 2015; y++) {
        double sum = 0.0;
        int    cnt = 0;
        for (int i = 0; i < nRecords; i++) {
            if (data[i].year == y && data[i].hasLandAvg) {
                sum += data[i].landAvg;
                cnt++;
            }
        }
        if (cnt == 0) continue;
        double avg = sum / cnt;
        if (avg > maxAvg) { maxAvg = avg; maxY = y; }
        if (avg < minAvg) { minAvg = avg; minY = y; }
    }
    printf("Hottest year : %d  (%.4f C average)\n", maxY, maxAvg);
    printf("Coldest year : %d  (%.4f C average)\n", minY, minAvg);
    printf("\n");
}

/* ================================================================== */
/*  QUESTION 6                                                          */
/*  Line plot: yearly LandAverageTemperature 1760-2015                 */
/* ================================================================== */
void question6(void) {
    printf("=======================================================\n");
    printf("QUESTION 6: GNUPlot - Yearly averages line plot\n");
    printf("=======================================================\n");

    /* Write data file */
    FILE *df = fopen("q6_data.dat", "w");
    fprintf(df, "# Year  LandAvgTemp\n");
    for (int y = 1760; y <= 2015; y++) {
        double sum = 0.0; int cnt = 0;
        for (int i = 0; i < nRecords; i++) {
            if (data[i].year == y && data[i].hasLandAvg) {
                sum += data[i].landAvg; cnt++;
            }
        }
        if (cnt > 0)
            fprintf(df, "%d  %.4f\n", y, sum/cnt);
    }
    fclose(df);

    /* Write GNUPlot script */
    FILE *gp = fopen("q6_plot.gnu", "w");
    fprintf(gp, "# GNUPlot script for Question 6\n");
    fprintf(gp, "set terminal pngcairo size 1000,600 enhanced font 'Arial,12'\n");
    fprintf(gp, "set output 'q6_yearly_avg.png'\n");
    fprintf(gp, "set title 'Yearly Average Land Temperature (1760-2015)' font 'Arial,14'\n");
    fprintf(gp, "set xlabel 'Year'\n");
    fprintf(gp, "set ylabel 'Average Temperature (°C)'\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set key top left\n");
    fprintf(gp, "plot 'q6_data.dat' using 1:2 with lines lw 2 lc rgb 'red' title 'Land Avg Temp'\n");
    fclose(gp);

    printf("Data file : q6_data.dat\n");
    printf("Script    : q6_plot.gnu\n");
    printf("Run: gnuplot q6_plot.gnu\n\n");
}

/* ================================================================== */
/*  QUESTION 7                                                          */
/*  Line plots: 19th and 20th century monthly data on same axes        */
/*  (same x-scale: year-within-century, i.e. 0-99)                    */
/* ================================================================== */
void question7(void) {
    printf("=======================================================\n");
    printf("QUESTION 7: GNUPlot - 19th vs 20th century line plots\n");
    printf("=======================================================\n");

    /* Build yearly averages for 19th (1800-1899) and 20th (1900-1999) */
    FILE *df = fopen("q7_data.dat", "w");
    fprintf(df, "# RelYear  19thCentury  20thCentury\n");
    for (int rel = 0; rel <= 99; rel++) {
        int y19 = 1800 + rel;
        int y20 = 1900 + rel;

        double sum19 = 0; int cnt19 = 0;
        double sum20 = 0; int cnt20 = 0;
        for (int i = 0; i < nRecords; i++) {
            if (data[i].year == y19 && data[i].hasLandAvg) { sum19 += data[i].landAvg; cnt19++; }
            if (data[i].year == y20 && data[i].hasLandAvg) { sum20 += data[i].landAvg; cnt20++; }
        }
        double avg19 = (cnt19 > 0) ? sum19/cnt19 : 0.0/0.0; /* NaN if missing */
        double avg20 = (cnt20 > 0) ? sum20/cnt20 : 0.0/0.0;

        if (cnt19 > 0 || cnt20 > 0)
            fprintf(df, "%d  %s  %s\n", rel,
                    cnt19 > 0 ? (sprintf((char[32]){}, "%.4f", avg19), (char[32]){}) : "?",
                    cnt20 > 0 ? (sprintf((char[32]){}, "%.4f", avg20), (char[32]){}) : "?");
    }
    fclose(df);

    /* Rewrite data file properly (simpler approach) */
    df = fopen("q7_data.dat", "w");
    fprintf(df, "# RelYear  19thCentury  20thCentury\n");
    for (int rel = 0; rel <= 99; rel++) {
        int y19 = 1800 + rel;
        int y20 = 1900 + rel;
        double sum19 = 0; int cnt19 = 0;
        double sum20 = 0; int cnt20 = 0;
        for (int i = 0; i < nRecords; i++) {
            if (data[i].year == y19 && data[i].hasLandAvg) { sum19 += data[i].landAvg; cnt19++; }
            if (data[i].year == y20 && data[i].hasLandAvg) { sum20 += data[i].landAvg; cnt20++; }
        }
        fprintf(df, "%d", rel);
        if (cnt19 > 0) fprintf(df, "  %.4f", sum19/cnt19);
        else            fprintf(df, "  NaN");
        if (cnt20 > 0) fprintf(df, "  %.4f", sum20/cnt20);
        else            fprintf(df, "  NaN");
        fprintf(df, "\n");
    }
    fclose(df);

    FILE *gp = fopen("q7_plot.gnu", "w");
    fprintf(gp, "# GNUPlot script for Question 7\n");
    fprintf(gp, "set terminal pngcairo size 1000,600 enhanced font 'Arial,12'\n");
    fprintf(gp, "set output 'q7_century_comparison.png'\n");
    fprintf(gp, "set title '19th vs 20th Century Land Average Temperatures' font 'Arial,14'\n");
    fprintf(gp, "set xlabel 'Year within Century'\n");
    fprintf(gp, "set ylabel 'Average Temperature (°C)'\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set key top left\n");
    fprintf(gp, "plot 'q7_data.dat' using 1:2 with lines lw 2 lc rgb 'blue'  title '19th Century (1800-1899)', \\\n");
    fprintf(gp, "     'q7_data.dat' using 1:3 with lines lw 2 lc rgb 'red'   title '20th Century (1900-1999)'\n");
    fclose(gp);

    printf("Data file : q7_data.dat\n");
    printf("Script    : q7_plot.gnu\n");
    printf("Run: gnuplot q7_plot.gnu\n\n");
}

/* ================================================================== */
/*  QUESTION 8                                                          */
/*  Line plots: LandAvg, LandMax, LandMin yearly avgs 1850-2015        */
/* ================================================================== */
void question8(void) {
    printf("=======================================================\n");
    printf("QUESTION 8: GNUPlot - Avg/Max/Min temperatures 1850-2015\n");
    printf("=======================================================\n");

    FILE *df = fopen("q8_data.dat", "w");
    fprintf(df, "# Year  LandAvg  LandMax  LandMin\n");
    for (int y = 1850; y <= 2015; y++) {
        double sumA=0,sumX=0,sumN=0;
        int cntA=0,cntX=0,cntN=0;
        for (int i = 0; i < nRecords; i++) {
            if (data[i].year != y) continue;
            if (data[i].hasLandAvg) { sumA += data[i].landAvg; cntA++; }
            if (data[i].hasLandMax) { sumX += data[i].landMax; cntX++; }
            if (data[i].hasLandMin) { sumN += data[i].landMin; cntN++; }
        }
        fprintf(df, "%d", y);
        fprintf(df, "  %s", cntA>0 ? (char[32]){0} : "NaN");
        if (cntA>0) { char t[32]; sprintf(t,"%.4f",sumA/cntA); fprintf(df, "  %s", t); }
        else         fprintf(df, "  NaN");
        if (cntX>0) { char t[32]; sprintf(t,"%.4f",sumX/cntX); fprintf(df, "  %s", t); }
        else         fprintf(df, "  NaN");
        if (cntN>0) { char t[32]; sprintf(t,"%.4f",sumN/cntN); fprintf(df, "  %s", t); }
        else         fprintf(df, "  NaN");
        fprintf(df, "\n");
    }
    fclose(df);

    /* Rewrite cleanly */
    df = fopen("q8_data.dat", "w");
    fprintf(df, "# Year  LandAvg  LandMax  LandMin\n");
    for (int y = 1850; y <= 2015; y++) {
        double sumA=0,sumX=0,sumN=0;
        int cntA=0,cntX=0,cntN=0;
        for (int i = 0; i < nRecords; i++) {
            if (data[i].year != y) continue;
            if (data[i].hasLandAvg) { sumA += data[i].landAvg; cntA++; }
            if (data[i].hasLandMax) { sumX += data[i].landMax; cntX++; }
            if (data[i].hasLandMin) { sumN += data[i].landMin; cntN++; }
        }
        double a = cntA>0 ? sumA/cntA : NAN;
        double x = cntX>0 ? sumX/cntX : NAN;
        double n = cntN>0 ? sumN/cntN : NAN;
        if (cntA>0 || cntX>0 || cntN>0) {
            fprintf(df, "%d", y);
            if (cntA>0) fprintf(df,"  %.4f",a); else fprintf(df,"  NaN");
            if (cntX>0) fprintf(df,"  %.4f",x); else fprintf(df,"  NaN");
            if (cntN>0) fprintf(df,"  %.4f",n); else fprintf(df,"  NaN");
            fprintf(df,"\n");
        }
    }
    fclose(df);

    FILE *gp = fopen("q8_plot.gnu", "w");
    fprintf(gp, "# GNUPlot script for Question 8\n");
    fprintf(gp, "set terminal pngcairo size 1000,600 enhanced font 'Arial,12'\n");
    fprintf(gp, "set output 'q8_avg_max_min.png'\n");
    fprintf(gp, "set title 'Land Temperature: Average, Max, and Min (1850-2015)' font 'Arial,14'\n");
    fprintf(gp, "set xlabel 'Year'\n");
    fprintf(gp, "set ylabel 'Temperature (°C)'\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set key top left\n");
    fprintf(gp, "plot 'q8_data.dat' using 1:2 with lines lw 3 lc rgb 'red'   title 'Land Avg Temp', \\\n");
    fprintf(gp, "     'q8_data.dat' using 1:3 with lines lw 1 lc rgb 'orange' dt 2 title 'Land Max Temp', \\\n");
    fprintf(gp, "     'q8_data.dat' using 1:4 with lines lw 1 lc rgb 'blue'  dt 2 title 'Land Min Temp'\n");
    fclose(gp);

    printf("Data file : q8_data.dat\n");
    printf("Script    : q8_plot.gnu\n");
    printf("Run: gnuplot q8_plot.gnu\n\n");
}

/* ================================================================== */
/*  QUESTION 9                                                          */
/*  Bar (histogram) subplots: avg/max/min by century                   */
/*  Centuries shown: after-1850 of 19th (1851-1899),                   */
/*                   20th (1900-1999), 21st (2000-2015)                */
/* ================================================================== */
void question9(void) {
    printf("=======================================================\n");
    printf("QUESTION 9: GNUPlot - Bar plots Avg/Max/Min per century\n");
    printf("=======================================================\n");

    int ranges[3][2] = {{1851,1899},{1900,1999},{2000,2015}};
    const char *cenLabels[] = {"19th (1851-1899)","20th (1900-1999)","21st (2000-2015)"};

    FILE *df = fopen("q9_data.dat", "w");
    fprintf(df, "# Century  AvgTemp  MaxTemp  MinTemp\n");
    for (int c = 0; c < 3; c++) {
        double sumA=0,sumX=0,sumN=0;
        int cntA=0,cntX=0,cntN=0;
        for (int i = 0; i < nRecords; i++) {
            if (data[i].year < ranges[c][0] || data[i].year > ranges[c][1]) continue;
            if (data[i].hasLandAvg) { sumA += data[i].landAvg; cntA++; }
            if (data[i].hasLandMax) { sumX += data[i].landMax; cntX++; }
            if (data[i].hasLandMin) { sumN += data[i].landMin; cntN++; }
        }
        fprintf(df, "\"%s\"  %.4f  %.4f  %.4f\n",
                cenLabels[c],
                cntA>0 ? sumA/cntA : 0.0,
                cntX>0 ? sumX/cntX : 0.0,
                cntN>0 ? sumN/cntN : 0.0);
    }
    fclose(df);

    FILE *gp = fopen("q9_plot.gnu", "w");
    fprintf(gp, "# GNUPlot script for Question 9\n");
    fprintf(gp, "set terminal pngcairo size 1200,900 enhanced font 'Arial,12'\n");
    fprintf(gp, "set output 'q9_bar_plots.png'\n");
    fprintf(gp, "set multiplot layout 1,3 title 'Average, Max, and Min Temperatures by Century' font 'Arial,14'\n");
    fprintf(gp, "set style data histogram\n");
    fprintf(gp, "set style fill solid 1.0 border -1\n");
    fprintf(gp, "set boxwidth 0.6\n");
    fprintf(gp, "set xtic rotate by -30 scale 0 font 'Arial,10'\n");
    fprintf(gp, "set ylabel 'Temperature (°C)'\n");
    fprintf(gp, "set yrange [0:*]\n");
    /* Subplot 1: Average */
    fprintf(gp, "set title 'Average Land Temperature'\n");
    fprintf(gp, "plot 'q9_data.dat' using 2:xtic(1) lc rgb 'steelblue'  title 'Average'\n");
    /* Subplot 2: Max */
    fprintf(gp, "set title 'Max Land Temperature'\n");
    fprintf(gp, "plot 'q9_data.dat' using 3:xtic(1) lc rgb 'red'        title 'Max'\n");
    /* Subplot 3: Min */
    fprintf(gp, "set title 'Min Land Temperature'\n");
    fprintf(gp, "plot 'q9_data.dat' using 4:xtic(1) lc rgb 'navy'   title 'Min'\n");
    fprintf(gp, "unset multiplot\n");
    fclose(gp);

    printf("Data file : q9_data.dat\n");
    printf("Script    : q9_plot.gnu\n");
    printf("Run: gnuplot q9_plot.gnu\n\n");
}

/* ================================================================== */
/*  QUESTION 10                                                         */
/*  Error bar plot: monthly avg land temp 2000-2015 with uncertainty   */
/* ================================================================== */
void question10(void) {
    printf("=======================================================\n");
    printf("QUESTION 10: GNUPlot - Error bar plot (2000-2015)\n");
    printf("=======================================================\n");

    FILE *df = fopen("q10_data.dat", "w");
    fprintf(df, "# Month  AvgTemp  AvgUncertainty\n");

    const char *monthNames[] = {"Jan","Feb","Mar","Apr","May","Jun",
                                "Jul","Aug","Sep","Oct","Nov","Dec"};

    for (int m = 1; m <= 12; m++) {
        double sumT = 0, sumU = 0;
        int    cnt = 0;
        for (int i = 0; i < nRecords; i++) {
            if (data[i].month == m &&
                data[i].year >= 2000 && data[i].year <= 2015 &&
                data[i].hasLandAvg) {
                sumT += data[i].landAvg;
                sumU += data[i].landUncertainty;
                cnt++;
            }
        }
        if (cnt > 0)
            fprintf(df, "%d  %.4f  %.4f  # %s\n",
                    m, sumT/cnt, sumU/cnt, monthNames[m-1]);
    }
    fclose(df);

    FILE *gp = fopen("q10_plot.gnu", "w");
    fprintf(gp, "# GNUPlot script for Question 10\n");
    fprintf(gp, "set terminal pngcairo size 1000,600 enhanced font 'Arial,12'\n");
    fprintf(gp, "set output 'q10_errorbars.png'\n");
    fprintf(gp, "set title 'Average Monthly Land Temperature with Error Bars (2000-2015)' font 'Arial,14'\n");
    fprintf(gp, "set xlabel 'Month'\n");
    fprintf(gp, "set ylabel 'Temperature (°C)'\n");
    fprintf(gp, "set xtics ('Jan' 1, 'Feb' 2, 'Mar' 3, 'Apr' 4, 'May' 5, 'Jun' 6, \\\n");
    fprintf(gp, "           'Jul' 7, 'Aug' 8, 'Sep' 9, 'Oct' 10, 'Nov' 11, 'Dec' 12)\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set key top right\n");
    fprintf(gp, "plot 'q10_data.dat' using 1:2:3 with yerrorbars lw 2 lc rgb 'dark-green' pt 7 title 'Avg Temp ± Uncertainty'\n");
    fclose(gp);

    printf("Data file : q10_data.dat\n");
    printf("Script    : q10_plot.gnu\n");
    printf("Run: gnuplot q10_plot.gnu\n\n");
}

/* ================================================================== */
/*  QUESTION 11                                                         */
/*  Line plot: LandAvgTemp + LandAndOceanAvgTemp 1850-2015             */
/* ================================================================== */
void question11(void) {
    printf("=======================================================\n");
    printf("QUESTION 11: GNUPlot - Land vs Land+Ocean 1850-2015\n");
    printf("=======================================================\n");

    FILE *df = fopen("q11_data.dat", "w");
    fprintf(df, "# Year  LandAvg  LandOceanAvg\n");
    for (int y = 1850; y <= 2015; y++) {
        double sumA=0, sumO=0;
        int cntA=0, cntO=0;
        for (int i = 0; i < nRecords; i++) {
            if (data[i].year != y) continue;
            if (data[i].hasLandAvg)   { sumA += data[i].landAvg;     cntA++; }
            if (data[i].hasLandOcean) { sumO += data[i].landOceanAvg; cntO++; }
        }
        fprintf(df, "%d", y);
        if (cntA>0) fprintf(df,"  %.4f",sumA/cntA); else fprintf(df,"  NaN");
        if (cntO>0) fprintf(df,"  %.4f",sumO/cntO); else fprintf(df,"  NaN");
        fprintf(df,"\n");
    }
    fclose(df);

    FILE *gp = fopen("q11_plot.gnu", "w");
    fprintf(gp, "# GNUPlot script for Question 11\n");
    fprintf(gp, "set terminal pngcairo size 1000,600 enhanced font 'Arial,12'\n");
    fprintf(gp, "set output 'q11_land_vs_ocean.png'\n");
    fprintf(gp, "set title 'Land vs Land+Ocean Average Temperature (1850-2015)' font 'Arial,14'\n");
    fprintf(gp, "set xlabel 'Year'\n");
    fprintf(gp, "set ylabel 'Average Temperature (°C)'\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set key top left\n");
    fprintf(gp, "plot 'q11_data.dat' using 1:2 with lines lw 2 lc rgb 'red'  title 'Land Average Temp', \\\n");
    fprintf(gp, "     'q11_data.dat' using 1:3 with lines lw 2 lc rgb 'blue' title 'Land+Ocean Average Temp'\n");
    fclose(gp);

    printf("Data file : q11_data.dat\n");
    printf("Script    : q11_plot.gnu\n");
    printf("Run: gnuplot q11_plot.gnu\n\n");
}

/* ================================================================== */
/*  MAIN                                                                */
/* ================================================================== */
int main(void) {
    /* Load data - adjust path as needed */
    loadCSV("GlobalTemperatures.csv");

    /* Answer all questions */
    question1();
    question2();
    question3();
    question4();
    question5();
    question6();
    question7();
    question8();
    question9();
    question10();
    question11();

    printf("=======================================================\n");
    printf("All questions answered. GNUPlot scripts generated.\n");
    printf("To generate all graphs, run:\n");
    printf("  gnuplot q6_plot.gnu\n");
    printf("  gnuplot q7_plot.gnu\n");
    printf("  gnuplot q8_plot.gnu\n");
    printf("  gnuplot q9_plot.gnu\n");
    printf("  gnuplot q10_plot.gnu\n");
    printf("  gnuplot q11_plot.gnu\n");
    printf("=======================================================\n");

    return 0;
}