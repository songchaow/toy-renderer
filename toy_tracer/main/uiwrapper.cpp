#include "main/uiwrapper.h"
#include "main/renderworker.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QDebug>
#include <QLabel>
#include <QCheckBox>
#include <QRadioButton>

void PBRMaterial_Ui::updateProperties() {
      if (!albedo_text->text().isEmpty())
            _m->albedo_map.resetImage(albedo_text->text().toStdString());
      if (!metallic_text->text().isEmpty())
            _m->metallicRoughnessMap.resetImage(metallic_text->text().toStdString());
      _m->setDirty();
}

void PBRMaterial_Ui::addProperties(QWidget* parent) const {
      //RendererObject::addProperties(parent);
      // TODO: add existing values
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
      albedo_text = RendererObject::addFileDialog("Albedo Map:", "Open", parent, albedo_path);
      metallic_text = RendererObject::addFileDialog("Metallic Map:", "Open:", parent, metallic_path);
      rough_text = RendererObject::addFileDialog("Rough Map:", "Open:", parent, rough_path);
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
      //shape.reset(_m->shape());
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
      _pos.addProperties(pl_group);
      _rgb.addProperties(pl_group);
      _dir.addProperties(pl_group);
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
      translationX = new QLineEdit(QString::number(srt.translation.x));
      translationY = new QLineEdit(QString::number(srt.translation.y));
      translationZ = new QLineEdit(QString::number(srt.translation.z));
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

void Vector3f_Ui::updateProperties() {
      *_m = Vector3f(posX->text().toFloat(), posY->text().toFloat(), posZ->text().toFloat());
}

void RGB_Spectrum_Ui::updateProperties() {
      *_m = RGBSpectrum(_r->text().toFloat(), _g->text().toFloat(), _b->text().toFloat());
}

void RGB_Spectrum_Ui::addProperties(QWidget* parent) const {
      QWidget* lineWidget_t = new QWidget;
      QHBoxLayout* lineLayout_t = new QHBoxLayout;
      lineWidget_t->setLayout(lineLayout_t);
      QLabel* pos_text = new QLabel("Color:");
      _r = new QLineEdit(QString::number(_m->rgb[0]));
      _g = new QLineEdit(QString::number(_m->rgb[1]));
      _b = new QLineEdit(QString::number(_m->rgb[2]));
      lineLayout_t->addWidget(_r);
      lineLayout_t->addWidget(_g);
      lineLayout_t->addWidget(_b);
      parent->layout()->addWidget(lineWidget_t);
      // connect
      QObject::connect(_r, &QLineEdit::returnPressed, this, &RGB_Spectrum_Ui::updateProperties);
      QObject::connect(_g, &QLineEdit::returnPressed, this, &RGB_Spectrum_Ui::updateProperties);
      QObject::connect(_b, &QLineEdit::returnPressed, this, &RGB_Spectrum_Ui::updateProperties);
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
      QObject::connect(posX, &QLineEdit::returnPressed, this, &Point3f_Ui::updateProperties);
      QObject::connect(posY, &QLineEdit::returnPressed, this, &Point3f_Ui::updateProperties);
      QObject::connect(posZ, &QLineEdit::returnPressed, this, &Point3f_Ui::updateProperties);
}

void Vector3f_Ui::addProperties(QWidget * parent) const
{
      QWidget* lineWidget_t = new QWidget;
      QHBoxLayout* lineLayout_t = new QHBoxLayout;
      lineWidget_t->setLayout(lineLayout_t);
      QLabel* pos_text = new QLabel("Direction:");
      posX = new QLineEdit(QString::number(_m->x));
      posY = new QLineEdit(QString::number(_m->y));
      posZ = new QLineEdit(QString::number(_m->z));
      lineLayout_t->addWidget(pos_text);
      lineLayout_t->addWidget(posX);
      lineLayout_t->addWidget(posY);
      lineLayout_t->addWidget(posZ);
      parent->layout()->addWidget(lineWidget_t);
      // connect
      QObject::connect(posX, &QLineEdit::returnPressed, this, &Vector3f_Ui::updateProperties);
      QObject::connect(posY, &QLineEdit::returnPressed, this, &Vector3f_Ui::updateProperties);
      QObject::connect(posZ, &QLineEdit::returnPressed, this, &Vector3f_Ui::updateProperties);
}

static void updateCameraSpeed(Float newSpeed) {
      RenderWorker::getCamera()->rspeed() = newSpeed;
}

void addHLayout(QWidget* parent, const std::vector<QWidget*>& elements) {
      QWidget* lineWidget = new QWidget;
      QHBoxLayout* lineLayout = new QHBoxLayout;
      lineWidget->setLayout(lineLayout);
      for (auto* e : elements) {
            lineLayout->addWidget(e);
      }
      parent->layout()->addWidget(lineWidget);
}

void addDefaultProperties(QWidget* parent) {
      QLabel* camSpeed_text = new QLabel("Camera speed:");
      QLineEdit* speed = new QLineEdit(QString::number(RenderWorker::getCamera()->rspeed()));
      addHLayout(parent, { camSpeed_text, speed });
      QObject::connect(speed, &QLineEdit::returnPressed, [=]() {
            Float newspeed = speed->text().toFloat();
            RenderWorker::getCamera()->rspeed() = newspeed;
      });
      QLabel* nearPlaneText = new QLabel("Camera near plane:");
      QLineEdit* nearPlane = new QLineEdit(QString::number(RenderWorker::getCamera()->nearPlane()));
      QLabel* farPlaneText = new QLabel("Camera far plane:");
      QLineEdit* farPlane = new QLineEdit(QString::number(RenderWorker::getCamera()->farPlane()));
      addHLayout(parent, { nearPlaneText, nearPlane });
      addHLayout(parent, { farPlaneText, farPlane });
      auto setNF = [=]() {
            Float near = nearPlane->text().toFloat();
            Float far = farPlane->text().toFloat();
            RenderWorker::getCamera()->setNearFar(near, far);
      };
      QObject::connect(nearPlane, &QLineEdit::returnPressed, setNF);
      QObject::connect(farPlane, &QLineEdit::returnPressed, setNF);
      QCheckBox* renderLight = new QCheckBox("Render point lights");
      renderLight->setCheckState(RenderWorker::Instance()->renderPointLight ? Qt::Checked : Qt::Unchecked);
      parent->layout()->addWidget(renderLight);
      QObject::connect(renderLight, &QCheckBox::stateChanged, [](int state) {
            RenderWorker::Instance()->renderPointLight = state == Qt::Checked;
      });
      QCheckBox* shadowMap = new QCheckBox("Enable shadow map");
      shadowMap->setCheckState(RenderWorker::Instance()->enableShadowMap ? Qt::Checked : Qt::Unchecked);
      parent->layout()->addWidget(shadowMap);
      QObject::connect(shadowMap, &QCheckBox::stateChanged, [](int state) {
            RenderWorker::Instance()->enableShadowMap = state == Qt::Checked;
      });
      QCheckBox* bloom = new QCheckBox("Enable Bloom");
      bloom->setCheckState(RenderWorker::Instance()->enableBloom ? Qt::Checked : Qt::Unchecked);
      parent->layout()->addWidget(bloom);
      QObject::connect(bloom, &QCheckBox::stateChanged, [](int state) {
            RenderWorker::Instance()->enableBloom = state == Qt::Checked;
      });
      QPushButton* refreshShader = new QPushButton("Refresh Shaders...");
      QObject::connect(bloom, &QPushButton::clicked, [](int state) {
            for (int i = 0; i < ShaderType::NUM_SHADER_TYPE; i++) {
                  ;
            }
      });
}
