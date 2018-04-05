#include <glib.h>
#include <stdlib.h>
#include <math.h>
#include "matmul.h"
#include <assert.h>

#define CANARY_LENGTH 1000
#define MAX_SIZE 1000

typedef void (*matmul_t)(double*, double*, double*, int, int, int);

typedef struct {
    double* a;
    double* b;
} fixture;

void teardown(fixture* f, gconstpointer data){
    free(f->a);
    free(f->b);
}

double* create_zero_matrix(int rows, int cols){
    double* m = (double*) calloc(rows*cols,sizeof(double));
    assert(m != NULL);
    return m;
}

void setup_empty(fixture* f, gconstpointer data){
    f->a = create_zero_matrix(MAX_SIZE,MAX_SIZE);
    f->b = create_zero_matrix(MAX_SIZE,MAX_SIZE);
}

double* create_identity_matrix(int size){
    double* m = create_zero_matrix(size,size);
    for(int i =0; i < size; i++)
        m[i*size+i] = 1;
    return m;
}

double* create_random_matrix(int rows, int cols){
    double* m = create_zero_matrix(rows,cols);
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
    srand48((int)m);
    for(int i = 0; i < rows*cols; i++)
        m[i] = drand48();
    return m;
}

void setup_random(fixture* f, gconstpointer data){
    f->a = create_random_matrix(MAX_SIZE,MAX_SIZE);
    f->b = create_random_matrix(MAX_SIZE,MAX_SIZE);
}

void fill_matrix(double* m, int rows, int cols, double value){
    for(int i = 0; i < rows*cols; i++)
        m[i] = value;
}

double* create_constant_matrix(int rows, int cols, double value){
    double* m = create_zero_matrix(rows,cols);
    for(int i = 0; i < rows*cols; i++)
        m[i] = value;
    return m;
}


double* create_canary_matrix(double** canary, int rows,int cols){
    *canary = (double*)calloc(rows*cols+2*CANARY_LENGTH,sizeof(double));
    assert(*canary != NULL);
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
    srand48((int)(*canary));
    double* c = *canary;
    for(int i = 0; i < CANARY_LENGTH; i++)
        c[i] = drand48();
    c = *canary + CANARY_LENGTH + rows*cols;
    for(int i = 0; i < CANARY_LENGTH; i++)
        c[i] = drand48();
    return *canary + CANARY_LENGTH;
}

void check_canary_matrix(double* canary, int rows, int cols){
    srand48((int)canary);
    for(int i = 0; i < CANARY_LENGTH; i++)
        g_assert(canary[i] == drand48());
    canary += CANARY_LENGTH + rows*cols;
    for(int i = 0; i < CANARY_LENGTH; i++)
        g_assert(canary[i] == drand48());
}

int is_equal(double* a, double* b, int rows, int cols){
    for(int i = 0; i < rows*cols; i++)
        if (a[i] != b[i]) return 0;
    return 1;
}

int is_close(double* a, double* b, int rows, int cols){
    for(int i = 0; i < rows*cols; i++)
        if (fabs(a[i]-b[i]) > 1e-9) return 0;
    return 1;
}

int is_constant(double* m, int rows, int cols, double value){
    for(int i = 0; i < rows*cols; i++)
        if(m[i] != value) return 0;
    return 1;
}

void check_constant(double* m, int rows, int cols, double value){
    g_assert_true(is_constant(m,rows,cols,value));
}

void check_equal(double* a, double* b, int rows, int cols){
    g_assert_true(is_equal(a,b,rows,cols));
}



void test_fill(){
    double* canary;
    double* m = create_canary_matrix(&canary,100,100);
    fill_matrix(m,100,100,1234);
    check_constant(m,100,100,1234);
    check_canary_matrix(canary,100,100);
    free(canary);
}

void test_equal(){
    double* m1 = create_zero_matrix(100,100);
    double* m2 = create_zero_matrix(100,100);
    check_equal(m1,m2,100,100);
    fill_matrix(m1,100,100,10);
    fill_matrix(m2,100,100,10);
    check_equal(m1,m2,100,100);
    double* canary;
    double* m3 = create_canary_matrix(&canary, 100,100);
    fill_matrix(m3, 100,100,10);
    check_equal(m1,m2,100,100);
    check_canary_matrix(canary,100,100);
    free(m1); free(m2); free(canary);
}

