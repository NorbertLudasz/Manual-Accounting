#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <string.h>

#define TH_COUNT 4
#define DOLGOZOSZAM 3
#define FONOKSZAM 1

int** allokalas(int xdim, int ydim) {
	int** kasszak;
	kasszak = (int**) calloc(xdim, sizeof(int*));
	for (int i = 0; i < xdim; i++) {
		kasszak[i] = (int*) calloc(ydim, sizeof(int));
	}
	return kasszak;
}

void deallokalas(int **kasszak, int xdim) {
	for(int i=0;i<xdim;i++){
		free(kasszak[i]);
	}
	free(kasszak);
}

int generalkassza(int rank, int** kasszak, int* seeds, int* jegyarak, int nr, int hibasKasszas){
	int seed = seeds[rank];
	int profit = 0;
	for(int i=0;i<nr;i++){
		int jegytipus = rand_r((unsigned int *)&seed)%4 + 1;
		profit = profit + jegyarak[jegytipus];
		kasszak[rank][i] = jegytipus;
	}

	if(rank == hibasKasszas){
		printf("hibas kasszas tid %d\n", rank);
		int hibasJegyPos = rand_r((unsigned int *)&seed)%nr;
		int ujjegy = 1;
		if(ujjegy == kasszak[rank][hibasJegyPos]){
			ujjegy = 2;
		}
		kasszak[rank][hibasJegyPos] = ujjegy;
	}

	char filename[32];
	sprintf(filename, "bevetel");
	FILE *out;
	out = fopen(filename, "a");
	fprintf(out, "%d ", profit); 
	//fprintf(out, "\n");
	fclose(out);

	printf("kassza %d profit %d \n", rank, profit);

	char filename2[32];
	sprintf(filename2, "%d_kassza", rank+1);
	FILE *out2;
	out2 = fopen(filename2, "a");
	for(int i=0;i<nr;i++){
		//printf("kassza %d elem %d", rank, kasszak[rank][i]);
		fprintf(out2, "%d\n ", kasszak[rank][i]);
	}
	fclose(out2);


	//int profitVerification = 0;
	/*for(int i=0;i<nr;i++){
		//profitVerification += kasszak[rank][i];
		printf("kasssza %d elem %d", rank, kasszak[rank][i]);
	}*/
	
	//fprintf("kassza %d profit verification %d \n", rank, profitVerification);
	
	return profit;
}



