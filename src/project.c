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

Record data[MAX_ROWS];// Array to hold all records, 3200 records is more than enough for 1760-2015 monthly data (3060 months)
int    nRecords = 0;

/* ------------------------------------------------------------------ */
/*  Helper: parse a possibly-empty double field                        */
/* ------------------------------------------------------------------ */
static int parseDouble(const char *s, double *out) {
    if (s == NULL || s[0] == '\0') return 0;
    char *end;
    *out = strtod(s, &end);//strtod: converts an initial portion of a string to a double-precision floating-point value
    return (end != s);  /* 1 = success */
}

/* ------------------------------------------------------------------ */
/*  Load CSV                                                            */
/* ------------------------------------------------------------------ */
void loadCSV(const char *filename) {
    FILE *fp = fopen(filename, "r");//fp: file pointer, fopen: opens the file specified by filename and returns a pointer to the file stream
    if (!fp) {//r: read mode, if the file cannot be opened, fopen returns NULL
        fprintf(stderr, "ERROR: Cannot open %s\n", filename);//error message printed to standard error stream if the file cannot be opened
        exit(1);
    }

    char line[1024];//line: buffer to hold each line read from the file, 1024 is an arbitrary size that should be sufficient for the lines in the CSV file
    /* skip header */
    fgets(line, sizeof(line), fp);//gets: reads a line from the specified stream and stores it in the string pointed to by line,
    //up to a maximum of sizeof(line) - 1 characters. This is used to skip the header line of the CSV file.

    while (fgets(line, sizeof(line), fp)) {
        /* Remove trailing newline / carriage return */
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) < 5) continue;

