// TODO: move to main
#pragma once
#include <QString>
#include <QWidget>
#include <QToolBox>
#include <QLineEdit>
#include <map>
#include "core/common.h"

struct RGBSpectrum;
struct Transform;

class RendererObject : public QObject {
      Q_OBJECT
      QString id;
public:
      static void addProperty(const RGBSpectrum* s, QWidget* parent, QString desc = "");
      static void addProperty(const Transform* t, QWidget* parent, QString desc = "");
      static void addConstText(QString desc, QString value, QWidget* parent);
      static void addText(QString desc, const QString value_ptr, QWidget* target);
      static void addNumberf(QString desc, Float* value_ptr, QWidget* target);
      static void addNumberi(QString desc, int* value_ptr, QWidget* target);
      static QLineEdit* addFileDialog(QString desc, QString button_text, QWidget* target, QString path = QString(), QStringList filters = QStringList());
private slots:
      void updateValue();
public:
      enum class TypeID {
            Camera,
            Primitive,
            Light, // and other lights
            Shape,
            Image,
            Material,
      };
      static const std::map<TypeID, QString> TypeIDMap;
private:
      TypeID _typeID;
protected:
      QString& nameRef() { return id; }
public:
      virtual void addProperties(QWidget* parent) const;
      const QString name() const { return id; }
      const TypeID typeID() const { return _typeID; }
      const QString type_name() const { return TypeIDMap.at(_typeID); }
      void rename(QString newname) { id = newname; }
      RendererObject(TypeID type_id, QString name) : _typeID(type_id), id(name) {}
      RendererObject(TypeID type_id) : _typeID(type_id) {}
};