int main(int argc, char* argv[]) {
	if(argc < 2){
		printf("Nincs megadva nr parameter\n");
		return 1;
	}
	int nr = atoi(argv[1]);
	int** kasszak = allokalas(4, nr);
	
	int* seeds = (int*)calloc(4, sizeof(int));
	srand(time(NULL));
	for (int i = 0; i < 4; i++) {
	        seeds[i] = rand();
	}

	int* jegyarak = (int*)calloc(4, sizeof(int));
	jegyarak[0] = 25;
	jegyarak[1] = 50;
	jegyarak[2] = 100;
	jegyarak[3] = 150;

	int hibasKasszas = rand()%4;
#pragma omp parallel num_threads(TH_COUNT)\
	default(none) shared(kasszak, seeds, jegyarak, nr, hibasKasszas)
	{
		int rank = omp_get_thread_num();
		int profit = generalkassza(rank, kasszak, seeds, jegyarak, nr, hibasKasszas);
	
	}

	int* numbers = (int*)calloc(4*nr, sizeof(int));//jegyek
	int* numbersF = (int*)calloc(4*nr, sizeof(int));
	int* counts = (int*)calloc(4, sizeof(int));//jegyek szama
	int* countsF = (int*)calloc(4, sizeof(int));
	int osszProfit = 0;
	int osszProfitF = 0;
	double starttime = omp_get_wtime()*1000;
#pragma omp parallel num_threads(DOLGOZOSZAM)\
	default(none)\
	shared(numbers, counts, nr, jegyarak, osszProfit)
	{
		int tid = omp_get_thread_num();
		int start, end;
		if (tid != (DOLGOZOSZAM-1)) 
		{
			start = tid * (nr / DOLGOZOSZAM);
			end = (tid + 1) * (nr / DOLGOZOSZAM);
		}else {
			start = tid * (nr / DOLGOZOSZAM);
			end = nr;
		}
		printf("tid start end: %d %d %d \n", tid, start, end);
		for(int i=1;i<=4;i++){
			char filename[32];
			sprintf(filename, "%d_kassza", i);

			FILE *file = fopen(filename,"r");
			if(file != NULL){
				//fseek(file, start * sizeof(int), SEEK_SET);

				int num;
				//int count = start;
				int count = 0;
				while (count < end && fscanf(file, "%d", &num) == 1){
					if(count >= start){
						//printf("tid %d file %d index %d num %d\n", tid, i, count, num);
							#pragma omp critical
							{
								//printf("tid %d file %d index %d num %d critical\n", tid, i, count, num);
								counts[num-1]++;
								numbers[(i-1)*nr+count] = num;
								osszProfit += jegyarak[num-1];
							}
					}
					count++;
				}
			}
			fclose(file);
		}
	}
	double endtime = omp_get_wtime()*1000;
	double ptime = endtime - starttime;
	printf("Parhuzamos ido: %f\n", ptime);
	printf("Ossz profit: %d\n", osszProfit);

	printf("Jegyek szama osszesen: ");
	for(int i=0;i<4;i++){
		printf("%d ",counts[i]);
	}
	printf("\n");

	/*printf("numbers: ");
	for(int i=0;i<4*nr;i++){
		printf("%d ",numbers[i]);
	}
	printf("\n");*/

	//fonok
	int fonokOsszProfit = 0;
	char filenameF[32];
	sprintf(filenameF, "bevetel");
	FILE *file = fopen(filenameF, "r");
	if(file != NULL){
		int count = 0;
		int num;
		//printf("fonok count %d\n", count);
		while(count < 4 && fscanf(file, "%d", &num) == 1){
			printf("fonok count %d num %d\n", count, num);
			fonokOsszProfit = fonokOsszProfit + num;
			count++;
		}
	}

	printf("fonok altal szamolt profit: %d\n", fonokOsszProfit);


//int FONOKSZAM = 1;
double starttime2 = omp_get_wtime()*1000;
#pragma omp parallel num_threads(FONOKSZAM)\
	default(none)\
	shared(numbersF, countsF, nr, jegyarak, osszProfitF)
	{
		int tid = omp_get_thread_num();
		int start, end;
		if (tid != (FONOKSZAM-1)) 
		{
			start = tid * (nr / FONOKSZAM);
			end = (tid + 1) * (nr / FONOKSZAM);
		}else {
			start = tid * (nr / FONOKSZAM);
			end = nr;
		}
		printf("tid start end: %d %d %d \n", tid, start, end);
		for(int i=1;i<=4;i++){
			char filename[32];
			sprintf(filename, "%d_kassza", i);

			FILE *file = fopen(filename,"r");
			if(file != NULL){
				//fseek(file, start * sizeof(int), SEEK_SET);

				int num;
				//int count = start;
				int count = 0;
				while (count < end && fscanf(file, "%d", &num) == 1){
					if(count >= start){
						//printf("tid %d file %d index %d num %d\n", tid, i, count, num);
							#pragma omp critical
							{
								//printf("tid %d file %d index %d num %d critical\n", tid, i, count, num);
								countsF[num-1]++;
								numbersF[(i-1)*nr+count] = num;
								osszProfitF += jegyarak[num-1];
							}
					}
					count++;
				}
			}
			fclose(file);
		}
	}
	double endtime2 = omp_get_wtime()*1000;
	double stime = endtime2 - starttime2;
	printf("Szekvencialis ido: %f\n", stime);
	printf("Ossz profit: %d\n", osszProfit);
	double gyorsitas = stime/ptime;
	double hatekonysag = gyorsitas/DOLGOZOSZAM;
	printf("Gyorsitas: %lf\n", gyorsitas);
	printf("Hatekonysag: %lf\n", hatekonysag);

	printf("a");
	free(counts);
	printf("b");
	free(numbers);
	printf("c");
	free(jegyarak);
	printf("d");
	free(seeds);
	printf("e");
	deallokalas(kasszak, 4);
	printf("f");
	return 0;
}