/* Tokenise by comma. a process of splitting a string of text into smaller units (tokens) whenever a comma (,) character is encountered */
//This technique is commonly used to parse comma-separated values (CSV), user-generated tags, or lists within a document
        char *fields[9];// fields: array of pointers to hold the 9 fields from the CSV line, after tokenization by comma
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

    int ranges[4][2] = {{1760,1799},{1800,1899},{1900,1999},{2000,2015}};//2d array to hold the year ranges for each century, where each row corresponds to a century and contains the start and end years of that century
    const char *labels[] = {"18th century (1760-1799)",
                            "19th century (1800-1899)",
                            "20th century (1900-1999)",
                            "21st century (2000-2015)"};

    for (int c = 0; c < 4; c++) {//loop iterates over the 4 centuries defined in the ranges array, where c is the index of the current century being processed
        double sum = 0.0;
        int    cnt = 0;
        for (int i = 0; i < nRecords; i++) {//loop iterates over all the records in the data array, where i is the index of the current record being processed
            if (data[i].year >= ranges[c][0] &&//condition checks if the year of the current record falls within the start and end years of the current century being processed, and also checks if the landAvg field is present for that record
                data[i].year <= ranges[c][1] &&//condition checks if the year of the current record falls within the start and end years of the current century being processed, and also checks if the landAvg field is present for that record
                data[i].hasLandAvg) {
                sum += data[i].landAvg;//if the conditions are met, the landAvg value of the current record is added to the sum variable, which accumulates the total land average temperature for that century
                cnt++;//if the conditions are met, the cnt variable is incremented by 1, which counts the number of records that contribute to the average land temperature for that century
            }
        }
        if (cnt > 0)//if there are records that contribute to the average land temperature for that century (cnt > 0), the average is calculated by dividing the sum of land average temperatures by the count of records
            printf("%s : %.4f C\n", labels[c], sum/cnt);//the result is printed along with the corresponding label for that century
        else
            printf("%s : no data\n", labels[c]);//if there are no records that contribute to the average land temperature for that century (cnt == 0)
    }// a message indicating "no data" is printed along with the corresponding label for that century
    printf("\n");//newline is printed after processing all centuries to separate the output of this question from subsequent questions
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

    for (int m = 1; m <= 12; m++) {//loop iterates over the 12 months of the year, where m is the index of the current month being processed (1 for January, 2 for February, etc.)
        double sum = 0.0;
        int    cnt = 0;
        for (int i = 0; i < nRecords; i++) {//loop iterates over all the records in the data array, where i is the index of the current record being processed
            if (data[i].month == m &&//condition checks if the month of the current record matches the current month being processed
                data[i].year >= 1900 && data[i].year <= 2015 &&// condition checksif the year of the current record falls within the range of 1900 to 2015
                data[i].hasLandAvg) {//condition checks the landAvg field is present for that record
                sum += data[i].landAvg;
                cnt++;
            }
        }
        if (cnt > 0)//if there are records that contribute to the average land temperature for that month (cnt > 0)
            printf("%-10s (month %2d): %.4f C\n", monthNames[m-1], m, sum/cnt);//the average is calculated by dividing the sum of land average temperatures by the count of records, and the result is printed along with the corresponding month name and month number
        else
            printf("%-10s (month %2d): no data\n", monthNames[m-1], m);//if there are no records that contribute to the average land temperature for that month (cnt == 0)
    }//a message indicating "no data" is printed along with the corresponding month name and month number
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

    double maxT = -9999, minT = 9999;//maxT and minT are initialized to extreme values to ensure that any valid temperature from the dataset will be correctly identified as the hottest or coldest month when compared against these initial values
    int    maxYear = 0, maxMonth = 0;
    int    minYear = 0, minMonth = 0;

    for (int i = 0; i < nRecords; i++) {//loop iterates over all the records in the data array, where i is the index of the current record being processed
        if (!data[i].hasLandAvg) continue;//condition checks if the landAvg field is present for the current record, and if it is not present,  the loop continues to the next iteration without executing the subsequent code for that record
        if (data[i].landAvg > maxT) {//condition checks if the landAvg value of the current record is greater than the current maximum temperature (maxT)
            maxT     = data[i].landAvg;//if the condition is true, maxT is updated to the landAvg value of the current record
            maxMonth = data[i].month;//if the condition is true, maxMonth is updated to the month of the current record
        }
        if (data[i].landAvg < minT) {
            minT     = data[i].landAvg;//if the condition is true, minT is updated to the landAvg value of the current record
            minYear  = data[i].year;//if the condition is true, minYear is updated to the year of the current record
            minMonth = data[i].month;//if the condition is true, minMonth is updated to the month of the current record
        }
    }
    printf("Hottest month : %s %d  (%.4f C)\n",//the hottest month is printed along with the corresponding month name, year, and temperature value.
           monthNames[maxMonth-1], maxYear, maxT);// The month name is retrieved from the monthNames array using maxMonth-1 as the index (since month numbers are 1-based and array indices are 0-based)
    printf("Coldest month : %s %d  (%.4f C)\n",//the coldest month is printed along with the corresponding month name, year, and temperature value. 
           monthNames[minMonth-1], minYear, minT);//The month name is retrieved from the monthNames array using minMonth-1 as the index (since month numbers are 1-based and array indices are 0-based)
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

    for (int y = 1760; y <= 2015; y++) {//loop iterates over the years from 1760 to 2015, where y is the current year being processed
        double sum = 0.0;
        int    cnt = 0;
        for (int i = 0; i < nRecords; i++) {//loop iterates over all the records in the data array, where i is the index of the current record being processed
            if (data[i].year == y && data[i].hasLandAvg) {//condition checks if the year of the current record matches the current year being processed and if the landAvg field is present for that record
                sum += data[i].landAvg;//the landAvg value of the current record is added to the sum variable, which accumulates the total land average temperature for that year
                cnt++;//the cnt variable is incremented by 1, which counts the number of records that contribute to the average land temperature for that year
            }
        }
        if (cnt == 0) continue;//if there are no records that contribute to the average land temperature for that year (cnt == 0), the loop continues to the next iteration without executing the subsequent code for that year
        double avg = sum / cnt;//if cnt > 0, the average is calculated by dividing the sum of land average temperatures by the count of records
        if (avg > maxAvg) { maxAvg = avg; maxY = y; }//condition checks if the average land temperature for the current year (avg) is greater than the current maximum average temperature (maxAvg), and if it is, maxAvg is updated to avg and maxY is updated to the current year (y)
        if (avg < minAvg) { minAvg = avg; minY = y; }//condition checks if the average land temperature for the current year (avg) is less than the current minimum average temperature (minAvg), and if it is, minAvg is updated to avg and minY is updated to the current year (y)
    }
    printf("Hottest year : %d  (%.4f C average)\n", maxY, maxAvg);//the hottest year is printed along with the corresponding average temperature value
    printf("Coldest year : %d  (%.4f C average)\n", minY, minAvg);//the coldest year is printed along with the corresponding average temperature value
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
    FILE *df = fopen("data/q6_data.dat", "w");//df: file pointer for the data file, fopen: opens the file specified by "data/q6_data.dat" in write mode ("w"), which 
