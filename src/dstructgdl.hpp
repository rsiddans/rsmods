/***************************************************************************
                          dstructgdl.hpp  -  GDL struct datatype
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef dstructgdl_hpp_
#define dstructgdl_hpp_

#include <string>
//#include <valarray>

#include "typedefs.hpp"
#include "datatypes.hpp" // for friend declaration
//#include "typetraits.hpp"
#include "dstructdesc.hpp"

// NOTE: lot of the code together with Data_<...>::functions
// does not fit really into the template because of different
// constructor
// also here a vector is used instead of a valarray

class DStructGDL: public SpDStruct
{
public:
  typedef SpDStruct::Ty    Ty;
  typedef SpDStruct        Traits;

private:
  typedef SpDStruct::DataT DataT;
  
  //public:
  DataT dd;           // the data
    
public:
  //structors
  ~DStructGDL(); 

  // default (needed for test instantiation)
  //  DStructGDL(): SpDStruct(), d() {}

  // new array, don't init fields
  DStructGDL( DStructDesc* desc_, const dimension& dim_, BaseGDL::InitType iT): 
    SpDStruct( desc_, dim_), 
    dd(dim.N_Elements()*desc_->NTags(),SpDStruct::zero)
  {
    dim.Purge();
  }
  
  // new array
  DStructGDL( DStructDesc* desc_, const dimension& dim_): 
    SpDStruct( desc_, dim_), 
    dd(dim.N_Elements()*desc_->NTags(),SpDStruct::zero)
  {
    dim.Purge();
    
    SizeT nEl=dim.N_Elements();

    SizeT nTags=NTags();
    
    for( SizeT ii=0; ii < nEl; ii++)
      {
	SizeT iiTag=ii*nTags;
	for( SizeT i=0; i < nTags; i++)
	  {
	    dd[iiTag+i]=(*Desc())[i]->GetInstance();
	  }
      }
  }
  
  // new scalar
  DStructGDL( DStructDesc* desc_): SpDStruct(desc_), dd(desc_->NTags())
  { 
    SizeT nTags=NTags();

    for( SizeT i=0; i < nTags; i++)
      {
	dd[i]=(*Desc())[i]->GetInstance();
      }
  }

  // new scalar, creating new descriptor
  // intended for internal (C++) use to ease struct definition
  // ATTENTION: This can only be used for NAMED STRUCTS!
  // please use the normal constructor (above) for unnamed structs 
  DStructGDL( const std::string& name_);
  
  // c-i 
  DStructGDL(const DStructGDL& d_);


  // operators

  // assignment. 
  //  DStructGDL& operator=(const DStructGDL& right);

  DStructGDL* CShift( DLong d);
  DStructGDL* CShift( DLong d[MAXRANK]);

  // for use by MIN and MAX functions
  void MinMax( DLong* minE, DLong* maxE, BaseGDL** minVal, BaseGDL** maxVal);

  //  const DType   Type()    const { return SpDStruct::t;}
  //  const std::string& TypeStr() const { return SpDStruct::str;}
  bool          EqType( const BaseGDL* r) const 
  { return (SpDStruct::t == r->Type());} 

  const SizeT N_Elements() const { return dd.size()/NTags();}
  const SizeT Size() const { return dd.size()/NTags();}
  const SizeT NBytes() const // for assoc function
  { 
    return (Sizeof() * N_Elements());
  }
  const SizeT ToTransfer() const // number of elements for IO tranfer  
  { 
    SizeT nB = 0;
    SizeT nTags=NTags();
    for( SizeT i=0; i < nTags; i++)
      {
	nB += dd[i]->ToTransfer();
      }
    return ( nB * N_Elements());
  }
  const SizeT Sizeof() const
  {
    SizeT nB = 0;
    SizeT nTags=NTags();
    for( SizeT i=0; i < nTags; i++)
      {
	nB += dd[i]->NBytes();
      }
    return nB;
  }

  void Clear() 
  {
    SizeT nEl = dd.size();
    for( SizeT i = 0; i<nEl; ++i)
      dd[ i]->Clear();
  }
  
  // code in default_io.cpp
  BaseGDL* AssocVar( int, SizeT);

  // single element access. 
  BaseGDL*& Get( SizeT tag, SizeT ix)
  {
    return dd[ NTags()*ix + tag];
  }

  // single tag access. 
  BaseGDL* Get( SizeT tag);

  //***
//   friend std::ostream& operator<<(std::ostream& o, DStructGDL& data_);
  friend std::istream& operator>>(std::istream& i, DStructGDL& data_);
  
  std::ostream& ToStream(std::ostream& o, SizeT width = 0, SizeT* actPosPtr = NULL);
  //  std::ostream& ToStream(std::ostream& o)
  //  { o << *this; return o;}

  std::istream& FromStream(std::istream& i)
  { i >> *this; return i;}
  

  void AddParent( DStructDesc* p);

private:
  void AddTag(BaseGDL* data);     // adds copy of data
  void AddTagGrab(BaseGDL* data); // adds data (only used by ExtraT) 
  friend class ExtraT;

public:
  // adds data (grabing) also to descriptor (for initialization only)
  void NewTag( const std::string& tName, BaseGDL* data); 

  // for easier internal (c++) usage
  template< class DataGDL>
  void InitTag(const std::string& tName, const DataGDL& data)
  {
    int tIx = Desc()->TagIndex( tName);
    if( tIx == -1)
      throw GDLException("Struct "+Desc()->Name()+
			 " does not contain tag "+tName+".");
    static_cast<DataGDL&>(*dd[ tIx]) = data; // copy data
  }
  
  // members
  // used by the interpreter
  // checks also if scalar is >=0
  int Scalar2index( SizeT& st) const;
  
  bool Scalar() const 
  { return (dd.size() == NTags());}
  
  // make a duplicate on the heap
  DStructGDL* Dup() { return new DStructGDL(*this);}
  
  DStructGDL* New( const dimension& dim_,
		   BaseGDL::InitType noZero=BaseGDL::ZERO)
  {
    if( noZero == BaseGDL::NOZERO)
      {
	DStructGDL* res = new DStructGDL( Desc(), dim_, noZero);
	res->MakeOwnDesc();
	return res;
      }
    if( noZero == BaseGDL::INIT)
      {
	DStructGDL* res =  new DStructGDL( Desc(), dim_, BaseGDL::NOZERO);
	res->MakeOwnDesc();
	SizeT nEl = res->dd.size();
	SizeT nTags = NTags();
	for( SizeT i=0; i<nEl; ++i) res->dd[ i] = dd[ i % nTags]->Dup();
	return res;
      }
    DStructGDL* res = new DStructGDL( Desc(), dim_);
    res->MakeOwnDesc();
    return res;
  }

  // used by interpreter, calls CatInsert
  DStructGDL* CatArray( ExprListT& exprList, 
			const SizeT catRank, 
			const SizeT rank);

  BaseGDL* Convert2( DType destTy, 
		     BaseGDL::Convert2Mode mode = BaseGDL::CONVERT);
  
#ifdef TEMPLATE_FRIEND_OK_
  // make all other Convert2 functions friends
  template<class Sp2>  
  friend BaseGDL* Data_<Sp2>::Convert2( DType destTy, 
					BaseGDL::Convert2Mode);
#else
  // this explicit version should work in all cases
  friend BaseGDL* Data_<SpDByte>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDInt>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDUInt>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDLong>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDULong>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDLong64>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDULong64>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDFloat>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDDouble>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDString>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDPtr>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDObj>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDComplex>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDComplexDbl>::Convert2( DType destTy, BaseGDL::Convert2Mode);

#endif

  std::ostream& Write( std::ostream& os, bool swapEndian);
  std::istream& Read( std::istream& os, bool swapEndian);

  bool True()
  { throw GDLException("Struct expression not allowed in this context.");}
  bool False()
  { throw GDLException("Struct expression not allowed in this context.");}
  bool LogTrue()
  { throw GDLException("Struct expression not allowed in this context.");}
  bool LogTrue( SizeT ix)
  { throw GDLException("Struct expression not allowed in this context.");}
  Data_<SpDByte>* LogNeg()
  { throw GDLException("Struct expression not allowed in this context.");}
  int  Sgn(); // -1,0,1
  bool Equal( BaseGDL*);
  bool ArrayEqual( BaseGDL*);
  // 'for' statement compliance (int types , float types scalar only)
  void ForCheck( BaseGDL**, BaseGDL** =NULL);
  bool ForCondUp( BaseGDL*);
  bool ForCondDown( BaseGDL*);
  void ForAdd( BaseGDL* add=NULL);

  BaseGDL* UMinus(); // UMinus for SpDString returns float
  DStructGDL*   NotOp();
  DStructGDL*   AndOp( BaseGDL* r);
  DStructGDL*   AndOpInv( BaseGDL* r);
  DStructGDL*   OrOp( BaseGDL* r);
  DStructGDL*   OrOpInv( BaseGDL* r);
  DStructGDL*   XorOp( BaseGDL* r);
  DStructGDL*   Add( BaseGDL* r);
  DStructGDL*   AddInv( BaseGDL* r);
  DStructGDL*   Sub( BaseGDL* r);
  DStructGDL*   SubInv( BaseGDL* r);
  DStructGDL*   GtMark( BaseGDL* r);
  DStructGDL*   LtMark( BaseGDL* r);
  DStructGDL*   Mult( BaseGDL* r);
  DStructGDL*   Div( BaseGDL* r);
  DStructGDL*   DivInv( BaseGDL* r);
  DStructGDL*   Mod( BaseGDL* r);
  DStructGDL*   ModInv( BaseGDL* r);
  DStructGDL*   Pow( BaseGDL* r);
  DStructGDL*   PowInv( BaseGDL* r);
  DStructGDL*   MatrixOp( BaseGDL* r);
  
  Data_<SpDByte>* EqOp( BaseGDL* r);
  Data_<SpDByte>* NeOp( BaseGDL* r);
  Data_<SpDByte>* GtOp( BaseGDL* r);
  Data_<SpDByte>* GeOp( BaseGDL* r);
  Data_<SpDByte>* LtOp( BaseGDL* r);
  Data_<SpDByte>* LeOp( BaseGDL* r);

  // assigns srcIn to this at ixList, if ixList is NULL does linear copy
  // assumes: ixList has this already set as variable
  // frees overwritten members
  void AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList=NULL, SizeT offset=0);

  void DecAt( ArrayIndexListT* ixList)
  {
    throw GDLException("Struct expression not allowed in this context.");
  }
  void IncAt( ArrayIndexListT* ixList)
  {
    throw GDLException("Struct expression not allowed in this context.");
  }
  void Dec()
  {
    throw GDLException("Struct expression not allowed in this context.");
  }
  void Inc()
  {
    throw GDLException("Struct expression not allowed in this context.");
  }

  // used by AccessDescT for resolving, no checking is done
  // inserts srcIn[ ixList] at offset
  // here d is initially empty -> no deleting of old data in InsertAt
  void InsertAt( SizeT offset, BaseGDL* srcIn, ArrayIndexListT* ixList);
  
  // returns (*this)[ ixList]
  DStructGDL* Index( ArrayIndexListT* ixList);

  // formatting output functions
  
  SizeT OFmtA( std::ostream* os, SizeT offset, SizeT num, int width); 
  SizeT OFmtF( std::ostream* os, SizeT offs, SizeT num, int width, 
		int prec, BaseGDL::IOMode oM = FIXED);
  SizeT OFmtI( std::ostream* os, SizeT offs, SizeT num, int width, 
		int minN, BaseGDL::IOMode oM = DEC);
  
  SizeT IFmtA( std::istream* is, SizeT offset, SizeT num, int width); 
  SizeT IFmtF( std::istream* is, SizeT offs, SizeT num, int width); 
  SizeT IFmtI( std::istream* is, SizeT offs, SizeT num, int width, 
		BaseGDL::IOMode oM = DEC);

private:
  void IFmtAll( SizeT offs, SizeT r,
		SizeT& firstOut, SizeT& firstOffs, SizeT& tCount, SizeT& tCountOut);
  
  void OFmtAll( SizeT offs, SizeT r,
		SizeT& firstOut, SizeT& firstOffs, SizeT& tCount, SizeT& tCountOut);
  
  // inserts srcIn at ixDim
  // respects the exact structure
  // used by Assign -> old data must be freed
  void InsAt( DStructGDL* srcIn, ArrayIndexListT* ixList);

  // used for concatenation, called from CatArray
  // assumes that everything is checked (see CatInfo)
  void CatInsert( const DStructGDL* srcArr, const SizeT atDim, SizeT& at);
};

#endif
