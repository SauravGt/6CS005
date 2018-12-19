#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <crypt.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

//Compile with mpicc CrackAZ99_mpi.c -o CrackAZ99_mpi -lcrypt
//Run with mpiexec ./CrackAZ99_mpi > CrackAZ99MpiResults.txt

int n_passwords = 4;

int size, rank;
char buffer[4];

char *encrypted_passwords[] = {
 "$6$KB$6SsUGf4Cq7/Oooym9WWQN3VKeo2lynKV9gXVyEG4HvYy1UFRx.XAye89TLp/OTcW7cGpf9UlU0F.cK/S9CfZn1",
  "$6$KB$kwebHrK29hjigYG8zvp87N2VpoQE51HML2nZsPyUYIRTcdOpbfCWgjr5wfdXEgNJFOMyt1weZuyTr8AmTZZ8d/",
  "$6$KB$OtjvdN6IOVby7U6PRYd2fBEjTv6R0IpsFfsy5JwtyZfsz5rBdrMnARYV9s7PxHkvKTI8wLz5YbEYFCFq1SUgC1",
  "$6$KB$MD4fomhxal9x9Bc5K06IHDh32W.8osCGJepMkj8H1QRuHP9OAtR8UpGHEGZ4Rj0mOm/qIBhnriusixnF6rpPW1"
};

void substr(char *dest, char *src, int start, int length){
  memcpy(dest, src + start, length);
  *(dest + length) = '\0';
}

void crack(char *salt_and_encrypted){
  int x, y, z;
  char salt[7];
  char plain[7];
  char *enc;
  int count = 0;

  char xStart, yStart, xEnd, yEnd;

  if (rank == 1) {
	  xStart = 'A'; 
	  yStart = 'A'; 
	  xEnd = 'M';
	  yEnd = 'Z';
  }

  if (rank == 2) {
	  xStart = 'N';
	  yStart = 'A';
	  xEnd = 'Z';
	  yEnd = 'Z';
  }

  substr(salt, salt_and_encrypted, 0, 6);

  for(x=xStart; x<=xEnd; x++){
    for(y=yStart; y<=yEnd; y++){
        for(z=0; z<=9999; z++){
          sprintf(plain, "%c%c%02d", x, y,z);
          enc = (char *) crypt(plain, salt);
          count++;
          if(strcmp(salt_and_encrypted, enc) == 0){
            printf("#%-8d%s %s\n", count, plain, enc);
          } else {
            printf(" %-8d%s %s\n", count, plain, enc);
          }
        }
    }
  }
  printf("%d solutions explored\n", count);
}

int time_difference(struct timespec *start, struct timespec *finish, long long int *difference) {
	long long int ds = finish->tv_sec - start->tv_sec;
	long long int dn = finish->tv_nsec - start->tv_nsec;

	if (dn < 0) {
		ds--;
		dn += 1000000000;
	}

	*difference = ds * 1000000000 + dn;
	return !(*difference > 0);
}

int main(int argc, char *argv[]){
  MPI_Status status;
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  struct timespec start, finish;
  long long int time_elapsed;
  clock_gettime(CLOCK_MONOTONIC, &start);
  
  int i;

  if (rank == 0) {
	//MPI_Finalize();
  }
  else {
	  for (i = 0; i < n_passwords; i++) {
		  crack(encrypted_passwords[i]);
	  }
  }

  MPI_Finalize();

  clock_gettime(CLOCK_MONOTONIC, &finish);
  time_difference(&start, &finish, &time_elapsed);
  printf("Time elasped was %lldns or %0.9lfs\n", time_elapsed, (time_elapsed / 1.0e9));

  return 0;
}
