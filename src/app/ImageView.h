#pragma once

#include "Export.h"

#include <QColor>
#include <QImage>
#include <QMetaObject>
#include <QUrl>
#include <QWidget>

class QFileSystemWatcher;
class QGraphicsPixmapItem;
class QGraphicsScene;
class QLabel;
class QStackedWidget;

namespace mufpen {

class ImageViewCanvas;

// Standalone image preview surface. Owns the loaded pixels, viewport zoom/pan,
// rotation and reload-on-change behavior. Markdown image/Mermaid previews reuse
// the same widget in a short-lived dialog.
class MUFPEN_UI_EXPORT ImageView final : public QWidget {
  Q_OBJECT

public:
  explicit ImageView(QWidget* parent = nullptr);
  ~ImageView() override;

  bool loadFile(const QString& path, QString* error = nullptr);
  bool loadUrl(const QUrl& url, QString* error = nullptr);
  // Used by the Mermaid preview: show an already-rendered image.
  bool setImage(const QImage& image, const QString& label = {});
  bool hasImage() const { return !image_.isNull(); }
  QImage image() const { return image_; }
  QSize imageSize() const { return image_.size(); }
  QString sourcePath() const { return sourcePath_; }

  qreal zoomPercent() const { return zoom_ * 100.0; }
  bool fitMode() const { return fitMode_; }
  void setZoomPercent(qreal percent);
  void fitToWindow();
  void actualSize();
  void zoomIn();
  void zoomOut();
  void rotateLeft();
  void rotateRight();
  void reload();
  void setBackgroundColor(const QColor& color);

signals:
  void zoomChanged(qreal percent);
  void loadFailed(QString message);

private:
  friend class ImageViewCanvas;

  void applyPixmap();
  void updateStatus();
  void showError(const QString& message);
  void clearError();
  void watchSource();
  void applyZoom();
  void rotateBy(int degrees);

  ImageViewCanvas* canvas_ = nullptr;
  QGraphicsScene* scene_ = nullptr;
  QGraphicsPixmapItem* item_ = nullptr;
  QLabel* status_ = nullptr;
  QLabel* error_ = nullptr;
  QStackedWidget* stack_ = nullptr;
  QFileSystemWatcher* watcher_ = nullptr;
  QMetaObject::Connection remoteConnection_;
  QString sourcePath_;
  QUrl sourceUrl_;
  QString label_;
  QImage image_;
  QColor background_{255, 255, 255};
  qreal zoom_ = 1.0;
  int rotationDegrees_ = 0;
  bool fitMode_ = true;
};

} // namespace mufpen
