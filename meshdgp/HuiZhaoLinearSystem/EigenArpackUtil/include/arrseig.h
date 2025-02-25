/*
   ARPACK++ v1.2 2/20/2000
   c++ interface to ARPACK code.

   MODULE ARRSEig.h.
   Arpack++ base class ARrcStdEig definition.
   This class implements c++ version of the reverse
   communication interface for standard problems.

   ARPACK Authors
      Richard Lehoucq
      Danny Sorensen
      Chao Yang
      Dept. of Computational & Applied Mathematics
      Rice University
      Houston, Texas
*/

#ifndef ARRSEIG_H
#define ARRSEIG_H

#include <new>
#include <cstddef>

#include "arch.h"
#include "arerror.h"
#include "debug.h"
#include "blas1c.h"


// "New" handler.

void MemoryOverflow() { throw ArpackError(ArpackError::MEMORY_OVERFLOW); }

// ARrcStdEig class definition.

template<class ARFLOAT, class ARTYPE>
class ARrcStdEig {

 protected:

 // a) Protected variables:

 // a.1) User defined parameters.

  int     n;          // Dimension of the eigenproblem.
  int     nev;        // Number of eigenvalues to be computed. 0 < nev < n-1.
  int     ncv;        // Number of Arnoldi vectors generated at each iteration.
  int     maxit;      // Maximum number of Arnoldi update iterations allowed.
  char*   which;      // Specify which of the Ritz values of OP to compute.
  ARFLOAT tol;        // Stopping criterion (relative accuracy of Ritz values).
  ARFLOAT sigmaI;     // Imaginary part of shift (for nonsymmetric problems).
  ARTYPE  sigmaR;     // Shift (real part only if problem is nonsymmetric).
  ARTYPE  *resid;     // Initial residual vector.


 // a.2) Internal variables.

  bool    rvec;       // Indicates if eigenvectors/Schur vectors were
                      // requested (or only eigenvalues will be determined).
  bool    newRes;     // Indicates if a new "resid" vector was created.
  bool    newVal;     // Indicates if a new "EigValR" vector was created.
  bool    newVec;     // Indicates if a new "EigVec" vector was created.
  bool    PrepareOK;  // Indicates if internal variables were correctly set.
  bool    BasisOK;    // Indicates if an Arnoldi basis was found.
  bool    ValuesOK;   // Indicates if eigenvalues were calculated.
  bool    VectorsOK;  // Indicates if eigenvectors were determined.
  bool    SchurOK;    // Indicates if Schur vectors were determined.
  bool    AutoShift;  // Indicates if implicit shifts will be generated
                      // internally (or will be supplied by the user).
  char    bmat;       // Indicates if the problem is a standard ('I') or
                      // generalized ('G") eigenproblem.
  char    HowMny;     // Indicates if eigenvectors ('A') or Schur vectors ('P')
                      // were requested (not referenced if rvec = false).
  int     ido;        // Original ARPACK reverse communication flag.
  int     info;       // Original ARPACK error flag.
  int     mode;       // Indicates the type of the eigenproblem (regular,
                      // shift and invert, etc).
  int     lworkl;     // Dimension of array workl.
  int     lworkv;     // Dimension of array workv.
  int     lrwork;     // Dimension of array rwork.
  int     iparam[12]; // Vector that handles original ARPACK parameters.
  int     ipntr[15];  // Vector that handles original ARPACK pointers.
  ARFLOAT *rwork;     // Original ARPACK internal vector.
  ARTYPE  *workl;     // Original ARPACK internal vector.
  ARTYPE  *workd;     // Original ARPACK internal vector.
  ARTYPE  *workv;     // Original ARPACK internal vector.
  ARTYPE  *V;         // Arnoldi basis / Schur vectors.


 // a.3) Pure output variables.

  int     nconv;      // Number of "converged" Ritz values.
  ARFLOAT *EigValI;   // Imaginary part of eigenvalues (nonsymmetric problems).
  ARTYPE  *EigValR;   // Eigenvalues (real part only if problem is nonsymmetric).
  ARTYPE  *EigVec;    // Eigenvectors.


 // b) Protected functions:

 // b.1) Memory control functions.

  bool OverV() { return (EigVec == &V[1]); }
  // Indicates whether EigVec overrides V or no.

  virtual int ValSize() { return nev; }
  // Provides the size of array EigVal.
  // Redefined in ARrcNonSymStdEig.

  void ClearFirst();
  // Clears some boolean variables in order to define a entire new problem.

  void ClearBasis();
  // Clear some boolean variables in order to recalculate the arnoldi basis.

  void ClearMem();
  // Clears workspace.

  virtual void ValAllocate();
  // Creates arrays EigValR and EigValI.
  // Redefined in ARrcNonSymStdEig.

  virtual void VecAllocate(bool newV = true);
  // Creates array EigVec.

  virtual void WorkspaceAllocate();
  // Function that must be defined by a derived class.
  // Redefined in ARrc[Sym|NonSym|Complex]StdEig.


 // b.2) Functions that call the original ARPACK FORTRAN code.

  virtual void Aupp() {
    throw ArpackError(ArpackError::NOT_IMPLEMENTED, "Aupp");
  }
  // Interface to FORTRAN subroutines __AUPD.
  // Must be defined by a derived class.
  // Redefined in ARrc[Sym|NonSym|Complex]StdEig.

  void AuppError();
  // Handles errors occurred in function Aupp.

  virtual void Eupp() {
    throw ArpackError(ArpackError::NOT_IMPLEMENTED, "Eupp");
  }
  // Interface to FORTRAN subroutines __EUPD.
  // Must be defined by a derived class.
  // Redefined in ARrc[Sym|NonSym|Complex]Eig.

  void EuppError();
  // Handles errors occurred in function Eupp.


