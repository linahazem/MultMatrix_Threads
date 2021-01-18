#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

int row1,column1,row2,column2;
int arr1[50][50],arr2[50][50];

void readFile()
{
    int i,j;
    FILE* fp;
    fp = fopen("input.txt","r");
    fscanf(fp,"%d",&row1);
    fscanf(fp,"%d",&column1);

    for(i = 0 ; i < row1 ; i++)
    {
        for(j = 0 ; j < column1 ; j++)
            fscanf(fp,"%d",&arr1[i][j]);
    }

    fscanf(fp,"%d",&row2);
    fscanf(fp,"%d",&column2);

    for(i = 0 ; i < row2 ; i++)
    {
        for(j = 0 ; j < column2 ; j++)
            fscanf(fp,"%d",&arr2[i][j]);
    }

    fclose(fp);

}
void displayMatrix()
{
    int i,j;

    printf("DISPLAY MATRIX 1: \n");
    for(i = 0 ; i < row1 ; i++)
    {
        for(j = 0 ; j < column1 ; j++)
            printf("%d ",arr1[i][j]);
        printf("\n");
    }

    printf("DISPLAY MATRIX 2: \n");
    for(i = 0 ; i < row2 ; i++)
    {
        for(j = 0 ; j < column2 ; j++)
            printf("%d ",arr2[i][j]);
        printf("\n");
    }
}
void *mult_each_element(void* arg)
{
    int* data = (int*)arg;
    int sum = 0,i;

    for(i = 0 ; i < column1 ; i++)
        sum = sum + data[i]*data[i+column1];
    int *p = (int*)malloc(sizeof(int));
    *p = sum; //pointer to sum

    pthread_exit(p); //terminate a thread
}
void compute_each_element()
{
    int max = row1*column2;
    int count = 0, i, j, k;
    int* data = NULL;

    clock_t start = clock();

    pthread_t *threads = (pthread_t*)malloc(max*sizeof(pthread_t));


    for(i = 0 ; i < row1 ; i++)
    {
        for(j = 0 ; j < column2 ; j++)
        {
            data = (int*)malloc(20*sizeof(int));
            for(k = 0 ; k < column1 ; k++)
                data[k] = arr1[i][k];
            for(k = 0 ; k < row2 ; k++)
                data[k+column1] = arr2[k][j];
            pthread_create(&threads[count++], NULL, mult_each_element, (void*)(data));
        }
    }

    printf("\nOUTPUT MATRIX (EACH ELEMENT IN A THREAD): \n");
    FILE* fp;
    fp = fopen("output.txt","w");
    fprintf(fp,"Output of Matrix (Each element in a thread)\n");
    for(i = 0 ; i < max ; i++)
    {
        void *k; //pointer points to any var not declared
        pthread_join(threads[i],&k);
        int* p = (int*)k;

        printf("%d ",*p);
        fprintf(fp,"%d ",*p);

        if((i+1) % column2 == 0)
        {
            printf("\n");
            fprintf(fp,"\n");
        }
    }
    clock_t end = clock();
    double elapsed_time = (double)(end-start)/CLOCKS_PER_SEC;

    printf("TIME MEASURED BY EACH ELEMENT IN A THREAD: %.5f seconds\n",elapsed_time);

    fprintf(fp,"Time elapsed = %.5f \n",elapsed_time);
    fprintf(fp,"\n");
    fclose(fp);
}

void *mult_each_row(void* arg)
{
    int* data = (int*)arg; //contains row1 column1 row1 column2...
    int* rowMul = NULL;
    int sum , i, j, c=0, c1r2;
    c1r2 = column1 +row2; //to go to the next row column in data
    rowMul = (int*)malloc(row1*sizeof(int));
    for(i = 0 ; i < column1 ; i++)
    {
        sum=0;
        for(j = 0 ; j < column1 ; j++)
            sum = sum + data[j+c*c1r2]*data[j+column1+c*c1r2];

        rowMul[i] = sum;
        c++; //when nth row column reached add c by 1
             //to go to next row column in data
    }

    int *p = (int*)malloc(sizeof(int));
    p = rowMul;

    pthread_exit(p); //terminate a thread
}
void compute_each_row()
{
    int max = row1*column2;
    int count = 0, i, j, k, c,c1r2;
    int* data = NULL;

    clock_t start = clock();

    pthread_t *threads = (pthread_t*)malloc(max*sizeof(pthread_t));

    c1r2 =column1+row2; // to add it to index of array to take
                        // the 1st row with 1st column of 2nd
                        // matrix then 1st row with 2nd col...
                        //for example everytime add 10

    for(i = 0 ; i < row1 ; i++)
    {
        data = (int*)malloc(120*sizeof(int));
        c=0;
        for(j = 0 ; j < column2 ; j++)
        {
            for(k = 0 ; k < column1 ; k++)
                data[k+c*c1r2] = arr1[i][k];
            for(k = 0 ; k < row2 ; k++)
                data[k+column1+c*c1r2] = arr2[k][j];

            c++; //when nth row column reached add c by 1
                 //to go to next row column in data
        }
        pthread_create(&threads[count++], NULL, mult_each_row, (void*)(data));
    }

    printf("\nOUTPUT MATRIX (EACH ROW IN A THREAD): \n");

    FILE* fp;
    fp = fopen("output.txt","a");
    fprintf(fp,"Output of Matrix (Each row in a thread)\n");
    for(i = 0 ; i < row1 ; i++)
    {
        void *k; //pointer points to any var not declared
        pthread_join(threads[i],&k);
        int* p = (int*)k;
        for(j = 0 ; j < column2 ; j++)
        {
            int* p2 = p;
            printf("%d ",*p++);
            fprintf(fp,"%d ",*p2++);
            if((j+1) % column2 == 0) //after each row new line
            {
                printf("\n");
                fprintf(fp,"\n");
            }
        }

    }
    clock_t end = clock();
    double elapsed_time = (double)(end-start)/CLOCKS_PER_SEC;

    printf("TIME MEASURED BY EACH ROW IN A THREAD: %.5f seconds\n",elapsed_time);

    fprintf(fp,"Time elapsed = %.5f",elapsed_time);
    fclose(fp);
}

int main()
{

    readFile();
    displayMatrix();
    compute_each_element();
    compute_each_row();

    return 0;
}
