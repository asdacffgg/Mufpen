#include "app/ImageView.h"

#include "render/ImageDecoder.h"
#include "render/ImageLoader.h"

#include <QBrush>
#include <QDir>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QKeySequence>
#include <QMouseEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QResizeEvent>
#include <QScrollBar>
#include <QShortcut>
#include <QStackedWidget>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWheelEvent>

#include <algorithm>
#include <cmath>
#include <functional>

namespace {

QPixmap checkerboard() {
  QPixmap tile(24, 24);
  tile.fill(QColor(255, 255, 255, 22));
  QPainter painter(&tile);
  painter.fillRect(0, 0, 12, 12, QColor(0, 0, 0, 22));
  painter.fillRect(12, 12, 12, 12, QColor(0, 0, 0, 22));
  return tile;
}

QColor contrastingText(const QColor& background) {
  return background.lightness() < 128 ? QColor(235, 235, 235) : QColor(32, 34, 38);
}

} // namespace

class mufpen::ImageViewCanvas final : public QGraphicsView {
public:
  explicit ImageViewCanvas(ImageView* owner, QWidget* parent = nullptr) : QGraphicsView(parent), owner_(owner) {
    setObjectName(QStringLiteral("ImageViewCanvas"));
    setFrameShape(QFrame::NoFrame);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setAlignment(Qt::AlignCenter);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setBackgroundBrush(Qt::NoBrush);
    setFocusPolicy(Qt::StrongFocus);
  }

protected:
  void wheelEvent(QWheelEvent* event) override {
    if (event->modifiers().testFlag(Qt::ControlModifier)) {
      if (event->angleDelta().y() >= 0)
        owner_->zoomIn();
      else
        owner_->zoomOut();
      event->accept();
      return;
    }
    QGraphicsView::wheelEvent(event);
  }

  void mouseDoubleClickEvent(QMouseEvent* event) override {
    if (event->button() == Qt::LeftButton) {
      if (owner_->fitMode())
        owner_->actualSize();
      else
        owner_->fitToWindow();
      event->accept();
      return;
    }
    QGraphicsView::mouseDoubleClickEvent(event);
  }

  void resizeEvent(QResizeEvent* event) override {
    QGraphicsView::resizeEvent(event);
    if (owner_->fitMode() && owner_->hasImage()) {
      QTimer::singleShot(0, owner_, [owner = owner_] {
        if (owner->fitMode() && owner->hasImage())
          owner->fitToWindow();
      });
    }
  }

  void drawBackground(QPainter* painter, const QRectF& rect) override {
    painter->fillRect(rect, owner_->background_);
    if (owner_->image_.hasAlphaChannel()) {
      static const QPixmap tile = checkerboard();
      painter->fillRect(rect, QBrush(tile));
    }
  }

private:
  ImageView* owner_ = nullptr;
};

mufpen::ImageView::ImageView(QWidget* parent) : QWidget(parent) {
  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  auto* toolbar = new QWidget(this);
  toolbar->setObjectName(QStringLiteral("ImageViewToolbar"));
  auto* toolbarLayout = new QHBoxLayout(toolbar);
  toolbarLayout->setContentsMargins(8, 4, 8, 4);
  toolbarLayout->setSpacing(2);

  const auto addButton = [&](const QString& text, const QString& tooltip, const std::function<void()>& action) {
    auto* button = new QToolButton(toolbar);
    button->setText(text);
    button->setToolTip(tooltip);
    button->setAutoRaise(true);
    button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    button->setFocusPolicy(Qt::NoFocus);
    QObject::connect(button, &QToolButton::clicked, this, action);
    toolbarLayout->addWidget(button);
    return button;
  };
  addButton(tr("Fit"), tr("Fit to window (Ctrl+0)"), [this] { fitToWindow(); });
  addButton(tr("100%"), tr("Actual size (Ctrl+1)"), [this] { actualSize(); });
  addButton(QStringLiteral("−"), tr("Zoom out (Ctrl+-)"), [this] { zoomOut(); });
  addButton(QStringLiteral("+"), tr("Zoom in (Ctrl++)"), [this] { zoomIn(); });
  addButton(QStringLiteral("↺"), tr("Rotate left"), [this] { rotateLeft(); });
  addButton(QStringLiteral("↻"), tr("Rotate right"), [this] { rotateRight(); });
  addButton(tr("Reload"), tr("Reload image"), [this] { reload(); });
  toolbarLayout->addStretch(1);

  status_ = new QLabel(toolbar);
  status_->setObjectName(QStringLiteral("ImageViewStatus"));
  status_->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
  toolbarLayout->addWidget(status_);
  layout->addWidget(toolbar);

  stack_ = new QStackedWidget(this);
  stack_->setObjectName(QStringLiteral("ImageViewStack"));
  canvas_ = new ImageViewCanvas(this, stack_);
  scene_ = new QGraphicsScene(this);
  canvas_->setScene(scene_);
  item_ = scene_->addPixmap(QPixmap());
  item_->setTransformationMode(Qt::SmoothTransformation);
  stack_->addWidget(canvas_);

  error_ = new QLabel(stack_);
  error_->setObjectName(QStringLiteral("ImageViewError"));
  error_->setAlignment(Qt::AlignCenter);
  error_->setWordWrap(true);
  error_->setTextFormat(Qt::PlainText);
  stack_->addWidget(error_);
  stack_->setCurrentWidget(canvas_);
  layout->addWidget(stack_, 1);

  watcher_ = new QFileSystemWatcher(this);
  connect(watcher_, &QFileSystemWatcher::fileChanged, this, [this] {
    QTimer::singleShot(100, this, [this] {
      if (!sourcePath_.isEmpty())
        loadFile(sourcePath_);
    });
  });
  connect(watcher_, &QFileSystemWatcher::directoryChanged, this, [this] {
    QTimer::singleShot(100, this, [this] {
      if (!sourcePath_.isEmpty() && QFileInfo::exists(sourcePath_))
        loadFile(sourcePath_);
    });
  });

  new QShortcut(QKeySequence(QStringLiteral("Ctrl+0")), this, [this] { fitToWindow(); });
  new QShortcut(QKeySequence(QStringLiteral("Ctrl+1")), this, [this] { actualSize(); });
  new QShortcut(QKeySequence(QStringLiteral("Ctrl++")), this, [this] { zoomIn(); });
  new QShortcut(QKeySequence(QStringLiteral("Ctrl+=")), this, [this] { zoomIn(); });
  new QShortcut(QKeySequence(QStringLiteral("Ctrl+-")), this, [this] { zoomOut(); });

  setBackgroundColor(background_);
  updateStatus();
  setMinimumSize(320, 240);
}

