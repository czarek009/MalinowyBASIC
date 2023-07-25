BASIC:
10  PRINT "Fibbonacii",
20  INPUT "Which element should I calculate? " N
30  DIM TAB[N]
40  LET TAB[0] = 1
50  LET TAB[1] = 1
60  FOR I = 2 TO N-1 STEP 1
70  LET TAB[I] = TAB[I-1] + TAB[I-2]
80  NEXT
90  PRINT "Fib(";N;") = ";TAB[N-1],
100  PRINT "Fin."

C:
add_instruction(current_session, 10, "PRINT \"Fibbonacii\"");
add_instruction(current_session, 20, "INPUT \"Which element should I calculate? \" N");
add_instruction(current_session, 30, "DIM TAB[N]");
add_instruction(current_session, 40, "LET TAB[0] = 1");
add_instruction(current_session, 50, "LET TAB[1] = 1");
add_instruction(current_session, 60, "FOR I = 2 TO N-1 STEP 1");
add_instruction(current_session, 70, "LET TAB[I] = TAB[I-1] + TAB[I-2]");
add_instruction(current_session, 80, "NEXT");
add_instruction(current_session, 90, "PRINT \"Fib(\";N;\") = \";TAB[N-1],");
add_instruction(current_session, 100, "PRINT \"Fin.\"");