void test_identity(){
    double* I = create_identity_matrix(100);
    double* m = create_random_matrix(1000,1000);
    double n[] = {10,100,500,1000,10000,0};
    for(int i = 0; n[i]; i++){
        double* canary;
        double* dst = create_canary_matrix(&canary,n[i],100);
        matmul_seq(m,I,dst,n[i],100,100);
        check_equal(m,dst,n[i],100);
        check_canary_matrix(canary,n[i],100);
        matmul_seq(I,m,dst,100,100,n[i]);
        check_equal(m,dst,n[i],100);
        check_canary_matrix(canary,n[i],100);
        free(canary);
    }
    free(I); free(m);
}

void test_zero(){
    double* zero = create_zero_matrix(1000,1000);
    double* m = create_random_matrix(1000,1000);
    double n[] = {10,100,500,1000,10000,0};
    for(int i = 0; n[i]; i++){
        double* canary;
        double* dst = create_canary_matrix(&canary,n[i],100);
        matmul_seq(m,zero,dst,n[i],100,100);
        check_equal(zero,dst,n[i],100);
        check_canary_matrix(canary,n[i],100);
        matmul_seq(zero,m,dst,100,100,n[i]);
        check_equal(zero,dst,n[i],100);
        check_canary_matrix(canary,n[i],100);
        free(canary);
    }
    free(zero); free(m);
}

void test_constant(fixture* f, gconstpointer data){
    matmul_t matmul = (matmul_t)data;
    fill_matrix(f->a,MAX_SIZE,MAX_SIZE,2);
    fill_matrix(f->b,MAX_SIZE,MAX_SIZE,3);
    double n[] = {10,100,500,1000,0};
    for(int nra = 0; n[nra]; nra++)
        for(int nca = 0; n[nca]; nca++)
            for(int ncb =0; n[ncb]; ncb++){
                double* canary;
                double* dst = create_canary_matrix(&canary,n[nra],n[ncb]);
                matmul(f->a,f->b,dst,n[nra],n[nca],n[ncb]);
                double value = 6 * n[nca];
                check_constant(dst,n[nra],n[ncb],value);
                check_canary_matrix(canary,n[nra],n[ncb]);
                free(canary);
    }
}




void test_random(fixture* f, gconstpointer data){
    matmul_t matmul = (matmul_t)data;
    double n[] = {10,100,500,1000,0};
    for(int nra = 0; n[nra]; nra++)
        for(int nca = 0; n[nca]; nca++)
            for(int ncb =0; n[ncb]; ncb++){
                double* canary;
                double* dst = create_canary_matrix(&canary,n[nra],n[ncb]);
                double* ref = create_zero_matrix(n[nra],n[ncb]);
                matmul_seq(f->a,f->b,ref,n[nra],n[nca],n[ncb]);
                matmul(f->a,f->b,dst,n[nra],n[nca],n[ncb]);
                g_assert_true(is_close(ref,dst,n[nra],n[ncb]));
                check_canary_matrix(canary,n[nra],n[ncb]);
                free(canary);free(ref);
    }
}


int main(int argc, char **argv){
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/sanity/test_fill", test_fill);
    g_test_add_func("/sanity/test_equal", test_equal);
    g_test_add_func("/matmul/seq/identity",test_identity);
    g_test_add_func("/matmul/seq/zero",test_zero);
    g_test_add("/matmul/seq/constant",fixture,matmul_seq,setup_empty,test_constant,teardown);
    g_test_add("/matmul/par/outer/random",fixture,matmul_o,setup_random,test_random,teardown);
    g_test_add("/matmul/par/inner/random",fixture,matmul_i,setup_random,test_random,teardown);
    g_test_add("/matmul/par/inner/constant",fixture,matmul_i,setup_empty,test_constant,teardown);
    g_test_add("/matmul/par/outer/constant",fixture,matmul_o,setup_empty,test_constant,teardown);
    return g_test_run();
}
