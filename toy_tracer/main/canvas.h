#pragma once
#include "core/geometry.h"
#include "core/transform.h"

#include <QtWidgets/qwidget.h>
#include <QWindow>
#include <QMouseEvent>

class Canvas : public QWindow {
      volatile bool _drag = false;
      volatile bool _resized = false;
      volatile bool _keyPressed = false;
      volatile bool _keyStatuses[4] = { false, false, false, false };
      Point2f _pos0;
      Transform _pendingRotation;
      bool camera_obj = true;
public:
      Canvas(int width, int height);
      Canvas() : Canvas(800, 800) {}
      bool resized() const { return _resized; }
      bool keyPressed() const { return _keyPressed; }
      void clearResized() { _resized = false; }
      bool drag() const { return _drag; }
      volatile bool* keyStatuses() { return &_keyStatuses[0]; }
      void clearDrag() { _drag = false; }
      void setControlCamera() { camera_obj = true; }
      void setControlObject() { camera_obj = false; }
      bool CameraorObject() { return camera_obj; }
protected:
      void mousePressEvent(QMouseEvent *ev) override;
      void mouseReleaseEvent(QMouseEvent *ev) override;
      void mouseMoveEvent(QMouseEvent *ev) override;
      void keyPressEvent(QKeyEvent *ev) override;
      void keyReleaseEvent(QKeyEvent *ev) override;
};

static void applyTranslation(Point3f& pos, const volatile bool* statuses, Float deltaT, Vector3f speedR, Vector3f speedU) {
      static Float offset = deltaT;
      if (statuses[0] && statuses[1])
            ; // Skiped
      else if (statuses[0])
            // Front
            pos += offset * speedU;
      else if (statuses[1])
            // Back
            pos -= offset * speedU;
      if (statuses[2] && statuses[3])
            ; // Skipped
      else if (statuses[2])
            // Left
            pos -= offset * speedR;
      else if (statuses[3])
            // Right
            pos += offset * speedR;
}