 // b.3) Functions that check user defined parameters.

  int CheckN(int np);
  // Does range checking on ncv.

  int CheckNcv(int ncvp);
  // Forces ncv to conform to its ranges.

  virtual int CheckNev(int nevp);
  // Does range checking on nev.
  // Redefined in ARrcNonSymStdEig.

  int CheckMaxit(int maxitp);
  // Forces maxit to be greater than zero.

  virtual char* CheckWhich(char* whichp);
  // Determines if the value of variable "which" is valid.
  // Redefined in ARrcSymStdEig.


 // b.4) Functions that set internal variables.

  void Restart();

  virtual void Prepare();
  // Defines internal variables and allocates working arrays.

  virtual void Copy(const ARrcStdEig& other);
  // Makes a deep copy of "other" over "this" object.
  // Old values are not deleted (this function is to be used
  // by the copy constructor and the assignment operator only).

 public:

 // c) Public functions:

 // c.1) Function that stores user defined parameters.

  virtual void DefineParameters(int np, int nevp, char* whichp="LM",
                                int ncvp=0, ARFLOAT tolp=0.0, int maxitp=0,
                                ARTYPE* residp=NULL, bool ishiftp=true);
  // Set values of problem parameters (also called by constructors).
  // Redefined in ARrcStdEig and ARrcGenEig.


 // c.2) Functions that detect if output data is ready.

  bool ParametersDefined() { return PrepareOK; }
  // Indicates if all internal variables and arrays were defined.

  bool ArnoldiBasisFound() { return BasisOK; }
  // Indicates if an Arnoldi basis is available.

  bool EigenvaluesFound() { return ValuesOK; }
  // Indicates if the requested eigenvalues are available.

  bool EigenvectorsFound() { return VectorsOK; }
  // Indicates if the requested eigenvectors are available.

  bool SchurVectorsFound() { return SchurOK; }
  // Indicates if the Schur vectors are available.


 // c.3) Functions that provides access to internal variables' values.

  int ConvergedEigenvalues() { return nconv; }
  // Provides the number of "converged" eigenvalues found so far.

  int GetMaxit() { return maxit; }
  // Returns the maximum number of Arnoldi update iterations allowed.

  int GetIter();
  // Returns the actual number of Arnoldi iterations taken.

  ARFLOAT GetTol() { return tol; }
  // Returns the stopping criterion.

  int GetN() { return n; }
  // Returns the dimension of the problem.

  int GetNev() { return nev; }
  // Returns the number of eigenvalues to be computed.

  int GetNcv() { return ncv; }
  // Returns the number of Arnoldi vectors generated at each iteration..

  char* GetWhich() { return which; }
  // Returns "which".

  ARTYPE GetShift() { return sigmaR; }
  // Returns the shift (when in shift and invert mode).

  bool GetAutoShift() { return AutoShift; }
  // Returns "AutoShift".

  int GetMode() { return mode; }
  // Returns the type of problem (regular, shift and invert, etc).


 // c.4) Functions that allow changes in problem parameters.

  void ChangeMaxit(int maxitp);
  // Changes the maximum number of Arnoldi update iterations allowed.

  void ChangeTol(ARFLOAT tolp);
  // Changes the stopping criterion.

  virtual void ChangeNev(int nevp);
  // Changes the number of eigenvalues to be computed.

  virtual void ChangeNcv(int ncvp);
  // Changes the number of Arnoldi vectors generated at each iteration..

  virtual void ChangeWhich(char* whichp);
  // Changes "which".

  virtual void ChangeShift(ARTYPE sigmaRp);
  // Turns the problem to shift-and-invert mode with shift defined by sigmaRp.
  // Redefined in many derived classes.

  virtual void NoShift();
  // Turns the problem to regular mode.
  // Redefined in ARrcGenEig.

  void InvertAutoShift();
  // Inverts "AutoShift".

  virtual void SetRegularMode() { NoShift(); }
  // Turns problem to regular mode.

  virtual void SetShiftInvertMode(ARTYPE sigmaRp) { ChangeShift(sigmaRp); }
  // Turns problem to regular mode.

 // c.5) Trace functions.

  virtual void Trace() {
    throw ArpackError(ArpackError::NOT_IMPLEMENTED, "Trace");
  }
  // Turns on trace mode.
  // Must be defined by a derived class.
  // Redefined in ARrc[Sym|NonSym|Complex]StdEig.

  void NoTrace() { TraceOff(); }
  // Turns off trace mode.


 // c.6) Functions that permit step by step execution of ARPACK.

  int GetNp() { return iparam[8]; }
  // Returns the number of shifts that must be supplied after a call to
  // TakeStep() when shifts are provided by the user (AutoShift = false).

  int GetIdo() { return ido; }
  // Indicates the type of operation the user must perform between two
  // successive calls to function TakeStep().
  // ido = -1: compute y = OP*x, where GetVector() gives a pointer to the
  //           vector x, and PutVector() indicates where to store y.
  // ido =  1: compute y = OP*x, where GetVector() gives a pointer to the
  //           vector x, and PutVector() indicates where to store y.
  //           When solving generalized problems, a pointer to the product
  //           B*x is also available by using GetProd().
  // ido =  2: compute y = B*x, where GetVector() gives a pointer to the
  //           vector x, and PutVector() indicates where to store y.
  // ido =  3: compute shifts, where PutVector() indicates where to store them.

  virtual ARTYPE* GetVector();
  // When ido = -1, 1 or 2 and the user must perform a product in the form
  // y <- M*x, this function indicates where x is stored. When ido = 3, this
  // function indicates where the eigenvalues of the current Hessenberg
  // matrix are located.

  ARTYPE* GetProd();
  // When ido = 1 and the user must perform a product in the form
  // y <- M*B*x, this function indicates where B*x is stored.

