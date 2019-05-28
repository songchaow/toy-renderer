#pragma once
#include "core/common.h"
#include "utils/array.h"

static const int nCIESamples = 471;
extern const Float CIE_X[nCIESamples];
extern const Float CIE_Y[nCIESamples];
extern const Float CIE_Z[nCIESamples];
static const Float CIE_Y_integral = 106.856895;
static const int nRGB2SpectSamples = 32;
extern const Float RGB2SpectLambda[nRGB2SpectSamples];
extern const Float RGBRefl2SpectWhite[nRGB2SpectSamples];
extern const Float RGBRefl2SpectCyan[nRGB2SpectSamples];
extern const Float RGBRefl2SpectMagenta[nRGB2SpectSamples];
extern const Float RGBRefl2SpectYellow[nRGB2SpectSamples];
extern const Float RGBRefl2SpectRed[nRGB2SpectSamples];
extern const Float RGBRefl2SpectGreen[nRGB2SpectSamples];
extern const Float RGBRefl2SpectBlue[nRGB2SpectSamples];
extern const Float RGBIllum2SpectWhite[nRGB2SpectSamples];
extern const Float RGBIllum2SpectCyan[nRGB2SpectSamples];
extern const Float RGBIllum2SpectMagenta[nRGB2SpectSamples];
extern const Float RGBIllum2SpectYellow[nRGB2SpectSamples];
extern const Float RGBIllum2SpectRed[nRGB2SpectSamples];
extern const Float RGBIllum2SpectGreen[nRGB2SpectSamples];
extern const Float RGBIllum2SpectBlue[nRGB2SpectSamples];

inline void XYZToRGB(const Float xyz[3], Float rgb[3]) {
	rgb[0] = 3.240479f * xyz[0] - 1.537150f * xyz[1] - 0.498535f * xyz[2];
	rgb[1] = -0.969256f * xyz[0] + 1.875991f * xyz[1] + 0.041556f * xyz[2];
	rgb[2] = 0.055648f * xyz[0] - 0.204043f * xyz[1] + 1.057311f * xyz[2];
}

inline void RGBToXYZ(const Float rgb[3], Float xyz[3]) {
	xyz[0] = 0.412453f * rgb[0] + 0.357580f * rgb[1] + 0.180423f * rgb[2];
	xyz[1] = 0.212671f * rgb[0] + 0.715160f * rgb[1] + 0.072169f * rgb[2];
	xyz[2] = 0.019334f * rgb[0] + 0.119193f * rgb[1] + 0.950227f * rgb[2];
}

enum SpectrumType
{
    Reflectance,
    Illuminance
};

class SampledSpectrum
{
public:
    static const int nSpectrralSamples = 60;
    static const int startLambda = 400;
    static const int endLambda = 700;
    static const int lambdaSlice = (endLambda-startLambda)/nSpectrralSamples;
    static Float CIE_X_Resampled[nSpectrralSamples];
    static Float CIE_Y_Resampled[nSpectrralSamples];
    static Float CIE_Z_Resampled[nSpectrralSamples];
    static Float Refl_White[nSpectrralSamples];
    static Float Refl_Cyan[nSpectrralSamples];
    static Float Refl_Magenta[nSpectrralSamples];
    static Float Refl_Yellow[nSpectrralSamples];
    static Float Refl_Red[nSpectrralSamples];
    static Float Refl_Green[nSpectrralSamples];
    static Float Refl_Blue[nSpectrralSamples];
    static Float Illum_White[nSpectrralSamples];
    static Float Illum_Cyan[nSpectrralSamples];
    static Float Illum_Magenta[nSpectrralSamples];
    static Float Illum_Yellow[nSpectrralSamples];
    static Float Illum_Red[nSpectrralSamples];
    static Float Illum_Green[nSpectrralSamples];
    static Float Illum_Blue[nSpectrralSamples];
    // Initialize: compute resampled CIE X,Y,Z.
    static void Initialize();
    static void FromSampled(Float lambdas[], Float values[], int n, SampledSpectrum* s);
    static void FromRGB(Float rgb[], SampledSpectrum* s, SpectrumType type = SpectrumType::Illuminance);
    Float& operator[](int n) { return c[n]; }
    const Float& operator[](int n) const {return c[n]; }
    // Operations on coefficients
    void AddFromArray(Float coeff[]) { for(int i=0;i<nSpectrralSamples;i++) c[i]+=coeff[i];}
    void AddFromArray(ArrayF<nSpectrralSamples> &ar) { for(int i=0;i<nSpectrralSamples;i++) c[i]+=ar[i]; }
    Float* GetAddr() {return c;}
    SampledSpectrum operator *(SampledSpectrum& s) const;
    SampledSpectrum operator -(Float s) const;
    SampledSpectrum operator -() const;
    SampledSpectrum operator /(Float s) const;
    SampledSpectrum& operator *=(Float s) { for(int i=0;i<nSpectrralSamples;i++) c[i]*=s; return *this; }
    SampledSpectrum& operator +=(ArrayF<nSpectrralSamples> &ar) { for(int i=0;i<nSpectrralSamples;i++) c[i]+=ar[i]; return *this; }
    // ctors
    SampledSpectrum() = default;
    SampledSpectrum(Float val) { for (int i = 0; i < nSpectrralSamples; i++) c[i] = val; }
    SampledSpectrum(int val) { for (int i = 0; i < nSpectrralSamples; i++) c[i] = (Float)(val); }
    SampledSpectrum(const Float* samples) { for(int i=0;i<nSpectrralSamples;i++) c[i]=samples[i]; }
    // ToXYZ
    void ToXYZ(Float xyz[3]) const;
    void ToRGB(Float rgb[3]) const;


private:
    Float c[nSpectrralSamples];
    bool fromRGB = false;
    Float rgb[3]; // available if generated from RGB
};

SampledSpectrum operator*(const SampledSpectrum& lhs, const SampledSpectrum& rhs);
SampledSpectrum operator+(const SampledSpectrum& lhs, const SampledSpectrum& rhs);
SampledSpectrum operator-(const SampledSpectrum& lhs, const SampledSpectrum& rhs);
SampledSpectrum operator/(const SampledSpectrum& lhs, const SampledSpectrum& rhs);

typedef SampledSpectrum Spectrum;

struct RGBSpectrum {
      Float rgb[3];
};