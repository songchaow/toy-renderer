#include "main/uiwrapper.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QDebug>
#include <QLabel>

void PBRMaterial_Ui::updateProperties() {
      if (!albedo_text->text().isEmpty())
            _m->albedo_map().resetImage(Image::CreateImageFromFile(albedo_text->text().toStdString()));
      if (!metallic_text->text().isEmpty())
            _m->metallic_map().resetImage(Image::CreateImageFromFile(metallic_text->text().toStdString()));
      if (!rough_text->text().isEmpty())
            _m->rough_map().resetImage(Image::CreateImageFromFile(rough_text->text().toStdString()));
      _m->setDirty();
}

void PBRMaterial_Ui::addProperties(QWidget* parent) const {
      //RendererObject::addProperties(parent);
      // TODO: add existing values
      qDebug() << "PBRMaterial thread affinity:" << thread();
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
      }
      parent->setLayout(globalLayout);
      albedo_text = RendererObject::addFileDialog("Albedo Map:", "Open", parent, QString::fromStdString(_m->albedo_map().path()));
      metallic_text = RendererObject::addFileDialog("Metallic Map:", "Open:", parent, QString::fromStdString(_m->metallic_map().path()));
      rough_text = RendererObject::addFileDialog("Rough Map:", "Open:", parent, QString::fromStdString(_m->rough_map().path()));
      // add create menu
      auto* parentLayout = parent->layout();
      QPushButton* update = new QPushButton("Update Material");
      parentLayout->addWidget(update);
      QObject::connect(update, &QPushButton::clicked, this, &PBRMaterial_Ui::updateProperties);
}

void Primitive_Ui::addProperties(QWidget* parent) const {
      RendererObject::addProperties(parent);
      QGroupBox* primitive_group = new QGroupBox("Primitive");
      // set current widget's layout
      QVBoxLayout* currentLayout = new QVBoxLayout;
      primitive_group->setLayout(currentLayout);
      // add toolbars for current item
      addConstText("Mesh Faces:", QString::number(_m->meshes().size()), primitive_group);
      // add sub toolbars for child items
      QGroupBox* shapeToolBox = new QGroupBox("Shape");
      shape.reset(_m->shape());
      if(shape.isValid()) {
            shape.addProperties(shapeToolBox);
      }
      currentLayout->addWidget(shapeToolBox);
      parent->layout()->addWidget(primitive_group);
}

void PointLight_Ui::addProperties(QWidget * parent) const {
      RendererObject::addProperties(parent);
      QGroupBox* pl_group = new QGroupBox("Point Light");
      QVBoxLayout* currentLayout = new QVBoxLayout;
      pl_group->setLayout(currentLayout);
      _pos.reset(&_m->rpos());
      _pos.addProperties(pl_group);
      parent->layout()->addWidget(pl_group);
}

void Shape_Ui::addProperties(QWidget* parent) const {
      RendererObject::addProperties(parent);
      addConstText("Shape:", QString::fromStdString(_m->shapeName()), parent);
}