  virtual ARTYPE* PutVector();
  // When ido = -1, 1 or 2 and the user must perform a product in the form
  // y <- M*x, this function indicates where to store y. When ido = 3, this
  // function indicates where to store the shifts.
  // Redefined in ARrcSymStdEig.

  virtual int TakeStep();
  // Calls Aupp once if there is no Arnoldi basis available.


 // c.7) Functions that perform final calculations.

  virtual int FindArnoldiBasis();
  // Determines the Arnoldi basis related to the given problem.
  // This function has no meaning for this class. It is maintained
  // here for compatibility with all derived classes.
  // Redefined in ARStdEig, ARGenEig and ARSymGenEig.

  virtual int FindEigenvalues();
  // Determines nev approximated eigenvalues of the given eigen-problem.
  // Redefined in ARNonSymGenEig.

  virtual int FindEigenvectors(bool schurp = false);
  // Determines nev approximated eigenvectors of the given eigen-problem
  // Optionally also determines nev Schur vectors that span the desired
  // invariant subspace.
  // Redefined in ARNonSymGenEig.

  virtual int FindSchurVectors();
  // Determines nev Schur vectors that span the desired invariant subspace.
  // Redefined in ARrcSymStdEig and ARNonSymGenEig.


 // c.8) Function that perform calculations using user supplied data structure.

  virtual int Eigenvectors(ARTYPE* &EigVecp, bool ischur = false);
  // Overrides array EigVecp sequentially with the eigenvectors of the
  // given eigen-problem. Also calculates Schur vectors if requested.


 // c.9) Functions that return elements of vectors and matrices.

  ARTYPE ArnoldiBasisVector(int i, int j);
  // Furnishes element j of the i-eth Arnoldi basis vector.

  ARTYPE SchurVector(int i, int j);
  // Furnishes element j of the i-eth Schur vector.

  ARTYPE ResidualVector(int i);
  // Furnishes element j of the residual vector.


 // c.10) Functions that provide raw access to internal vectors and matrices.

  ARTYPE* RawArnoldiBasisVectors();
  // Provides raw access to Arnoldi basis vectors elements.

  ARTYPE* RawArnoldiBasisVector(int i);
  // Provides raw access to Arnoldi basis vector i.

  ARTYPE* RawEigenvalues();
  // Provides raw access to eigenvalues.

  ARTYPE* RawEigenvectors();
  // Provides raw access to eigenvectors elements.

  ARTYPE* RawEigenvector(int i);
  // Provides raw access to eigenvector i.

  ARTYPE* RawSchurVectors();
  // Provides raw access to Schur vectors elements.

  ARTYPE* RawSchurVector(int i);
  // Provides raw access to Schur vector i.

  ARTYPE* RawResidualVector();
  // Provides raw access to residual vector elements.


 // c.11) Functions that use STL vector class.

#ifdef STL_VECTOR_H

  vector<ARTYPE>* StlArnoldiBasisVectors();
  // Returns a copy of the Arnoldi basis in a single STL vector.

  vector<ARTYPE>* StlArnoldiBasisVector(int i);
  // Returns the i-th Arnoldi basis vector in a STL vector.

  vector<ARTYPE>* StlEigenvectors(bool ischur = false);
  // Calculates the eigenvectors and stores them sequentially in a
  // single STL vector. Also calculates Schur vectors if requested.

  vector<ARTYPE>* StlSchurVectors();
  // Returns a copy of the Schur vectors in a single STL vector.

  vector<ARTYPE>* StlSchurVector(int i);
  // Returns the i-th Schur vector in a STL vector.

  vector<ARTYPE>* StlResidualVector();
  // Returns the residual vector in a STL vector.

#endif // #ifdef STL_VECTOR_H.


 // c.12) Constructors and destructor.

  ARrcStdEig();
  // Short constructor that does almost nothing.

  ARrcStdEig(const ARrcStdEig& other) { Copy(other); }
  // Copy constructor.

  virtual ~ARrcStdEig() { ClearMem(); }
  // Very simple destructor.

 // d) Operators:

  ARrcStdEig& operator=(const ARrcStdEig& other);
  // Assignment operator.

}; // class ARrcStdEig.


// ------------------------------------------------------------------------ //
// ARrcStdEig member functions definition.                                  //
// ------------------------------------------------------------------------ //


template<class ARFLOAT, class ARTYPE>
inline void ARrcStdEig<ARFLOAT, ARTYPE>::ClearFirst()
{

  PrepareOK = newVal = newVec = newRes = false;

} // ClearFirst.


template<class ARFLOAT, class ARTYPE>
inline void ARrcStdEig<ARFLOAT, ARTYPE>::ClearBasis()
{

  BasisOK = ValuesOK = VectorsOK = SchurOK = false;

} // ClearBasis.


template<class ARFLOAT, class ARTYPE>
void ARrcStdEig<ARFLOAT, ARTYPE>::ClearMem()
{
 
  // Deleting working arrays.
    
  delete[] workl;
  delete[] workd;
  delete[] workv;
  delete[] rwork;
  delete[] V;

  workl = NULL;
  workd = NULL;
  workv = NULL;
  rwork = NULL;
  V     = NULL;

  // Deleting input and output arrays.

  if (newRes) {
    delete[] resid;
    newRes = false;
    resid = NULL;   // Salwen. Mar 3, 2000.
  }

  if (newVal) {
    delete[] EigValR;
    delete[] EigValI;
    newVal = false;
  }
  EigValR=NULL;
  EigValI=NULL;

  if (newVec) {
    delete[] EigVec;
    newVec = false;
  }
  EigVec=NULL;

  // Adjusting boolean variables.

  ClearFirst();
  
} // ClearMem.


