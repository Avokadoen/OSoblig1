# README #

Aksel Hjerpbakk   997816

Mathias Stifjeld  473199

Nikolai Åkerholt  473184

Dette har vi gjort for å kvalitetssikkre koden:
Vi gjorde det med alle filene, men kun en er skrevet for ikke å repetere så mye. Filene er:

1_fork.c

1_p_thread.c

1_producer_consumer.c


valgrind --leak-check=yes ./1_fork.c

valgrind --tool=helgrind ./1_fork.c
