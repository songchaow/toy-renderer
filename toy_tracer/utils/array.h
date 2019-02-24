/*  array.h
    A light-weight array with operaters overloaded.
 */
#pragma once
#include "core/common.h"
template<unsigned int N>
struct ArrayF
{
    Float data[N];
	ArrayF() = default;
    ArrayF(Float* begin) { for(int i=0;i<N;i++) data[i]=begin[i];}
    Float& operator [](int i) {return data[i];}
	ArrayF<N> operator *(Float scale) const { ArrayF<N> ret; for (int i = 0; i < N; i++) ret[i] = scale * data[i]; return ret; }
    ArrayF<N>& operator*=(Float scale) { for(int i=0;i<N;i++) data[i]*=scale; return *this;}
    Float* GetAddr() const {return data;}
};