template<class ARFLOAT, class ARTYPE>
inline void ARrcStdEig<ARFLOAT, ARTYPE>::ValAllocate()
{

  if (EigValR == NULL) {              // Creating a new array EigValR.
    EigValR = new ARTYPE[ValSize()];
    newVal = true;
  }

} // ValAllocate.

template<class ARFLOAT, class ARTYPE>
inline void ARrcStdEig<ARFLOAT, ARTYPE>::VecAllocate(bool newV)
{

  if (EigVec == NULL) {
    if (newV) {                       // Creating a new array EigVec.
      EigVec = new ARTYPE[ValSize()*n];
      newVec = true;
    }
    else {                            // Using V to store EigVec.
      EigVec = &V[1];
    }
  }

} // VecAllocate.


template<class ARFLOAT, class ARTYPE>
void ARrcStdEig<ARFLOAT, ARTYPE>::WorkspaceAllocate()
{

  lworkl = 0;
  lworkv = 0;
  lrwork = 0;

} // WorkspaceAllocate.


template<class ARFLOAT, class ARTYPE>
void ARrcStdEig<ARFLOAT, ARTYPE>::AuppError()
{

  switch (info) {
  case     0:
    return;
  case    -8:
    throw ArpackError(ArpackError::LAPACK_ERROR, "Aupp");
  case    -9:
    throw ArpackError(ArpackError::START_RESID_ZERO, "Aupp");
  case -9999:
    throw ArpackError(ArpackError::ARNOLDI_NOT_BUILD, "Aupp");
  case     1:
    ArpackError(ArpackError::MAX_ITERATIONS, "Aupp");
    return;
  case     3:
    ArpackError(ArpackError::NO_SHIFTS_APPLIED, "Aupp");
    return;
  default   :
    throw ArpackError(ArpackError::AUPP_ERROR, "Aupp");
  }

} // AuppError.


template<class ARFLOAT, class ARTYPE>
void ARrcStdEig<ARFLOAT, ARTYPE>::EuppError()
{

  switch (info) {
  case   0:
    return;
  case  -8:
  case  -9:
    throw ArpackError(ArpackError::LAPACK_ERROR, "Eupp");
  case -14:
    throw ArpackError(ArpackError::NOT_ACCURATE_EIG, "Eupp");
  case   1:
    throw ArpackError(ArpackError::REORDERING_ERROR, "Eupp");
  default :
    throw ArpackError(ArpackError::EUPP_ERROR, "Eupp");
  }

} // EuppError.


template<class ARFLOAT, class ARTYPE>
inline int ARrcStdEig<ARFLOAT, ARTYPE>::CheckN(int np)
{

  if (np < 2) {
    throw ArpackError(ArpackError::N_SMALLER_THAN_2);
  }
  return np;

} // CheckN.


template<class ARFLOAT, class ARTYPE>
inline int ARrcStdEig<ARFLOAT, ARTYPE>::CheckNcv(int ncvp)
{

  // Adjusting ncv if ncv <= nev or ncv > n.

  if (ncvp < nev+1) {
    if (ncvp) ArpackError::Set(ArpackError::NCV_OUT_OF_BOUNDS);
    return ((2*nev+1)>n)?n:(2*nev+1);
  }
  else if (ncvp > n) {
    ArpackError::Set(ArpackError::NCV_OUT_OF_BOUNDS);
    return n;
  }
  else {
    return ncvp;
  }

} // CheckNcv.


template<class ARFLOAT, class ARTYPE>
inline int ARrcStdEig<ARFLOAT, ARTYPE>::CheckNev(int nevp)
{

  if ((nevp<1)||(nevp>=n)) { 
    throw ArpackError(ArpackError::NEV_OUT_OF_BOUNDS);
  }
  return nevp;

} // CheckNev.


template<class ARFLOAT, class ARTYPE>
inline int ARrcStdEig<ARFLOAT, ARTYPE>::CheckMaxit(int maxitp)
{

  if (maxitp >= 1) return maxitp;
  if (maxitp < 0)  ArpackError::Set(ArpackError::MAXIT_NON_POSITIVE);
  return 100*nev;

} // CheckMaxit.

template<class ARFLOAT, class ARTYPE>
char* ARrcStdEig<ARFLOAT, ARTYPE>::CheckWhich(char* whichp)
{

  switch (whichp[0]) {              // The first ought to be S or L.
  case 'S':
  case 'L':
    switch (whichp[1]) {            // The second must be M, R or I.
    case 'M':
    case 'R':
    case 'I':
      return whichp;
    }
  default :
    throw ArpackError(ArpackError::WHICH_UNDEFINED);
  }

} // CheckWhich.


template<class ARFLOAT, class ARTYPE>
void ARrcStdEig<ARFLOAT, ARTYPE>::Restart()
{

  nconv=0;                  // No eigenvalues found yet.
  ido  =0;                  // First call to AUPP.
  iparam[1]=(int)AutoShift; // Shift strategy used.
  iparam[3]=maxit;          // Maximum number of Arnoldi iterations allowed.
  iparam[4]=1;              // Blocksize must be 1.
  info =(int)(!newRes);     // Starting vector used.
  ClearBasis();

} // Restart.


template<class ARFLOAT, class ARTYPE>
void ARrcStdEig<ARFLOAT, ARTYPE>::Prepare()
{

  // Deleting old stuff.

  ClearMem();

  // Defining internal variables.

  try {

    if (resid == NULL) {       // Using a random starting vector.
      resid  = new ARTYPE[n];
      newRes = true;
    }

    // Setting dimensions of working arrays.

    workd    = new ARTYPE[3*n+1];
    V        = new ARTYPE[n*ncv+1];
    WorkspaceAllocate();

  }
  catch (ArpackError) {    // Returning from here if an error has occurred.
    ArpackError(ArpackError::CANNOT_PREPARE, "Prepare");
    return;
  }

  Restart();

  // That's all.

  PrepareOK = true;

} // Prepare.


