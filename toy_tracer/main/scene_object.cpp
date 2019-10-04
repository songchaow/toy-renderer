#include "main/scene_object.h"
#include "utils/utils.h"
#include "main/MainWindow.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QFileDialog>

const std::map<RendererObject::TypeID, QString> RendererObject::TypeIDMap = {
      GEN_VERBOSE_STRING_MAP(TypeID::Camera),
      GEN_VERBOSE_STRING_MAP(TypeID::Primitive),
      GEN_VERBOSE_STRING_MAP(TypeID::Light),
      GEN_VERBOSE_STRING_MAP(TypeID::Camera),
      GEN_VERBOSE_STRING_MAP(TypeID::Shape),
      GEN_VERBOSE_STRING_MAP(TypeID::Image),
      GEN_VERBOSE_STRING_MAP(TypeID::Material)
};

void RendererObject::updateValue() {
      QObject* obj = sender();
      if (obj->property("string_ptr").isValid()) {
            const QString& t = static_cast<QLineEdit*>(obj)->text();
            QString* p = static_cast<QString*>(obj->property("string_ptr").value<void*>());
            *p = t;
      }
      else
            ;
}

void RendererObject::addProperty(const RGBSpectrum* s, QWidget* parent, QString desc/* = ""*/) {
      // TODO:
}

void RendererObject::addProperty(const Transform* t, QWidget* parent, QString desc/* = ""*/) {
      // add signal when the text edit changes
      // maybe another parameter isMesh is needed.
      // another way: directly change t!
}

void RendererObject::addConstText(QString desc, QString value, QWidget* target) {
      
      QWidget* lineWidget = new QWidget;
      QHBoxLayout* lineLayout = new QHBoxLayout;
      lineLayout->addWidget(new QLabel(desc));
      auto* edit = new QLineEdit(value);
      edit->setEnabled(false);
      lineLayout->addWidget(edit);
      lineWidget->setLayout(lineLayout);
      QLayout* parentLayout = target->layout();
      parentLayout->addWidget(lineWidget);
}

void RendererObject::addText(QString desc, const QString value_ptr, QWidget* target) {
      QWidget* lineWidget = new QWidget;
      QHBoxLayout* lineLayout = new QHBoxLayout;
      lineLayout->addWidget(new QLabel(desc));
      auto* edit = new QLineEdit(value_ptr);
      lineLayout->addWidget(edit);
      lineWidget->setLayout(lineLayout);
      QLayout* parentLayout = target->layout();
      parentLayout->addWidget(lineWidget);
      edit->setProperty("string_ptr", QVariant::fromValue(value_ptr));
      // TODO: instead of the following, store the UI element in a UI wrapper.
      //connect(edit, &QLineEdit::returnPressed, this, &RendererObject::updateValue);
}

void RendererObject::addNumberf(QString desc, Float* value_ptr, QWidget* target) {
      QHBoxLayout* lineLayout = new QHBoxLayout;
      lineLayout->addWidget(new QLabel(desc));
      QDoubleSpinBox* numEdit = new QDoubleSpinBox();
      numEdit->setValue(*value_ptr);
      lineLayout->addWidget(numEdit);
      target->setLayout(lineLayout);
      // TODO: signal: QDoubleSpinBox::valueChanged() -> updateValue()
}

QLineEdit* RendererObject::addFileDialog(QString desc, QString button_text, QWidget* target, QString path, QStringList filters) {
      // TODO: add captions, default dir to parameters
      QWidget* lineWidget = new QWidget;
      QHBoxLayout* lineLayout = new QHBoxLayout;
      lineLayout->addWidget(new QLabel(desc));
      QLineEdit* filePath = new QLineEdit();
      lineLayout->addWidget(filePath);
      QPushButton* open = new QPushButton(tr(button_text.toLocal8Bit()));
      lineLayout->addWidget(open);
      lineWidget->setLayout(lineLayout);
      auto* parentLayout = target->layout();
      parentLayout->addWidget(lineWidget);
      auto openFile = [=]() {
            QFileDialog dialog(MainWindow::getInstance(), "Open File...");
            if (dialog.exec())
                  filePath->setText(dialog.selectedFiles()[0]);
      };
      QObject::connect(open, &QPushButton::clicked, openFile);
      return filePath;
}

void RendererObject::addNumberi(QString desc, int* value_ptr, QWidget* target) {
      QHBoxLayout* lineLayout = new QHBoxLayout;
      lineLayout->addWidget(new QLabel(desc));
      QSpinBox* numEdit = new QSpinBox();
      numEdit->setValue(*value_ptr);
      lineLayout->addWidget(numEdit);
      target->setLayout(lineLayout);
      // TODO: signal: QSpinBox::valueChanged() -> updateValue()
}

void RendererObject::addProperties(QWidget* parent) const {
      // set global layout
      QVBoxLayout* globalLayout = new QVBoxLayout;
      if (parent->layout()) {
            // Delete all existing widgets, if any.
            if (parent->layout() != NULL)
            {
                  QLayoutItem* item;
                  while ((item = parent->layout()->takeAt(0)) != NULL)
                  {
                        delete item->widget();
                        delete item;
                  }
                  delete parent->layout();
            }
            delete parent->layout();
      }
      parent->setLayout(globalLayout);
      QGroupBox* roGroup = new QGroupBox("Object");
      // set current widget's layout
      QVBoxLayout* currentLayout = new QVBoxLayout;
      if (roGroup->layout())
            delete roGroup->layout();
      roGroup->setLayout(currentLayout);
      // Add items to current widget
      addConstText("Object type:", TypeIDMap.at(_typeID), roGroup);
      addText("Name:", id, roGroup);
      parent->layout()->addWidget(roGroup);
      
}