void Transform_Ui::addProperties(QWidget * parent) const
{
      QGroupBox* transform_grp = new QGroupBox("Transform");
      QVBoxLayout* grp_layout = new QVBoxLayout;
      transform_grp->setLayout(grp_layout);
      // extract SRT matrices
      srt = _m->m.toSRT();
      // T
      QWidget* lineWidget_t = new QWidget;
      QHBoxLayout* lineLayout_t = new QHBoxLayout;
      lineWidget_t->setLayout(lineLayout_t);
      QLabel* translation_text = new QLabel("Translate:");
      translationX = new QLineEdit(QString::number(srt.translationX));
      translationY = new QLineEdit(QString::number(srt.translationY));
      translationZ = new QLineEdit(QString::number(srt.translationZ));
      lineLayout_t->addWidget(translation_text);
      lineLayout_t->addWidget(translationX);
      lineLayout_t->addWidget(translationY);
      lineLayout_t->addWidget(translationZ);
      // R
      QWidget* lineWidget_r = new QWidget;
      QHBoxLayout* lineLayout_r = new QHBoxLayout;
      lineWidget_r->setLayout(lineLayout_r);
      QLabel* rotation_text = new QLabel("Rotation:");
      rotationX = new QLineEdit(QString::number(srt.rotationX));
      rotationY = new QLineEdit(QString::number(srt.rotationY));
      rotationZ = new QLineEdit(QString::number(srt.rotationZ));
      lineLayout_r->addWidget(rotation_text);
      lineLayout_r->addWidget(rotationX);
      lineLayout_r->addWidget(rotationY);
      lineLayout_r->addWidget(rotationZ);

      // S
      QWidget* lineWidget_s = new QWidget;
      QHBoxLayout* lineLayout_s = new QHBoxLayout;
      lineWidget_s->setLayout(lineLayout_s);
      QLabel* scale_text = new QLabel("Scale:");
      scaleX = new QLineEdit(QString::number(srt.scaleX));
      scaleY = new QLineEdit(QString::number(srt.scaleY));
      scaleZ = new QLineEdit(QString::number(srt.scaleZ));
      lineLayout_s->addWidget(scale_text);
      lineLayout_s->addWidget(scaleX);
      lineLayout_s->addWidget(scaleY);
      lineLayout_s->addWidget(scaleZ);
      grp_layout->addWidget(lineWidget_r);
      grp_layout->addWidget(lineWidget_s);
      grp_layout->addWidget(lineWidget_t);
      // connect
      QObject::connect(translationX, SIGNAL(returnPressed()), this, SLOT(updateProperties()));
      QObject::connect(translationY, SIGNAL(returnPressed()), this, SLOT(updateProperties()));
      QObject::connect(translationZ, SIGNAL(returnPressed()), this, SLOT(updateProperties()));
      QObject::connect(rotationX, SIGNAL(returnPressed()), this, SLOT(updateProperties()));
      QObject::connect(rotationY, SIGNAL(returnPressed()), this, SLOT(updateProperties()));
      QObject::connect(rotationZ, SIGNAL(returnPressed()), this, SLOT(updateProperties()));
      QObject::connect(scaleX, SIGNAL(returnPressed()), this, SLOT(updateProperties()));
      QObject::connect(scaleY, SIGNAL(returnPressed()), this, SLOT(updateProperties()));
      QObject::connect(scaleZ, SIGNAL(returnPressed()), this, SLOT(updateProperties()));
}

void Transform_Ui::updateProperties() {
      // construct matrix from SRT
      Matrix4 m = srt.toMatrix4();
      *_m = Transform(m);
}

void Point3f_Ui::updateProperties() {
      *_m = Point3f(posX->text().toFloat(), posY->text().toFloat(), posZ->text().toFloat());
}

void Point3f_Ui::addProperties(QWidget * parent) const
{
      QWidget* lineWidget_t = new QWidget;
      QHBoxLayout* lineLayout_t = new QHBoxLayout;
      lineWidget_t->setLayout(lineLayout_t);
      QLabel* pos_text = new QLabel("Position:");
      posX = new QLineEdit(QString::number(_m->x));
      posY = new QLineEdit(QString::number(_m->y));
      posZ = new QLineEdit(QString::number(_m->z));
      lineLayout_t->addWidget(pos_text);
      lineLayout_t->addWidget(posX);
      lineLayout_t->addWidget(posY);
      lineLayout_t->addWidget(posZ);
      parent->layout()->addWidget(lineWidget_t);
      // connect
      QObject::connect(posX, SIGNAL(returnPressed()), this, SLOT(updateProperties()));
      QObject::connect(posY, SIGNAL(returnPressed()), this, SLOT(updateProperties()));
      QObject::connect(posZ, SIGNAL(returnPressed()), this, SLOT(updateProperties()));
}