template<class ARFLOAT, class ARTYPE>
void ARrcStdEig<ARFLOAT,ARTYPE>::Copy(const ARrcStdEig<ARFLOAT,ARTYPE>& other)
{

  // Defining local variables.

  int i;

  // Copying variables that belong to fundamental types.

  n         = other.n;
  nev       = other.nev;
  ncv       = other.ncv;
  maxit     = other.maxit;
  which     = other.which;
  tol       = other.tol;
  sigmaI    = other.sigmaI;
  sigmaR    = other.sigmaR;
  rvec      = other.rvec;
  newRes    = other.newRes;
  newVal    = other.newVal;
  newVec    = other.newVec;
  PrepareOK = other.PrepareOK;
  BasisOK   = other.BasisOK;
  ValuesOK  = other.ValuesOK;
  VectorsOK = other.VectorsOK;
  SchurOK   = other.SchurOK;
  AutoShift = other.AutoShift;
  bmat      = other.bmat;
  HowMny    = other.HowMny;
  ido       = other.ido;
  info      = other.info;
  mode      = other.mode;
  nconv     = other.nconv;

  // Copying arrays with static dimension.

  for (i=0; i<12; i++) iparam[i] = other.iparam[i];
  for (i=0; i<15; i++) ipntr[i]  = other.ipntr[i];

  // Returning from here if "other" was not initialized.

  if (!PrepareOK) return;

  // Copying dynamic variables.

  workd     = new ARTYPE[3*n+1];       // workd.
  copy(3*n+1,other.workd,1,workd,1);

  V         = new ARTYPE[n*ncv+1];     // V.
  copy(n*ncv+1,other.V,1,V,1);

  if (newRes) {                        // resid.
    resid   = new ARTYPE[n];
    copy(n,other.resid,1,resid,1);
  }
  else {
    resid   = other.resid;
  }

  if (newVec) {                        // EigVec.
    EigVec  = new ARTYPE[ValSize()*n];
    copy(ValSize()*n,other.EigVec,1,EigVec,1);
  }
  else if (other.EigVec == (&other.V[1])) {
    EigVec  = &V[1];
  }
  else {
    EigVec  = other.EigVec;
  }

  if (newVal) {                        // EigValR and EigValI.
    EigValR = new ARTYPE[ValSize()];
    copy(ValSize(),other.EigValR,1,EigValR,1);
    if (other.EigValI != NULL) {
      EigValI = new ARFLOAT[ValSize()];
      copy(ValSize(),other.EigValI,1,EigValI,1);
    }
    else {
      EigValI = NULL;
    }
  }
  else {
    EigValR = other.EigValR;
    EigValI = other.EigValI;
  }

  WorkspaceAllocate();               // lworkl, workl, workv and rwork.
  if (lworkl) copy(lworkl+1,other.workl,1,workl,1);
  if (lworkv) copy(lworkv+1,other.workv,1,workv,1);
  if (lrwork) copy(lrwork+1,other.rwork,1,rwork,1);

} // Copy.


template<class ARFLOAT, class ARTYPE>
void ARrcStdEig<ARFLOAT, ARTYPE>::
DefineParameters(int np, int nevp, char* whichp, int ncvp,
                 ARFLOAT tolp, int maxitp, ARTYPE* residp, bool ishiftp)

{

  // Providing a "new" handler.

  std::set_new_handler ( MemoryOverflow );

  // Setting user defined parameters.

  try {
    n         = CheckN(np);
    nev       = CheckNev(nevp);
    ncv       = CheckNcv(ncvp);
    which     = CheckWhich(whichp);
    maxit     = CheckMaxit(maxitp);
    tol       = tolp;
    resid     = residp;
    AutoShift = ishiftp;
  }

  // Returning from here if an error has occurred.

  catch (ArpackError) {
    ArpackError(ArpackError::PARAMETER_ERROR, "DefineParameter");
    return;
  }

  // Setting internal variables.

  ClearFirst();
  Prepare();

} // DefineParameters.


template<class ARFLOAT, class ARTYPE>
int ARrcStdEig<ARFLOAT, ARTYPE>::GetIter()
{

  if (BasisOK || ValuesOK || VectorsOK || SchurOK) {
    return iparam[3];
  }
  else {
    return 0;
  }

} // GetIter.


template<class ARFLOAT, class ARTYPE>
inline void ARrcStdEig<ARFLOAT, ARTYPE>::ChangeMaxit(int maxitp)
{

  maxit = CheckMaxit(maxitp);
  Restart();

} // ChangeMaxit.


template<class ARFLOAT, class ARTYPE>
inline void ARrcStdEig<ARFLOAT, ARTYPE>::ChangeTol(ARFLOAT tolp)
{

  if (tolp < tol) Restart();
  tol = tolp;

} // ChangeTol.


template<class ARFLOAT, class ARTYPE>
void ARrcStdEig<ARFLOAT, ARTYPE>::ChangeNev(int nevp)
{

  try { 
    nev = CheckNev(nevp); 
    ncv = CheckNcv(ncv);
  }
  catch (ArpackError) { return; }
  if (PrepareOK) Prepare();

} // ChangeNev.


template<class ARFLOAT, class ARTYPE>
inline void ARrcStdEig<ARFLOAT, ARTYPE>::ChangeNcv(int ncvp)
{

  ncv = CheckNcv(ncvp);
  if (PrepareOK) Prepare();

} // ChangeNcv.


template<class ARFLOAT, class ARTYPE>
void ARrcStdEig<ARFLOAT, ARTYPE>::ChangeWhich(char* whichp)
{

  try { which = CheckWhich(whichp); }
  catch (ArpackError) { return; }
  Restart();

} // ChangeWhich.


