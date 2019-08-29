#pragma once
#include <QString>
#include <QWidget>
#include <QToolBox>
#include <map>
#include "core/common.h"

struct RGBSpectrum;
struct Transform;

class RendererObject : public QObject {
      QString id;
protected:
      void addProperty(const RGBSpectrum* s, QWidget* parent, QString desc = "");
      void addProperty(const Transform* t, QWidget* parent, QString desc = "");
      void addConstText(QString desc, QString value, QWidget* parent);
      void addText(QString desc, QString* value_ptr, QWidget* target);
      void addNumberf(QString desc, Float* value_ptr, QWidget* target);
      void addNumberi(QString desc, int* value_ptr, QWidget* target);
private slots:
      void updateValue();
public:
      enum class TypeID {
            Camera,
            Primitive,
            Light, // and other lights
            Shape,
      };
      static const std::map<TypeID, QString> TypeIDMap;
private:
      TypeID type_id;
protected:
      QString& nameRef() { return id; }
public:
      virtual void addProperties(QWidget* parent);
      const QString name() { return id; }
      const QString type_name() { return TypeIDMap.at(type_id); }
      void rename(QString newname) { id = newname; }
      RendererObject(TypeID type_id, QString name) : type_id(type_id), id(name) {}
      RendererObject(TypeID type_id) : type_id(type_id) {}
};