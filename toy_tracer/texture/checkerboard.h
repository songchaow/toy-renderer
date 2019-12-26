#pragma once
#include "core/texture.h"

class CheckerBoard : public RGBSpectrumTexture2D {
      // all in radiance
      RGBSpectrum _colorASpectrum;
      RGBSpectrum _colorBSpectrum;
      
      Float stripSize;


public:
      // As inputs, colorA, colorB are in sRGB
      CheckerBoard(R8G8B8 colorA, R8G8B8 colorB, Float stripSize, bool sRGB = true) : stripSize(stripSize) {
            if(sRGB) {
                  _colorASpectrum = InverseGammaCorrection(colorA);
                  _colorBSpectrum = InverseGammaCorrection(colorB);
            }
            else {
                  _colorASpectrum = colorA.toRGBSpectrum();
                  _colorBSpectrum = colorB.toRGBSpectrum();
            }
      }
      CheckerBoard(R8G8B8 colorA, R8G8B8 colorB, int numStrip, bool sRGB = true) : 
            CheckerBoard(colorA, colorB, 1.f/numStrip, sRGB) {}
      RGBSpectrum Evaluate(Float u, Float v) override;

};