mufpen::ImageView::~ImageView() = default;

bool mufpen::ImageView::loadFile(const QString& path, QString* error) {
  const QFileInfo info(path);
  if (!info.isFile() || !info.isReadable()) {
    const QString message = tr("Image file not found: %1").arg(QDir::toNativeSeparators(path));
    if (error)
      *error = message;
    image_ = QImage();
    sourcePath_.clear();
    sourceUrl_ = QUrl();
    applyPixmap();
    showError(message);
    return false;
  }
  QImage image(path);
  if (image.isNull())
    image = image_decoder::decodeFileFallback(info.absoluteFilePath());
  if (image.isNull()) {
    const QString message = tr("Could not decode image: %1").arg(QDir::toNativeSeparators(path));
    if (error)
      *error = message;
    image_ = QImage();
    sourcePath_.clear();
    sourceUrl_ = QUrl();
    applyPixmap();
    showError(message);
    return false;
  }
  sourcePath_ = info.absoluteFilePath();
  sourceUrl_ = QUrl::fromLocalFile(sourcePath_);
  label_ = info.fileName();
  image_ = image;
  applyPixmap();
  clearError();
  fitToWindow();
  watchSource();
  return true;
}

bool mufpen::ImageView::loadUrl(const QUrl& url, QString* error) {
  if (url.isLocalFile())
    return loadFile(url.toLocalFile(), error);
  if (url.scheme() == QStringLiteral("data")) {
    const QImage image = image_decoder::decodeDataUri(url.toString());
    if (image.isNull()) {
      const QString message = tr("Could not decode image data");
      if (error)
        *error = message;
      showError(message);
      return false;
    }
    sourcePath_.clear();
    sourceUrl_ = url;
    label_ = tr("Image");
    image_ = image;
    applyPixmap();
    clearError();
    fitToWindow();
    return true;
  }
  if (url.scheme() == QStringLiteral("http") || url.scheme() == QStringLiteral("https")) {
    sourcePath_.clear();
    sourceUrl_ = url;
    label_ = url.fileName().isEmpty() ? tr("Remote image") : url.fileName();
    if (remoteConnection_)
      QObject::disconnect(remoteConnection_);
    remoteConnection_ =
        connect(&ImageLoader::instance(), &ImageLoader::imageReady, this, [this, url](const QString& ready) {
          if (ready != url.toString())
            return;
          const QImage image = ImageLoader::instance().cached(ready);
          if (image.isNull())
            showError(tr("Could not load remote image: %1").arg(url.toString()));
          else
            setImage(image, url.fileName());
        });
    const QImage cached = ImageLoader::instance().image(url.toString());
    if (!cached.isNull()) {
      setImage(cached, label_);
      return true;
    }
    showError(tr("Loading image…"));
    return true;
  }
  const QString message = tr("Unsupported image URL: %1").arg(url.toString());
  if (error)
    *error = message;
  showError(message);
  return false;
}

bool mufpen::ImageView::setImage(const QImage& image, const QString& label) {
  if (image.isNull())
    return false;
  sourcePath_.clear();
  sourceUrl_ = QUrl();
  label_ = label;
  image_ = image;
  applyPixmap();
  clearError();
  fitToWindow();
  return true;
}