template<class ARFLOAT, class ARTYPE>
inline void ARrcStdEig<ARFLOAT, ARTYPE>::ChangeShift(ARTYPE sigmaRp)
{

  sigmaR    = sigmaRp;
  sigmaI    = 0.0;
  mode      = 3;
  iparam[7] = mode;
  Restart();

} // ChangeShift.


template<class ARFLOAT, class ARTYPE>
inline void ARrcStdEig<ARFLOAT, ARTYPE>::NoShift()
{

  sigmaR    = (ARTYPE)0;
  sigmaI    = 0.0;
  mode      = 1;
  iparam[7] = mode;
  Restart();

} // NoShift.


template<class ARFLOAT, class ARTYPE>
void ARrcStdEig<ARFLOAT, ARTYPE>::InvertAutoShift()
{

  AutoShift = !AutoShift;
  Restart();

} // InvertAutoShift.


template<class ARFLOAT, class ARTYPE>
ARTYPE* ARrcStdEig<ARFLOAT, ARTYPE>::GetVector()
{

  switch (ido) {
  case -1:
  case  1:
  case  2:
    return &workd[ipntr[1]];
  case  3:
    return &workl[ipntr[6]];
  default:
    throw ArpackError(ArpackError::CANNOT_GET_VECTOR, "GetVector");
  }

} // GetVector.


template<class ARFLOAT, class ARTYPE>
ARTYPE* ARrcStdEig<ARFLOAT, ARTYPE>::GetProd()
{

  if (ido != 1) {
    throw ArpackError(ArpackError::CANNOT_GET_PROD, "GetProd");
  }
  return &workd[ipntr[3]];

} // GetProd.


template<class ARFLOAT, class ARTYPE>
ARTYPE* ARrcStdEig<ARFLOAT, ARTYPE>::PutVector()
{

  switch (ido) {
  case -1:
  case  1:
  case  2:
    return &workd[ipntr[2]];
  case  3:
    return &workl[ipntr[14]];
  default:
    throw ArpackError(ArpackError::CANNOT_PUT_VECTOR, "PutVector");
  }

} // PutVector.


template<class ARFLOAT, class ARTYPE>
int ARrcStdEig<ARFLOAT, ARTYPE>::TakeStep()
{

  // Requiring the definition of all internal variables.

  if (!PrepareOK) {

    throw ArpackError(ArpackError::PREPARE_NOT_OK, "TakeStep");

  }
  else if (!BasisOK) {

    // Taking a step if the Arnoldi basis is not available.

    Aupp();

    // Checking if convergence was obtained.

    if (ido==99) {
      nconv = iparam[5];
      AuppError();
      if (info >= 0) BasisOK = true;
    }
  }

  return ido;

} // TakeStep.


template<class ARFLOAT, class ARTYPE>
inline int ARrcStdEig<ARFLOAT, ARTYPE>::FindArnoldiBasis()
{

  if (!BasisOK) {
    throw ArpackError(ArpackError::CANNOT_FIND_BASIS, "FindArnoldiBasis");
  }
  return nconv;

} // FindArnoldiBasis.


template<class ARFLOAT, class ARTYPE>
int ARrcStdEig<ARFLOAT, ARTYPE>::FindEigenvalues()
{

  // Determining eigenvalues if they are not available.

  if (!ValuesOK) {
    try {
      ValAllocate();
      nconv = FindArnoldiBasis();
      rvec  = false;
      if (nconv>0) {
        Eupp();
        EuppError();
      }
    }
    catch (ArpackError) {
      ArpackError(ArpackError::CANNOT_FIND_VALUES, "FindEigenvalues");
      return 0;
    }
    if (newVal) ValuesOK = true;
  }
  return nconv;

} // FindEigenvalues.


template<class ARFLOAT, class ARTYPE>
int ARrcStdEig<ARFLOAT, ARTYPE>::FindEigenvectors(bool schurp)
{

  // Determining eigenvectors if they are not available.

  if (!VectorsOK) {
    try {
      ValAllocate();
      VecAllocate(schurp);
      nconv = FindArnoldiBasis();
      rvec  = true;
      HowMny = 'A';
      if (nconv>0) {
        Eupp();
        EuppError();
      }
    }
    catch (ArpackError) {
      ArpackError(ArpackError::CANNOT_FIND_VECTORS, "FindEigenvectors");
      return 0;
    }
    BasisOK = false;
    if (newVal) ValuesOK = true;
    if (newVec || OverV()) VectorsOK = true;
    if (!OverV()) SchurOK = true;
  }
  return nconv;

} // FindEigenvectors.


template<class ARFLOAT, class ARTYPE>
int ARrcStdEig<ARFLOAT, ARTYPE>::FindSchurVectors()
{

  // Determining Schur vectors if they are not available.

  if (!SchurOK) {
    try {
      ValAllocate();
      nconv  = FindArnoldiBasis();
      rvec   = true;
      HowMny = 'P';
      if (nconv>0) {
        Eupp();
        EuppError();
      }
    }
    catch (ArpackError) {
      ArpackError(ArpackError::CANNOT_FIND_SCHUR, "FindSchurVectors");
      return 0;
    }
    BasisOK = false;
    if (newVal) ValuesOK = true;
    SchurOK =true;
  }
  return nconv;

} // FindSchurVectors.


