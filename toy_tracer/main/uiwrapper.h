#pragma once
#include "main/scene_object.h"
#include "core/material.h"
#include "core/primitive.h"
#include "core/transform.h"
#include "light/point.h"

class Point3f_Ui : public QObject {
      Point3f* _m;
      // UI widgets
      mutable QLineEdit* posX;
      mutable QLineEdit* posY;
      mutable QLineEdit* posZ;
public:
      Point3f_Ui(Point3f* m) : _m(m) {}
      void addProperties(QWidget* parent) const;
      void reset(Point3f* _mIn) { _m = _mIn; }
public slots:
      void updateProperties();
};

class Vector3f_Ui : public QObject {
      Vector3f* _m;
      // UI widgets
      mutable QLineEdit* posX;
      mutable QLineEdit* posY;
      mutable QLineEdit* posZ;
public:
      Vector3f_Ui(Vector3f* m) : _m(m) {}
      void addProperties(QWidget* parent) const;
      void reset(Vector3f* _mIn) { _m = _mIn; }
public slots:
      void updateProperties();
};

class Transform_Ui : public QObject {
      Transform* _m = nullptr;
      // SRT data
      mutable SRT srt;
      mutable QLineEdit* translationX;
      mutable QLineEdit* translationY;
      mutable QLineEdit* translationZ;
      mutable QLineEdit* rotationX;
      mutable QLineEdit* rotationY;
      mutable QLineEdit* rotationZ;
      mutable QLineEdit* scaleX;
      mutable QLineEdit* scaleY;
      mutable QLineEdit* scaleZ;

public:
      void addProperties(QWidget* parent) const;
      void reset(Transform* m) { _m = m; }
public slots:
      void updateProperties();
};

class PBRMaterial_Ui : public RendererObject {
      PBRMaterial* _m = nullptr;
      // UI
      mutable QLineEdit* albedo_text;
      mutable QLineEdit* metallic_text;
      mutable QLineEdit* rough_text;
      QString albedo_path;
      QString metallic_path;
      QString rough_path;
public slots:
      void updateProperties();
public:
      PBRMaterial_Ui(PBRMaterial* _m) : _m(_m), RendererObject(TypeID::Material, "Material") {}
      PBRMaterial_Ui():RendererObject(TypeID::Material, "Material") {}
      void reset(PBRMaterial* _mIn) { _m = _mIn; }
      bool isValid() const { return _m != nullptr; }
      void addProperties(QWidget* parent) const override;
};

class Shape_Ui : public RendererObject {
      const Shape* _m = nullptr;
      // Child UI
      // TODO:
      // Transform_Ui obj2world, world2obj
public:
      Shape_Ui(const Shape* s) : _m(s), RendererObject(TypeID::Shape) {}
      void addProperties(QWidget* parent) const override;
      void reset(const Shape* _mIn) { _m = _mIn; }
      bool isValid() const { return _m != nullptr; }
};

class RGB_Spectrum_Ui : public QObject {
      mutable QLineEdit* _r;
      mutable QLineEdit* _g;
      mutable QLineEdit* _b;
      RGBSpectrum* _m;

public:
      RGB_Spectrum_Ui(RGBSpectrum* _m) : _m(_m) {}
      void addProperties(QWidget* parent) const;
public slots:
      void updateProperties();
};

class Primitive3D_Ui : public RendererObject {
      Primitive3D* _m = nullptr;
      // Child UI
      mutable PBRMaterial_Ui _materialUi;
      mutable Shape_Ui shape;
public:
      Primitive3D_Ui(Primitive3D* p) : _m(p), shape(p->shape()), RendererObject(TypeID::Primitive3D, "3DChar") {}
      void addProperties(QWidget * parent) const override;
      void reset(Primitive3D* _mIn) { _m = _mIn; }
      PBRMaterial_Ui& materialUi() { return  _materialUi; }
      void setMaterialUi(PBRMaterial* pbr_m) const { _materialUi.reset(pbr_m); }
      const PBRMaterial_Ui& materialUi() const { return _materialUi; }
      const Shape_Ui& shapeUi() const { return shape; }
      void setShapeUi(const Shape* s) { shape.reset(s); }
      Primitive3D* m() const { return _m; }
      bool isValid() const { return _m != nullptr; }
};

class Primitive2D_Ui : public RendererObject {
      Primitive2D* _m = nullptr;
public:
      Primitive2D_Ui(Primitive2D* p) : _m(p), RendererObject(TypeID::Primitive2D, "2DChar") {}
      Primitive2D* m() const { return _m; }
};

class PointLight_Ui : public RendererObject {
      PointLight* _m = nullptr;
      // Child UI
      mutable Point3f_Ui _pos;
      mutable Vector3f_Ui _dir;
      mutable RGB_Spectrum_Ui _rgb;
public:
      void addProperties(QWidget * parent) const override;
      PointLight_Ui(PointLight* l) : _m(l), _pos(&l->rpos()), _dir(&l->rdirection()), _rgb(&l->rradiance()), RendererObject(TypeID::Light) {}
      void reset(PointLight* _mIn) { _m = _mIn; }
      PointLight* m() const { return _m; }
};

void addDefaultProperties(QWidget* parent);