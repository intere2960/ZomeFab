
#ifdef WITH_CXSPARSE
	#include <Include/cs.h>
#endif

#ifndef NUM_DBL_MAX
	#define NUM_DBL_MAX 1E30
#endif

class LinAlgComplex;

////#ifdef __APPLE__
////	//http://developer.apple.com/library/mac/#documentation/Accelerate/Reference/BLAS_Ref/Reference/reference.html
////	//#include <Accelerate/../Frameworks/vecLib.framework/Versions/A/Headers/cblas.h>
////	#include <vecLib/cblas.h>
////#else
//	#include "cblas.h"
////#endif

extern "C" void dgetrf_(int* n1, int * n2, double* a, int* lda, int * ipvt, int * info);
extern "C" void dgetri_(int* n, double* a, int* lda, int * ipvt, 
						double * work, int * lwork, int * info);
extern "C" void   dgesvd_(char* jobu, char* jobvt, int* m, int* n, double* a, int* lda, 
						  double* s, double* u, int* ldu, double* vt, int* ldvt, double* work, 
						  int* lwork, int* info);
extern "C" void   dgesv_(int *n, int *nrhs, double *a, int *lda, int *ipiv, 
						 double *b, int *ldb, int *info);

extern "C" void dgels_(char* trans, int* m, int* n, int* nrhs, double* a, int* lda,
					   double* b, int* ldb, double* work, int* lwork, int* info );

extern "C" void dpbtrf_(char uplo, int n, int kd, double  *a,  int  lda, int *info);
extern "C" void dpbtrs_(char uplo, int n, int kd, int nrhs, double * a, int lda, 
						double * b, int ldb, int * info);
extern "C" void dpftrf_(char transfr, char uplo, int n, double  *a,  int *info);
extern "C" void dpftrs_(char transfr, char uplo, int n, int nrhs, double * A, 
						double * B, int ldb, int *info);

extern "C" void dpotrf_(char uplo, int n, double  *a,  int lda, int *info);
extern "C" void dpotrs_(char uplo, int n, int nrhs, double * A, int lda, 
						double * B, int ldb, int *info);

// NOTE: there are several lapack options for this function, see benchmark:
//			http://www.netlib.org/lapack/lug/node71.html
extern "C" void dsyev_( char* jobz, char* uplo, int* n, double* a, int* lda,
					   double* w, double* work, int* lwork, int* info );

extern "C" void dggev_( char* jobz, char* uplo, int* n, double* a, int* lda,
					   double* w, double* work, int* lwork, int* info );

extern "C" void zgesv_(int *n, int *nrhs, LinAlgComplex *a, int *lda, int *ipiv, 
					   LinAlgComplex *b, int *ldb, int *info);

extern "C" void zgels_(char* trans, int* m, int* n, int* nrhs, LinAlgComplex* a,
					   int* lda, LinAlgComplex* b, int* ldb, LinAlgComplex* work, 
					   int* lwork, int* info );
extern "C" void zgesvd_( char* jobu, char* jobvt, int* m, int* n, LinAlgComplex* a,
						int* lda, double* s, LinAlgComplex * u, int* ldu, LinAlgComplex* vt, 
						int* ldvt, LinAlgComplex* work, int* lwork, double* rwork, int* info );

extern "C" void zgetrf_(int* n1, int * n2, LinAlgComplex* a, int* lda, int * ipvt, int * info);
extern "C" void zgetri_(int* n, LinAlgComplex* a, int* lda, int * ipvt, 
						LinAlgComplex * work, int * lwork, int * info);



#include <iostream>
#include <assert.h>
#include <vector>
#include <map>
#include <string.h>
#include <math.h>
#include "VkFunctions.h"

#ifndef __LINALG_H
#define __LINALG_H

class LinAlg
{
	public:
		enum MatrixFactorizationType
		{
			MAT_FACT_NONE,
			MAT_FACT_QR,
			MAT_FACT_CHOLESKY,
			MAT_FACT_CHOLESKY_SYMM
		};
	
//		enum SVDMethod
//		{
//			SVD_AUTO,
//			SVD_REGULAR, 
//			SVD_GOLUB_REINSCH,		// use if M>>N
//			SVD_JACOBI				// use if M>>N and higher accuracy is desired
//		};
//		enum InverseMethod
//		{
//			INV_AUTO,
//			INV_LU,
//			INV_SVD,
//			INV_CHOLESKY
//		};
		static bool RunMatlabScript(const char * matlabScriptName,
								const char * dataPrefix);
		static void WaitForFile(const char * fileName, 
								int checkEveySeconds, 
								int maxSeconds);
	
		static bool FileExists(const char * filename);
	
		static std::string GetMatlabCMD(const char * matlabCommand);
		static bool RunMatlab(const char * matlabCommand, 
							  const char * externalMonitorDir = NULL);
		static char * m_sDefaultExternalMonitorDir;
};

#endif

