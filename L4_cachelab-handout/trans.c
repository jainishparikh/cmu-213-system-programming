/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

void transpose_64_64(int M, int N, int A[N][M], int B[M][N]){
    register int t1, t2, t3, t4, t5, t6, t7, t8;
    int i, j, l;
    for (l = 0; l < M; l += 8) {
        for (j = 0; j < N; j += 8) {
            
            // --- STEP 1: Process top 4 rows of the A-block ---
            for (i = 0; i < 4; i++) {
                // Read row i of the current A-block
                t1 = A[l + i][j + 0]; t2 = A[l + i][j + 1]; 
                t3 = A[l + i][j + 2]; t4 = A[l + i][j + 3];
                t5 = A[l + i][j + 4]; t6 = A[l + i][j + 5]; 
                t7 = A[l + i][j + 6]; t8 = A[l + i][j + 7];

                // Transpose A's Top-Left into B's Top-Left
                B[j + 0][l + i] = t1; B[j + 1][l + i] = t2;
                B[j + 2][l + i] = t3; B[j + 3][l + i] = t4;

                // "Park" A's Top-Right into B's Top-Right HORIZONTALLY (Parking Lot)
                B[j + i][l + 4] = t5; B[j + i][l + 5] = t6;
                B[j + i][l + 6] = t7; B[j + i][l + 7] = t8;
            }

            // --- STEP 2: The Hand-off (The "Switch") ---
            for (i = 0; i < 4; i++) {
                // 1. Read the "Parked" row from B's Top-Right
                t5 = B[j + i][l + 4]; t6 = B[j + i][l + 5];
                t7 = B[j + i][l + 6]; t8 = B[j + i][l + 7];

                // 2. Read the new Bottom-Left COLUMN from Matrix A
                t1 = A[l + 4][j + i]; t2 = A[l + 5][j + i];
                t3 = A[l + 6][j + i]; t4 = A[l + 7][j + i];

                // 3. Place A's values into B's Top-Right (Correctly transposed)
                B[j + i][l + 4] = t1; B[j + i][l + 5] = t2;
                B[j + i][l + 6] = t3; B[j + i][l + 7] = t4;

                // 4. Place the Parked values into B's Bottom-Left (Now it's transposed!)
                B[j + i + 4][l + 0] = t5; B[j + i + 4][l + 1] = t6;
                B[j + i + 4][l + 2] = t7; B[j + i + 4][l + 3] = t8;
            }

            // --- STEP 3: Transpose the Bottom-Right 4x4 ---
            for (i = 4; i < 8; i++) {
                t1 = A[l + i][j + 4]; t2 = A[l + i][j + 5];
                t3 = A[l + i][j + 6]; t4 = A[l + i][j + 7];

                B[j + 4][l + i] = t1; B[j + 5][l + i] = t2;
                B[j + 6][l + i] = t3; B[j + 7][l + i] = t4;
            }
        }
    }
}



/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if(M==N && M==32){
        int i, j, l, k, t1, t2, t3, t4, t5, t6, t7, t8;
        for(l=0; l<=M-8; l= l+8){
            for(k=0; k<=N-8; k =k+8){
                j = k;
                for (i = l; i < l+8; i++) {
                    t1 = A[i][j];
                    t2 = A[i][j+1];
                    t3 = A[i][j+2];
                    t4 = A[i][j+3];
                    t5 = A[i][j+4];
                    t6 = A[i][j+5];
                    t7 = A[i][j+6];
                    t8 = A[i][j+7];

                    B[j][i] = t1;
                    B[j+1][i] = t2;
                    B[j+2][i] = t3;
                    B[j+3][i] = t4;
                    B[j+4][i] = t5;
                    B[j+5][i] = t6;
                    B[j+6][i] = t7;
                    B[j+7][i] = t8;     
                }  
            }
        }
    }
    else if(M==N && (M==64)){
        transpose_64_64(M, N, A, B);
    }
}


/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 
    // registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