//means that if the file already exists, it will be truncated to zero length and the program will start writing from the beginning of the file. If the file does not exist, it will be created.
    fprintf(df, "# Year  LandAvgTemp\n");//the first line written to the data file is a comment line that serves as a header, indicating that the first column contains the year and
//the second column contains the average land temperature for that year. The "#" character at the beginning of the line denotes that it is a comment in GNUPlot data files
//which helps to clarify the structure of the data for anyone who reads the file or uses it for plotting.
    for (int y = 1760; y <= 2015; y++) {//loop iterates over the years from 1760 to 2015, where y is the current year being processed
        double sum = 0.0; int cnt = 0;
        for (int i = 0; i < nRecords; i++) {//loop iterates over all the records in the data array, where i is the index of the current record being processed
            if (data[i].year == y && data[i].hasLandAvg) {//condition checks if the year of the current record matches the current year being processed and if the landAvg field is present for that record
                sum += data[i].landAvg;//the landAvg value of the current record is added to the sum variable, which accumulates the total land average temperature for that year, and the cnt variable is incremented by 1, which counts the number of records that contribute to the average land temperature for that year
                cnt++;// the cnt variable is incremented by 1, which counts the number of records that contribute to the average land temperature for that year
            }
        }
        if (cnt > 0)//if cnt > 0, the average is calculated by dividing the sum of land average temperatures by the count of records, and the result is written to the data file
            fprintf(df, "%d  %.4f\n", y, sum/cnt);
    }
    fclose(df);

    /* Write GNUPlot script */
    FILE *gp = fopen("scripts/q6_plot.gnu", "w");//gp: file pointer for the GNUPlot script, fopen: opens the file specified by "scripts/q6_plot.gnu" in write mode ("w")
    //which means that if the file already exists, it will be truncated to zero length and the program will start writing from the beginning of the file.
    //If the file does not exist, it will be created.
    fprintf(gp, "# GNUPlot script for Question 6\n");
    fprintf(gp, "set terminal pngcairo size 1000,600 enhanced font 'Arial,12'\n");
    fprintf(gp, "set output 'report/q6_yearly_avg.png'\n");
    fprintf(gp, "set title 'Yearly Average Land Temperature (1760-2015)' font 'Arial,14'\n");
    fprintf(gp, "set xlabel 'Year'\n");
    fprintf(gp, "set ylabel 'Average Temperature (°C)'\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set key top left\n");
    fprintf(gp, "plot 'data/q6_data.dat' using 1:2 with lines lw 2 lc rgb 'red' title 'Land Avg Temp'\n");
    fclose(gp);

    printf("Data file : data/q6_data.dat\n");
    printf("Script    : scripts/q6_plot.gnu\n");
    printf("Run: gnuplot scripts/q6_plot.gnu\n\n");
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
    FILE *df = fopen("data/q7_data.dat", "w");//df: file pointer for the data file, fopen: opens the file specified by "data/q7_data.dat" in write mode ("w")
    //which means that if the file already exists, it will be truncated to zero length and the program will start writing from the beginning of the file.
    //If the file does not exist, it will be created.
    fprintf(df, "# RelYear  19thCentury  20thCentury\n");
    for (int rel = 0; rel <= 99; rel++) {// loop iterates over the relative years from 0 to 99, where rel is the current relative year being processed.
        //This loop is used to calculate the average land temperatures for each year within the 19th and 20th centuries, where the relative year (rel) corresponds
        //to the year within the century (e.g., rel=0 corresponds to 1800 for the 19th century and 1900 for the 20th century)
        int y19 = 1800 + rel;//y19 is calculated by adding the relative year (rel) to the base year of 1800, which gives the actual year in the 19th century that corresponds to the current relative year being processed
        int y20 = 1900 + rel;//y20 is calculated by adding the relative year (rel) to the base year of 1900, which gives the actual year in the 20th century that corresponds to the current relative year being processed

        double sum19 = 0; int cnt19 = 0;
        double sum20 = 0; int cnt20 = 0;
        for (int i = 0; i < nRecords; i++) {//loop iterates over all the records in the data array, where i is the index of the current record being processed
            if (data[i].year == y19 && data[i].hasLandAvg) { sum19 += data[i].landAvg; cnt19++; }//condition checks if the year of the current record matches the calculated year for the 19th century (y19) and if the 
            //landAvg field is present for that record, and if both conditions are true, the landAvg value of that record is added to sum19 and cnt19 is incremented by 1
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
    df = fopen("data/q7_data.dat", "w");
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

    FILE *gp = fopen("scripts/q7_plot.gnu", "w");
    fprintf(gp, "# GNUPlot script for Question 7\n");
    fprintf(gp, "set terminal pngcairo size 1000,600 enhanced font 'Arial,12'\n");
    fprintf(gp, "set output 'report/q7_century_comparison.png'\n");
    fprintf(gp, "set title '19th vs 20th Century Land Average Temperatures' font 'Arial,14'\n");
    fprintf(gp, "set xlabel 'Year within Century'\n");
    fprintf(gp, "set ylabel 'Average Temperature (°C)'\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set key top left\n");
    fprintf(gp, "plot 'data/q7_data.dat' using 1:2 with lines lw 2 lc rgb 'blue'  title '19th Century (1800-1899)', \\\n");
    fprintf(gp, "     'data/q7_data.dat' using 1:3 with lines lw 2 lc rgb 'red'   title '20th Century (1900-1999)'\n");
    fclose(gp);

    printf("Data file : data/q7_data.dat\n");
    printf("Script    : scripts/q7_plot.gnu\n");
    printf("Run: gnuplot scripts/q7_plot.gnu\n\n");
}

/* ================================================================== */
/*  QUESTION 8                                                          */
/*  Line plots: LandAvg, LandMax, LandMin yearly avgs 1850-2015        */
/* ================================================================== */
void question8(void) {
    printf("=======================================================\n");
    printf("QUESTION 8: GNUPlot - Avg/Max/Min temperatures 1850-2015\n");
    printf("=======================================================\n");

    FILE *df = fopen("data/q8_data.dat", "w");
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
    df = fopen("data/q8_data.dat", "w");
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

    FILE *gp = fopen("scripts/q8_plot.gnu", "w");//gp: file pointer for the GNUPlot script, fopen: opens the file specified by "scripts/q8_plot.gnu" in write mode ("w")
    fprintf(gp, "# GNUPlot script for Question 8\n");
    fprintf(gp, "set terminal pngcairo size 1000,600 enhanced font 'Arial,12'\n");
    fprintf(gp, "set output 'report/q8_avg_max_min.png'\n");
    fprintf(gp, "set title 'Land Temperature: Average, Max, and Min (1850-2015)' font 'Arial,14'\n");
    fprintf(gp, "set xlabel 'Year'\n");
    fprintf(gp, "set ylabel 'Temperature (°C)'\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set key top left\n");
    fprintf(gp, "plot 'data/q8_data.dat' using 1:2 with lines lw 3 lc rgb 'red'   title 'Land Avg Temp', \\\n");
    fprintf(gp, "     'data/q8_data.dat' using 1:3 with lines lw 1 lc rgb 'orange' dt 2 title 'Land Max Temp', \\\n");
    fprintf(gp, "     'data/q8_data.dat' using 1:4 with lines lw 1 lc rgb 'blue'  dt 2 title 'Land Min Temp'\n");
    fclose(gp);

    printf("Data file : data/q8_data.dat\n");
    printf("Script    : scripts/q8_plot.gnu\n");
    printf("Run: gnuplot scripts/q8_plot.gnu\n\n");
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

    FILE *df = fopen("data/q9_data.dat", "w");//df: file pointer for the data file, fopen: opens the file specified by "data/q9_data.dat" in write mode ("w")
    fprintf(df, "# Century  AvgTemp  MaxTemp  MinTemp\n");
    for (int c = 0; c < 3; c++) {//loop iterates over the 3 centuries defined in the ranges array, where c is the index of the current century being processed
        double sumA=0,sumX=0,sumN=0;
        int cntA=0,cntX=0,cntN=0;
        for (int i = 0; i < nRecords; i++) {//loop iterates over all the records in the data array, where i is the index of the current record being processed
            if (data[i].year < ranges[c][0] || data[i].year > ranges[c][1]) continue;//condition checks if the year of the current record falls outside the start and end years of the current century being processed, and if it does, the loop continues to the next iteration without executing the subsequent code for that record
            if (data[i].hasLandAvg) { sumA += data[i].landAvg; cntA++; }//condition checks if the landAvg field is present for that record, and if it is, the landAvg value of that record is added to sumA and cntA is incremented by 1
            if (data[i].hasLandMax) { sumX += data[i].landMax; cntX++; }//condition checks if the landMax field is present for that record, and if it is, the landMax value of that record is added to sumX and cntX is incremented by 1
            if (data[i].hasLandMin) { sumN += data[i].landMin; cntN++; }//condition checks if the landMin field is present for that record, and if it is, the landMin value of that record is added to sumN and cntN is incremented by 1
        }
        fprintf(df, "\"%s\"  %.4f  %.4f  %.4f\n",
                cenLabels[c],
                cntA>0 ? sumA/cntA : 0.0,//if cntA > 0, the average land temperature for that century is calculated by dividing the sum of land average temperatures (sumA) by the count of records (cntA), and if cntA is 0, it defaults to 0.0
                cntX>0 ? sumX/cntX : 0.0,//if cntX > 0, the average land maximum temperature for that century is calculated by dividing the sum of land maximum temperatures (sumX) by the count of records (cntX), and if cntX is 0, it defaults to 0.0
                cntN>0 ? sumN/cntN : 0.0);//if cntN > 0, the average land minimum temperature for that century is calculated by dividing the sum of land minimum temperatures (sumN) by the count of records (cntN), and if cntN is 0, it defaults to 0.0
    }
    fclose(df);

    FILE *gp = fopen("scripts/q9_plot.gnu", "w");//gp: file pointer for the GNUPlot script, fopen: opens the file specified by "scripts/q9_plot.gnu" in write mode ("w")
    fprintf(gp, "# GNUPlot script for Question 9\n");
    fprintf(gp, "set terminal pngcairo size 1200,900 enhanced font 'Arial,12'\n");
    fprintf(gp, "set output 'report/q9_bar_plots.png'\n");
    fprintf(gp, "set multiplot layout 1,3 title 'Average, Max, and Min Temperatures by Century' font 'Arial,14'\n");
    fprintf(gp, "set style data histogram\n");
    fprintf(gp, "set style fill solid 1.0 border -1\n");
    fprintf(gp, "set boxwidth 0.6\n");
    fprintf(gp, "set xtic rotate by -30 scale 0 font 'Arial,10'\n");
    fprintf(gp, "set ylabel 'Temperature (°C)'\n");
    fprintf(gp, "set yrange [0:*]\n");
    /* Subplot 1: Average */
    fprintf(gp, "set title 'Average Land Temperature'\n");
    fprintf(gp, "plot 'data/q9_data.dat' using 2:xtic(1) lc rgb 'steelblue'  title 'Average'\n");
    /* Subplot 2: Max */
    fprintf(gp, "set title 'Max Land Temperature'\n");
    fprintf(gp, "plot 'data/q9_data.dat' using 3:xtic(1) lc rgb 'red'        title 'Max'\n");
    /* Subplot 3: Min */
    fprintf(gp, "set title 'Min Land Temperature'\n");
    fprintf(gp, "plot 'data/q9_data.dat' using 4:xtic(1) lc rgb 'navy'   title 'Min'\n");
    fprintf(gp, "unset multiplot\n");
    fclose(gp);

    printf("Data file : data/q9_data.dat\n");
    printf("Script    : scripts/q9_plot.gnu\n");
    printf("Run: gnuplot scripts/q9_plot.gnu\n\n");
}

/* ================================================================== */
/*  QUESTION 10                                                         */
/*  Error bar plot: monthly avg land temp 2000-2015 with uncertainty   */
/* ================================================================== */
void question10(void) {
    printf("=======================================================\n");
    printf("QUESTION 10: GNUPlot - Error bar plot (2000-2015)\n");
    printf("=======================================================\n");

    FILE *df = fopen("data/q10_data.dat", "w");//w: write mode, if the file already exists, it will be truncated to zero length,
    //and the program will start writing from the beginning of the file. If the file does not exist, it will be created.
    fprintf(df, "# Month  AvgTemp  AvgUncertainty\n");

    const char *monthNames[] = {"Jan","Feb","Mar","Apr","May","Jun",
                                "Jul","Aug","Sep","Oct","Nov","Dec"};

    for (int m = 1; m <= 12; m++) {//loop iterates over the 12 months of the year, where m is the index of the current month being processed (1 for January, 2 for February, etc.)
        double sumT = 0, sumU = 0;
        int    cnt = 0;
        for (int i = 0; i < nRecords; i++) {//loop iterates over all the records in the data array, where i is the index of the current record being processed
            if (data[i].month == m &&//condition checks if the month of the current record matches the current month being processed
                data[i].year >= 2000 && data[i].year <= 2015 &&//condition checks if the year of the current record falls within the range of 2000 to 2015
                data[i].hasLandAvg) {//condition checks if the landAvg field is present for that record, , the landUncertainty value of that record is added to sumU, and cnt is incremented by 1
                sumT += data[i].landAvg;//and if all conditions are true, the landAvg value of that record is added to sumT, which accumulates the total land average temperature for that month across all records that match the criteria
                sumU += data[i].landUncertainty;//the landUncertainty value of that record is added to sumU, which accumulates the total uncertainty for that month across all records that match the criteria
                cnt++;//the cnt variable is incremented by 1, which counts the number of records that contribute to the average land temperature and uncertainty for that month across all records that match the criteria
            }
        }
        if (cnt > 0)//if (cnt > 0), the results are written to the data file along with the corresponding month name and month number
            fprintf(df, "%d  %.4f  %.4f  # %s\n",
                    m, sumT/cnt, sumU/cnt, monthNames[m-1]);//the average temperature and average uncertainty are calculated by dividing the respective sums by the count of records,
    }
    fclose(df);

    FILE *gp = fopen("scripts/q10_plot.gnu", "w");//gp: file pointer for the GNUPlot script, fopen: opens the file specified by "scripts/q10_plot.gnu" in write mode ("w")
    fprintf(gp, "# GNUPlot script for Question 10\n");
    fprintf(gp, "set terminal pngcairo size 1000,600 enhanced font 'Arial,12'\n");
    fprintf(gp, "set output 'report/q10_errorbars.png'\n");
    fprintf(gp, "set title 'Average Monthly Land Temperature with Error Bars (2000-2015)' font 'Arial,14'\n");
    fprintf(gp, "set xlabel 'Month'\n");
    fprintf(gp, "set ylabel 'Temperature (°C)'\n");
    fprintf(gp, "set xtics ('Jan' 1, 'Feb' 2, 'Mar' 3, 'Apr' 4, 'May' 5, 'Jun' 6, \\\n");
    fprintf(gp, "           'Jul' 7, 'Aug' 8, 'Sep' 9, 'Oct' 10, 'Nov' 11, 'Dec' 12)\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set key top right\n");
    fprintf(gp, "plot 'data/q10_data.dat' using 1:2:3 with yerrorbars lw 2 lc rgb 'dark-green' pt 7 title 'Avg Temp ± Uncertainty'\n");
    fclose(gp);

    printf("Data file : data/q10_data.dat\n");
    printf("Script    : scripts/q10_plot.gnu\n");
    printf("Run: gnuplot scripts/q10_plot.gnu\n\n");
}

/* ================================================================== */
/*  QUESTION 11                                                         */
/*  Line plot: LandAvgTemp + LandAndOceanAvgTemp 1850-2015             */
/* ================================================================== */
void question11(void) {
    printf("=======================================================\n");
    printf("QUESTION 11: GNUPlot - Land vs Land+Ocean 1850-2015\n");
    printf("=======================================================\n");

    FILE *df = fopen("data/q11_data.dat", "w");//df: file pointer for the data file, fopen: opens the file specified by "data/q11_data.dat" in write mode ("w")
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

    FILE *gp = fopen("scripts/q11_plot.gnu", "w");//gp: file pointer for the GNUPlot script, fopen: opens the file specified by "scripts/q11_plot.gnu" in write mode ("w")
    fprintf(gp, "# GNUPlot script for Question 11\n");
    fprintf(gp, "set terminal pngcairo size 1000,600 enhanced font 'Arial,12'\n");
    fprintf(gp, "set output 'report/q11_land_vs_ocean.png'\n");
    fprintf(gp, "set title 'Land vs Land+Ocean Average Temperature (1850-2015)' font 'Arial,14'\n");
    fprintf(gp, "set xlabel 'Year'\n");
    fprintf(gp, "set ylabel 'Average Temperature (°C)'\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set key top left\n");
    fprintf(gp, "plot 'data/q11_data.dat' using 1:2 with lines lw 2 lc rgb 'red'  title 'Land Average Temp', \\\n");
    fprintf(gp, "     'data/q11_data.dat' using 1:3 with lines lw 2 lc rgb 'blue' title 'Land+Ocean Average Temp'\n");
    fclose(gp);

    printf("Data file : data/q11_data.dat\n");
    printf("Script    : scripts/q11_plot.gnu\n");
    printf("Run: gnuplot scripts/q11_plot.gnu\n\n");
}

/* ================================================================== */
/*  MAIN                                                                */
/* ================================================================== */
int main(void) {
    /* Load data - adjust path as needed */
    loadCSV("data/GlobalTemperatures.csv");

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
    printf("  gnuplot scripts/q6_plot.gnu\n");
    printf("  gnuplot scripts/q7_plot.gnu\n");
    printf("  gnuplot scripts/q8_plot.gnu\n");
    printf("  gnuplot scripts/q9_plot.gnu\n");
    printf("  gnuplot scripts/q10_plot.gnu\n");
    printf("  gnuplot scripts/q11_plot.gnu\n");
    printf("=======================================================\n");

    return 0;
}