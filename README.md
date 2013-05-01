A simple request counter

COMPILE:
gcc request_counter.c -o request_counter -lpthread

RUN:
./request_counter port

where port is the port that it will listen on
