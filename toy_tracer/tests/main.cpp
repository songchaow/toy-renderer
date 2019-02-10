#include <iostream>
#include "core/spectrum.h"
#include "core/common.h"
int main()
{
    // convert a SPD to XYZ
    SampledSpectrum::Initialize();
    std::cout<< "Refl_Red: ";
    for(int i=0;i<SampledSpectrum::nSpectrralSamples;i++)
        std::cout<<SampledSpectrum::Refl_Red[i]<<' ';
    std::cout<<std::endl;
    SampledSpectrum a(SampledSpectrum::Refl_Red);
    Float xyz[3];
    a.ToXYZ(xyz);
    std::cout << "XYZ: ";
    for(int i=0;i<3;i++)
        std::cout<<xyz[i]<<' ';
    std::cout << std::endl;
}