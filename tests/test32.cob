       IDENTIFICATION DIVISION.
           PROGRAM-ID. TEST-PICTURE-CLAUSES.

       ENVIRONMENT DIVISION.

       DATA DIVISION.

       WORKING-STORAGE SECTION.

       01  TEST1.
           05 T1-FLD1 PIC 9999 VALUE 0.
           05 T1-FLD2 PIC 9999 VALUE 0.02.
           05 T1-FLD3 PIC 9999 VALUE 2.
           05 T1-FLD4 PIC 9999 VALUE 1234.5.
           05 T1-FLD5 PIC 9999 VALUE -123.4.

       01  TEST2.
           05 T2-FLD1 PIC ZZ99 VALUE 0.
           05 T2-FLD2 PIC ZZ99 VALUE 0.02.
           05 T2-FLD3 PIC ZZ99 VALUE 2.
           05 T2-FLD4 PIC ZZ99 VALUE 1234.5.
           05 T2-FLD5 PIC ZZ99 VALUE -123.4.

       01  TEST3.
           05 T3-FLD1 PIC ZZZZ VALUE 0.
           05 T3-FLD2 PIC ZZZZ VALUE 0.02.
           05 T3-FLD3 PIC ZZZZ VALUE 2.
           05 T3-FLD4 PIC ZZZZ VALUE 1234.5.
           05 T3-FLD5 PIC ZZZZ VALUE -123.4.

       01  TEST4.
           05 T4-FLD1 PIC .ZZ VALUE 0.
           05 T4-FLD2 PIC .ZZ VALUE 0.02.
           05 T4-FLD3 PIC .ZZ VALUE 2.
           05 T4-FLD4 PIC .ZZ VALUE 1234.5.
           05 T4-FLD5 PIC .ZZ VALUE -123.4.

       01  TEST5.
           05 T5-FLD1 PIC ZZ.ZZ VALUE 0.
           05 T5-FLD2 PIC ZZ.ZZ VALUE 0.02.
           05 T5-FLD3 PIC ZZ.ZZ VALUE 2.
           05 T5-FLD4 PIC ZZ.ZZ VALUE 1234.5.
           05 T5-FLD5 PIC ZZ.ZZ VALUE -123.4.

       PROCEDURE DIVISION.
           DISPLAY "-------- TEST 1: 9999 ----------".
           DISPLAY "DATA    |    0   |     .02|    2  | 1234.5 | -123.4 |".
           DISPLAY "---".
           DISPLAY "CORRECT |0000|0000|0002|1234|0123|".
           DISPLAY "RESULTS |" 
                            T1-FLD1 "|"
                            T1-FLD2 "|"
                            T1-FLD3 "|"
                            T1-FLD4 "|"
                            T1-FLD5 "|".
           DISPLAY "--------------------------------".

           DISPLAY "-------- TEST 2: ZZ99 ----------".
           DISPLAY "DATA    |    0   |     .02|   2   | 1234.5 | -123.4 |".
           DISPLAY "---".
           DISPLAY "CORRECT |  00|  00|  02|1234|0123|".
           DISPLAY "RESULTS |" 
                            T2-FLD1 "|"
                            T2-FLD2 "|"
                            T2-FLD3 "|"
                            T2-FLD4 "|"
                            T2-FLD5 "|".
           DISPLAY "--------------------------------".

           DISPLAY "-------- TEST 3: ZZZZ ----------".
           DISPLAY "DATA    |    0   |     .02|   2   | 1234.5 | -123.4 |".
           DISPLAY "---".
           DISPLAY "CORRECT |    |    |   2|1234| 123|".
           DISPLAY "RESULTS |" 
                            T3-FLD1 "|"
                            T3-FLD2 "|"
                            T3-FLD3 "|"
                            T3-FLD4 "|"
                            T3-FLD5 "|".
           DISPLAY "--------------------------------".

           DISPLAY "-------- TEST 4: .ZZ ----------".
           DISPLAY "DATA    |    0   |     .02|   2   | 1234.5 | -123.4 |".
           DISPLAY "---".
           DISPLAY "CORRECT |    |.02|    |.50|.40|".
           DISPLAY "RESULTS |" 
                            T4-FLD1 "|"
                            T4-FLD2 "|"
                            T4-FLD3 "|"
                            T4-FLD4 "|"
                            T4-FLD5 "|".
           DISPLAY "--------------------------------".

           DISPLAY "-------- TEST 5: ZZ.ZZ ----------".
           DISPLAY "DATA    |    0   |     .02|   2   | 1234.5 | -123.4 |".
           DISPLAY "---".
           DISPLAY "CORRECT |    |.02|2.00|34.50|23.40|".
           DISPLAY "RESULTS |" 
                            T5-FLD1 "|"
                            T5-FLD2 "|"
                            T5-FLD3 "|"
                            T5-FLD4 "|"
                            T5-FLD5 "|".
           DISPLAY "--------------------------------".