void mufpen::ImageView::applyPixmap() {
  if (image_.isNull()) {
    item_->setPixmap(QPixmap());
    scene_->setSceneRect(QRectF());
    return;
  }
  item_->setPixmap(QPixmap::fromImage(image_));
  item_->setTransformOriginPoint(item_->boundingRect().center());
  item_->setRotation(rotationDegrees_);
  scene_->setSceneRect(item_->sceneBoundingRect());
}

void mufpen::ImageView::setBackgroundColor(const QColor& color) {
  background_ = color.isValid() ? color : QColor(255, 255, 255);
  QPalette colors = palette();
  colors.setColor(QPalette::Window, background_);
  colors.setColor(QPalette::Base, background_);
  colors.setColor(QPalette::WindowText, contrastingText(background_));
  colors.setColor(QPalette::Text, contrastingText(background_));
  colors.setColor(QPalette::ButtonText, contrastingText(background_));
  setPalette(colors);
  if (canvas_)
    canvas_->viewport()->update();
}

void mufpen::ImageView::fitToWindow() {
  if (!hasImage() || !canvas_ || !item_)
    return;
  const QRectF bounds = item_->sceneBoundingRect();
  if (bounds.isEmpty())
    return;
  const QSize viewport = canvas_->viewport()->size();
  if (viewport.width() <= 1 || viewport.height() <= 1)
    return;
  const qreal scale =
      std::min({1.0, (viewport.width() - 2.0) / bounds.width(), (viewport.height() - 2.0) / bounds.height()});
  zoom_ = std::max<qreal>(0.01, scale);
  fitMode_ = true;
  canvas_->resetTransform();
  canvas_->scale(zoom_, zoom_);
  canvas_->centerOn(item_);
  updateStatus();
  emit zoomChanged(zoomPercent());
}

void mufpen::ImageView::actualSize() {
  if (!hasImage())
    return;
  zoom_ = 1.0;
  fitMode_ = false;
  applyZoom();
}

void mufpen::ImageView::setZoomPercent(qreal percent) {
  if (!hasImage())
    return;
  zoom_ = std::clamp(percent / 100.0, 0.05, 20.0);
  fitMode_ = false;
  applyZoom();
}

void mufpen::ImageView::zoomIn() {
  setZoomPercent(zoomPercent() * 1.25);
}

void mufpen::ImageView::zoomOut() {
  setZoomPercent(zoomPercent() / 1.25);
}

void mufpen::ImageView::applyZoom() {
  canvas_->resetTransform();
  canvas_->scale(zoom_, zoom_);
  canvas_->centerOn(item_);
  updateStatus();
  emit zoomChanged(zoomPercent());
}

void mufpen::ImageView::rotateLeft() {
  rotateBy(-90);
}

void mufpen::ImageView::rotateRight() {
  rotateBy(90);
}

void mufpen::ImageView::rotateBy(int degrees) {
  if (!hasImage())
    return;
  rotationDegrees_ = (rotationDegrees_ + degrees) % 360;
  if (rotationDegrees_ < 0)
    rotationDegrees_ += 360;
  item_->setRotation(rotationDegrees_);
  scene_->setSceneRect(item_->sceneBoundingRect());
  if (fitMode_)
    fitToWindow();
  else
    applyZoom();
}

void mufpen::ImageView::reload() {
  if (!sourcePath_.isEmpty()) {
    loadFile(sourcePath_);
    return;
  }
  if (!sourceUrl_.isEmpty()) {
    loadUrl(sourceUrl_);
    return;
  }
  applyPixmap();
  updateStatus();
}

void mufpen::ImageView::updateStatus() {
  if (!status_)
    return;
  if (!hasImage()) {
    status_->clear();
    return;
  }
  QString text = QStringLiteral("%1 × %2  ·  %3%").arg(image_.width()).arg(image_.height()).arg(qRound(zoomPercent()));
  if (!label_.isEmpty() && label_ != sourcePath_)
    text = label_ + QStringLiteral("  ·  ") + text;
  status_->setText(text);
}

void mufpen::ImageView::showError(const QString& message) {
  if (!error_)
    return;
  error_->setText(message);
  stack_->setCurrentWidget(error_);
}

void mufpen::ImageView::clearError() {
  if (stack_)
    stack_->setCurrentWidget(canvas_);
}

void mufpen::ImageView::watchSource() {
  if (!watcher_)
    return;
  if (!watcher_->files().isEmpty())
    watcher_->removePaths(watcher_->files());
  if (!watcher_->directories().isEmpty())
    watcher_->removePaths(watcher_->directories());
  if (sourcePath_.isEmpty())
    return;
  if (QFileInfo::exists(sourcePath_))
    watcher_->addPath(sourcePath_);
  watcher_->addPath(QFileInfo(sourcePath_).absolutePath());
}
