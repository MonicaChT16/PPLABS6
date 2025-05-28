#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#define MAX_THREADS 8

typedef struct{
    double *A;
    double *B;
    int inicio;
    int fin;
} ThreadData;

double resultado_paralelo=0.0;
pthread_mutex_t mutex;

double producto_punto_secuencial(double *A, double *B, int N){
    double resultado=0.0;
    for (int i=0; i<N; i++){
        resultado+=A[i]*B[i];
    }
    return resultado;
}

void *producto_punto_paralelo(void *arg){
    ThreadData *data=(ThreadData *)arg;
    double suma_parcial=0.0;
    for (int i=data->inicio; i<data->fin; i++){
        suma_parcial+=data->A[i]*data->B[i];
    }
    pthread_mutex_lock(&mutex);
    resultado_paralelo+=suma_parcial;
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]){
    double A[]={1.0, 2.0, 3.0};
    double B[]={4.0, 5.0, 6.0};
    int N=3;

    printf("A=[");
    for (int i=0; i<N; i++) printf("%.1f%s", A[i], (i==N-1) ? "]\n":", ");
    printf("B=[");
    for (int i=0; i<N; i++) printf("%.1f%s", B[i], (i==N-1) ? "]\n":", ");

    double resultado_seq=producto_punto_secuencial(A, B, N);
    printf("\nProducto punto secuencial:%.2f\n", resultado_seq);

    int num_hilos=2;

    pthread_t hilos[num_hilos];
    ThreadData thread_data[num_hilos];
    pthread_mutex_init(&mutex, NULL);
    int segmento=N/num_hilos;
    int resto=N%num_hilos;

    int inicio=0;
    for (int i=0; i<num_hilos; i++){
        thread_data[i].A=A;
        thread_data[i].B=B;
        thread_data[i].inicio=inicio;
        int fin=inicio+segmento+(i<resto?1:0);
        thread_data[i].fin=fin;
        inicio=fin;

        if (pthread_create(&hilos[i], NULL, producto_punto_paralelo, &thread_data[i]) !=0){
            perror("Error al crear hilo");
            exit(EXIT_FAILURE);
        }
    }

    for (int i=0; i<num_hilos; i++){
        pthread_join(hilos[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    printf("Producto punto paralelo (con%d hilos):%.2f\n", num_hilos, resultado_paralelo);
    return 0;
}
