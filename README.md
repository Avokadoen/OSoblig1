# README #

Aksel Hjerpbakk   
Mathias Stifjeld  
Nikolai Åkerholt  

Dette har vi gjort for å kvalitetssikkre koden:

1_fork.c
1_p_thread.c
1_producer_consumer.c

cppcheck --enable=all ./1_fork.c
clang-tidy-5.0 -checks='*' 1_fork.c -- -std=c11
valgrind --leak-check=yes ./1_fork.c
valgrind --tool=helgrind ./1_fork.c
