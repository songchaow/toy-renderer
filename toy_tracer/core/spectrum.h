#pragma once
#include "core/common.h"
#include "utils/array.h"
#include "utils/utils.h"

static const int nCIESamples = 471;
extern const Float CIE_X[nCIESamples];
extern const Float CIE_Y[nCIESamples];
extern const Float CIE_Z[nCIESamples];
static const Float CIE_Y_integral = 106.856895F;
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
    SampledSpectrum operator -() const;
    SampledSpectrum& operator *=(SampledSpectrum& s);
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

struct R8G8B8;

struct RGBSpectrum {
      Float rgb[3];
      RGBSpectrum() = default;
      RGBSpectrum(Float val) { for (int i = 0; i < 3; i++) rgb[i] = val; }
      RGBSpectrum(Float r, Float g, Float b) {rgb[0] = r; rgb[1] = g; rgb[2] = b;}
      R8G8B8 toR8G8B8();
      RGBSpectrum& operator/=(const RGBSpectrum& rhs) { for (int i = 0; i < 3; i++) rgb[i] /= rhs.rgb[i]; return *this; }
      RGBSpectrum& operator+=(const RGBSpectrum& rhs) { for (int i = 0; i < 3; i++) rgb[i] += rhs.rgb[i]; return *this; }
      RGBSpectrum& operator*=(const RGBSpectrum& rhs) { for (int i = 0; i < 3; i++) rgb[i] *= rhs.rgb[i]; return *this; }
      RGBSpectrum operator-();
      Float& operator[](int idx) { return rgb[idx]; }
      const Float& operator[](int idx) const { return rgb[idx]; }
};

typedef RGBSpectrum Spectrum;

RGBSpectrum operator+(const RGBSpectrum& lhs, const RGBSpectrum& rhs);
RGBSpectrum operator-(const RGBSpectrum& lhs, const RGBSpectrum& rhs);
RGBSpectrum operator*(const RGBSpectrum& lhs, const RGBSpectrum& rhs);
RGBSpectrum operator/(const RGBSpectrum& lhs, const RGBSpectrum& rhs);

static inline Float GammaTransform(Float stimus) {
      if (stimus <= 0.0031308f) return 12.92f * stimus;
      return 1.055f * std::pow(stimus, (Float)(1.f / 2.4f)) - 0.055f;
}

static inline Float GammaInvTransform(Float img_value) {
      if (img_value <= 0.04045f) return img_value * 1.f / 12.92f;
      return std::pow((img_value + 0.055f) * 1.f / 1.055f, (Float)2.4f);
}

struct R8G8B8 {
      unsigned char rgb[3];
      R8G8B8() = default;
      explicit R8G8B8(char val) { for (int i = 0; i < 3; i++) rgb[i] = val; }
      R8G8B8(char r, char g, char b) { rgb[0] = r; rgb[1] = g; rgb[2] = b; }
      unsigned char& operator [](int idx) { return rgb[idx]; }
      R8G8B8 operator/(Float div) { R8G8B8 ret(rgb[0] / div, rgb[1] / div, rgb[2] / div); return ret; }
      RGBSpectrum toRGBSpectrum() { return RGBSpectrum((Float)(rgb[0]) / 255, (Float)(rgb[1]) / 255, (Float)(rgb[2]) / 255); }
};

struct R8G8B8A8 {
      char rgba[4];
      R8G8B8A8() = default;
      R8G8B8A8(char val) { for (int i = 0; i < 4; i++) rgba[i] = val; }
      R8G8B8A8(char r, char g, char b, char a) { rgba[0] = r; rgba[1] = g; rgba[2] = b; rgba[3] = a; }
      RGBSpectrum toRGBSpectrum() { return RGBSpectrum((Float)(rgba[0]) / 255, (Float)(rgba[1]) / 255, (Float)(rgba[2]) / 255); }
};

// From Radiance 0-1 -> sRGB 0-256
static Float GammaCorrection(Float stimus) {
      return 255.99f * Clamp(GammaTransform(stimus), 0, 1);
}
static R8G8B8 GammaCorrection(RGBSpectrum stimus) {
      return R8G8B8(GammaCorrection(stimus[0]),
                    GammaCorrection(stimus[1]),
                    GammaCorrection(stimus[2]));
}

// sRGB 0-256 -> Radiance 0-1
static Float InverseGammaCorrection(unsigned char image_value) {
      return GammaInvTransform(Clamp((Float)image_value/256, 0.f, 1.f));
}
static RGBSpectrum InverseGammaCorrection(R8G8B8 image_value) {
      return RGBSpectrum(InverseGammaCorrection(image_value[0]),
                         InverseGammaCorrection(image_value[1]),
                         InverseGammaCorrection(image_value[2]));
}