template<class ARFLOAT, class ARTYPE>
int ARrcStdEig<ARFLOAT, ARTYPE>::
Eigenvectors(ARTYPE* &EigVecp, bool ischur)
{

  // Overriding EigVecp with the converged eigenvectors.

  if (VectorsOK) {                       // Eigenvectors are available.
    if ((EigVecp == NULL) && (newVec)) { // Moving eigenvectors.
      EigVecp   = EigVec;
      EigVec    = NULL;
      newVec    = false;
      VectorsOK = false;
    }
    else {                               // Copying eigenvectors.
      if (EigVecp == NULL) {
        try { EigVecp = new ARTYPE[ValSize()*n]; }
        catch (ArpackError) { return 0; }
      }
      copy(ValSize()*n,EigVec,1,EigVecp,1);
    }
  }
  else {                                // Eigenvectors are not available.
    if (newVec) {
      delete[] EigVec;
      newVec = false;
    }
    if (EigVecp == NULL) {
      try { EigVecp = new ARTYPE[ValSize()*n]; }
      catch (ArpackError) { return 0; }
    }
    EigVec = EigVecp;
    nconv  = FindEigenvectors(ischur);
    EigVec = NULL;
  }
  return nconv;

} // Eigenvectors(EigVecp, ischur).


template<class ARFLOAT, class ARTYPE>
inline ARTYPE ARrcStdEig<ARFLOAT, ARTYPE>::ArnoldiBasisVector(int i, int j)
{

  // Returning element j of Arnoldi basis vector i.

  if (!BasisOK) {
    throw ArpackError(ArpackError::BASIS_NOT_OK, "ArnoldiBasisVector(i,j)");
  }
  else if ((i>=ncv)||(i<0)||(j>=n)||(j<0)) {
    throw ArpackError(ArpackError::RANGE_ERROR,"ArnoldiBasisVector(i,j)");
  }
  return V[i*n+j+1];

} // ArnoldiBasisVector(i,j).


template<class ARFLOAT, class ARTYPE>
inline ARTYPE ARrcStdEig<ARFLOAT, ARTYPE>::SchurVector(int i, int j)
{

  // Returning element j of Schur vector i.

  if (!SchurOK) {
    throw ArpackError(ArpackError::SCHUR_NOT_OK, "SchurVector(i,j)");
  }
  else if ((i>=nconv)||(i<0)||(j>=n)||(j<0)) {
    throw ArpackError(ArpackError::RANGE_ERROR, "SchurVector(i,j)");
  }
  return V[i*n+j+1];

} // SchurVector(i,j).


template<class ARFLOAT, class ARTYPE>
inline ARTYPE ARrcStdEig<ARFLOAT, ARTYPE>::ResidualVector(int i)
{

  // Returning element i of the residual vector.

  if ((!newRes)||(!(BasisOK||ValuesOK||VectorsOK||SchurOK))) {
    throw ArpackError(ArpackError::RESID_NOT_OK, "ResidualVector(i)");
  }
  else if ((i>=n)||(i<0)) {
    throw ArpackError(ArpackError::RANGE_ERROR, "ResidualVector(i)");
  }
  return resid[i];

} // ResidualVector(i).


template<class ARFLOAT, class ARTYPE>
inline ARTYPE* ARrcStdEig<ARFLOAT, ARTYPE>::RawArnoldiBasisVectors()
{

  // Returning a constant pointer to Arnoldi basis.

  if (!BasisOK) {
    throw ArpackError(ArpackError::BASIS_NOT_OK, "RawArnoldiBasisVectors");
  }
  return &V[1];

} // RawArnoldiBasisVectors.


template<class ARFLOAT, class ARTYPE>
inline ARTYPE* ARrcStdEig<ARFLOAT, ARTYPE>::RawArnoldiBasisVector(int i)
{

  // Returning a constant pointer to Arnoldi basis vector i.

  if (!BasisOK) {
    throw ArpackError(ArpackError::BASIS_NOT_OK, "RawArnoldiBasisVector(i)");
  }
  else if ((i>=ncv)||(i<0)) {
    throw ArpackError(ArpackError::RANGE_ERROR,"RawArnoldiBasisVector(i)");
  }
  return &V[i*n+1];

} // RawArnoldiBasisVector(i).


template<class ARFLOAT, class ARTYPE>
inline ARTYPE* ARrcStdEig<ARFLOAT, ARTYPE>::RawEigenvalues()
{

  if (!ValuesOK) {
    throw ArpackError(ArpackError::VALUES_NOT_OK, "RawEigenvalues");
  }
  return EigValR;

} // RawEigenvalues.


template<class ARFLOAT, class ARTYPE>
inline ARTYPE* ARrcStdEig<ARFLOAT, ARTYPE>::RawEigenvectors()
{

  if (!VectorsOK) {
    throw ArpackError(ArpackError::VECTORS_NOT_OK, "RawEigenvectors");
  }
  return EigVec;

} // RawEigenvectors.


template<class ARFLOAT, class ARTYPE>
inline ARTYPE* ARrcStdEig<ARFLOAT, ARTYPE>::RawEigenvector(int i)
{

  // Returning a constant pointer to eigenvector i.

  if (!VectorsOK) {
    throw ArpackError(ArpackError::VECTORS_NOT_OK, "RawEigenvector(i)");
  }
  else if ((i>=ValSize())||(i<0)) {
    throw ArpackError(ArpackError::RANGE_ERROR, "RawEigenvector(i)");
  }
  return &EigVec[i*n];

} // RawEigenvector(i).


template<class ARFLOAT, class ARTYPE>
inline ARTYPE* ARrcStdEig<ARFLOAT, ARTYPE>::RawSchurVectors()
{

  if (!SchurOK) {
    throw ArpackError(ArpackError::SCHUR_NOT_OK, "RawSchurVectors");
  }
  return &V[1];

} // RawSchurVectors.


