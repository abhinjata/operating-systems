#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

//merge function
void merge(int arr[], int left, int mid, int right){

    int n1 = m - left + 1;
    int n2 = r - m;
    int Left[n1], Right[n2];
    for (int i = 0; i < n1; i++) Left[i] = arr[left + i];
    for (int j = 0; j < n2; j++) Right[j] = arr[mid + 1 + j];
    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2){

    if (Left[i] <= Right[j]){

        arr[k] = Left[i];
        i++;

    } 
    
    else{
        arr[k] = Right[j];
        j++;
    }

    k++;
}

    while (i < n1) arr[k++] = Left[i++]; //push the rest
    while (j < n2) arr[k++] = Right[j++];
}

//merge sort function
void mergeSort(int arr[], int left, int right){

    if (left < right) {

        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

int main(){

    int arr[] = {5, 2, 9, 1, 6, 3};
    int n = sizeof(arr) / sizeof(arr[0]);
    int mid = n / 2;

    int pipefd1[2], pipefd2[2];
    pipe(pipefd1); // For first half
    pipe(pipefd2); // For second half

    pid_t pid1 = fork();

    if (pid1 == 0){ // Child 1

        close(pipefd1[0]); // Close read end
        mergeSort(arr, 0, mid - 1);
        write(pipefd1[1], arr, mid * sizeof(int));
        close(pipefd1[1]);
        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 == 0){ // Child 2

        close(pipefd2[0]); // Close read end
        mergeSort(arr, mid, n - 1);
        write(pipefd2[1], arr + mid, (n - mid) * sizeof(int));
        close(pipefd2[1]);
        exit(0);
    }

    //parent process
    close(pipefd1[1]); 
    close(pipefd2[1]); 

    int first_half[mid], second_half[n - mid];

    read(pipefd1[0], first_half, mid * sizeof(int));
    read(pipefd2[0], second_half, (n - mid) * sizeof(int));

    close(pipefd1[0]);
    close(pipefd2[0]);

    wait(NULL); //wait for both children
    wait(NULL);

    //merge the two sorted halves
    int sorted[n], i = 0, j = 0, k = 0;

    while (i < mid && j < n - mid){

        sorted[k++] = (first_half[i] <= second_half[j]) ? first_half[i++] : second_half[j++];

    }

    while (i < mid) sorted[k++] = first_half[i++];
    while (j < n - mid) sorted[k++] = second_half[j++];

    printf("Sorted array: ");
    for (int x = 0; x < n; x++) printf("%d ", sorted[x]);
    printf("\n");

    return 0;
}
