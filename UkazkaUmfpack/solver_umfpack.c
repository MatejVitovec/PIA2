#include "solver_umfpack.h"

void solve(double *x, double *b, int N) {
    const int32_t n = (N-1)*(N-1);
    int32_t *Ap = (int32_t*) malloc((n+1)*sizeof(int32_t));
    const int32_t n_elem = 2*(6+(N-2)*4) + (N-3)*(8+(N-2)*5);

    int32_t *Ai = (int32_t*) malloc(n_elem*sizeof(int32_t));
    double *Ax = (double*) malloc(n_elem*sizeof(double));
    double *xx = (double*) malloc(n*sizeof(double));
    double *bb = (double*) malloc(n*sizeof(double));

    // Construct matrix
    Ap[0] = 0;
    for (int i=0;i<n;++i) {
        int k = 0;
        if (i >= N-1) {
            Ai[Ap[i]] = i-N+1;
            Ax[Ap[i]] = -1.0;
            ++k;
        }
        if (i%(N-1) > 0) {
            Ai[Ap[i]+k] = i-1;
            Ax[Ap[i]+k] = -1.0;
            ++k;
        }
        Ai[Ap[i]+k] = i;
        Ax[Ap[i]+k] = 4.0;
        ++k;
        if ((i+1)%(N-1) > 0) {
            Ai[Ap[i]+k] = i+1;
            Ax[Ap[i]+k] = -1.0;
            ++k;
        }
        if (i < (N-2)*(N-1)) {
            Ai[Ap[i]+k] = i+N-1;
            Ax[Ap[i]+k] = -1.0;
            ++k;
        }
        Ap[i+1] = Ap[i] + k;
    }

    // Initialize vectors
    for (int i=1;i<N;++i) {
        for (int j=1;j<N;++j) {
            xx[(i-1)*(N-1) + j-1] = x[i*(N+1) + j];
            bb[(i-1)*(N-1) + j-1] = b[i*(N+1) + j];
        }
    }

    double *null = (double *) NULL ;
    void *Symbolic, *Numeric;
    (void) umfpack_di_symbolic(n,n,Ap,Ai,Ax,&Symbolic,null,null);
    (void) umfpack_di_numeric(Ap,Ai,Ax,Symbolic,&Numeric,null,null);
    umfpack_di_free_symbolic(&Symbolic);
    (void) umfpack_di_solve(UMFPACK_A,Ap,Ai,Ax,xx,bb,Numeric,null,null);
    umfpack_di_free_numeric(&Numeric);

    // Return vector
    for (int i=1;i<N;++i) {
        for (int j=1;j<N;++j) {
            x[i*(N+1) + j] = xx[(i-1)*(N-1) + j-1];
        }
    }

    free(Ap); free(Ai); free(Ax); free(xx); free(bb);
}