template<class ARFLOAT, class ARTYPE>
inline ARTYPE* ARrcStdEig<ARFLOAT, ARTYPE>::RawSchurVector(int i)
{

  // Returning a constant pointer to Schur vector i.

  if (!SchurOK) {
    throw ArpackError(ArpackError::SCHUR_NOT_OK, "RawSchurVector(i)");
  }
  else if ((i>=nev)||(i<0)) {
    throw ArpackError(ArpackError::RANGE_ERROR, "RawSchurVector(i)");
  }
  return &V[i*n+1];

} // RawSchurVector(i).


template<class ARFLOAT, class ARTYPE>
inline ARTYPE* ARrcStdEig<ARFLOAT, ARTYPE>::RawResidualVector()
{

  if (!newRes) {
    throw ArpackError(ArpackError::RESID_NOT_OK, "RawResidualVector");
  }
  return resid;

} // RawResidualVector.


#ifdef STL_VECTOR_H // Defining some functions that use STL vector class.

template<class ARFLOAT, class ARTYPE>
inline vector<ARTYPE>* ARrcStdEig<ARFLOAT, ARTYPE>::StlArnoldiBasisVectors()
{

  // Returning the Arnoldi basis in a single STL vector.

  vector<ARTYPE>* StlBasis;

  if (!BasisOK) {
    nconv = FindArnoldiBasis();
  }
  try {
    StlBasis = new vector<ARTYPE>(&V[1], &V[n*ncv+1]);
  }
  catch (ArpackError) { return NULL; }
  return StlBasis;

} // StlArnoldiBasisVectors.


template<class ARFLOAT, class ARTYPE>
inline vector<ARTYPE>* ARrcStdEig<ARFLOAT,ARTYPE>::StlArnoldiBasisVector(int i)
{

  // Returning the i-th Arnoldi basis vector in a STL vector.

  vector<ARTYPE>* StlBasis;

  if (!BasisOK) {
    throw ArpackError(ArpackError::BASIS_NOT_OK, "StlArnoldiBasisVector(i)");
  }
  else if ((i>=ncv)||(i<0)) {
    throw ArpackError(ArpackError::RANGE_ERROR,"StlArnoldiBasisVector(i)");
  }
  try {
    StlBasis = new vector<ARTYPE>(&V[i*n+1], &V[(i+1)*n+1]);
  }
  catch (ArpackError) { return NULL; }
  return StlBasis;

} // StlArnoldiBasisVector(i).


template<class ARFLOAT, class ARTYPE>
inline vector<ARTYPE>* ARrcStdEig<ARFLOAT,ARTYPE>::StlEigenvectors(bool ischur)
{

  // Returning the eigenvector in a single STL vector.

  vector<ARTYPE>* StlEigVec;
  ARTYPE*         VecPtr;

  try { StlEigVec = new vector<ARTYPE>(ValSize()*n); }
  catch (ArpackError) { return NULL; }
  VecPtr = StlEigVec->begin();
  nconv  = Eigenvectors(VecPtr, ischur);
  return StlEigVec;

} // StlEigenvectors.


template<class ARFLOAT, class ARTYPE>
inline vector<ARTYPE>* ARrcStdEig<ARFLOAT, ARTYPE>::StlSchurVectors()
{

  vector<ARTYPE>* StlSchurVec;

  if (!SchurOK) {
    nconv = FindSchurVectors();
  } 
  try {
    StlSchurVec = new vector<ARTYPE>(&V[1], &V[nev*n+1]);
  }
  catch (ArpackError) { return NULL; }
  return StlSchurVec;

} // StlSchurVectors.


template<class ARFLOAT, class ARTYPE>
inline vector<ARTYPE>* ARrcStdEig<ARFLOAT, ARTYPE>::StlSchurVector(int i)
{

  // Returning the i-th Schur vector in a STL vector.

  vector<ARTYPE>* StlSchurVec;

  if (!SchurOK) {
    throw ArpackError(ArpackError::SCHUR_NOT_OK, "StlSchurVector(i)");
  }
  else if ((i>=nev)||(i<0)) {
    throw ArpackError(ArpackError::RANGE_ERROR, "StlSchurVector(i)");
  }
  try {
    StlSchurVec = new vector<ARTYPE>(&V[i*n+1], &V[(i+1)*n+1]);
  }
  catch (ArpackError) { return NULL; }
  return StlSchurVec;

} // StlSchurVector(i).


template<class ARFLOAT, class ARTYPE>
inline vector<ARTYPE>* ARrcStdEig<ARFLOAT, ARTYPE>::StlResidualVector()
{

  // Returning the residual vector in a STL vector.

  vector<ARTYPE>* StlResid;

  if (!newRes) {
    throw ArpackError(ArpackError::RESID_NOT_OK, "StlResidualVector");
  }
  try {
    StlResid = new vector<ARTYPE>(resid, &resid[n]);
  }
  catch (ArpackError) { return NULL; }
  return StlResid;

} // StlResidualVector.

#endif // #ifdef STL_VECTOR_H.


template<class ARFLOAT, class ARTYPE>
inline ARrcStdEig<ARFLOAT, ARTYPE>::ARrcStdEig()
{

  resid   = NULL;
  rwork   = NULL;
  workl   = NULL;
  workd   = NULL;
  workv   = NULL;
  V       = NULL;
  EigValR = NULL;
  EigValI = NULL;
  EigVec  = NULL;
  bmat    = 'I';   // This is a standard problem.
  ClearFirst();
  NoShift();
  NoTrace();

} // Short constructor.


template<class ARFLOAT, class ARTYPE>
ARrcStdEig<ARFLOAT, ARTYPE>& ARrcStdEig<ARFLOAT, ARTYPE>::
operator=(const ARrcStdEig<ARFLOAT, ARTYPE>& other)
{

  if (this != &other) { // Stroustrup suggestion.
    ClearMem();
    Copy(other);
  }
  return *this;

} // operator=.


#endif // ARRSEIG_H

