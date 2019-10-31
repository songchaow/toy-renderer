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
      void addProperties(QWidget* parent) const;
      void reset(Point3f* _mIn) { _m = _mIn; }
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

class Primitive_Ui : public RendererObject {
      Primitive* _m = nullptr;
      // Child UI
      mutable PBRMaterial_Ui _materialUi;
      mutable Shape_Ui shape;
public:
      Primitive_Ui(Primitive* p) : _m(p), shape(p->shape()), RendererObject(TypeID::Primitive) {}
      void addProperties(QWidget * parent) const override;
      void reset(Primitive* _mIn) { _m = _mIn; }
      PBRMaterial_Ui& materialUi() { return  _materialUi; }
      void setMaterialUi(PBRMaterial* pbr_m) const { _materialUi.reset(pbr_m); }
      const PBRMaterial_Ui& materialUi() const { return _materialUi; }
      const Shape_Ui& shapeUi() const { return shape; }
      void setShapeUi(const Shape* s) { shape.reset(s); }
      Primitive* m() const { return _m; }
      bool isValid() const { return _m != nullptr; }
};

class PointLight_Ui : public RendererObject {
      PointLight* _m = nullptr;
      // Child UI
      mutable Point3f_Ui _pos;
public:
      void addProperties(QWidget * parent) const override;
      PointLight_Ui(PointLight* l) : _m(l), RendererObject(TypeID::Light) {}
      void reset(PointLight* _mIn) { _m = _mIn; }
      PointLight* m() const { return _m; }
};