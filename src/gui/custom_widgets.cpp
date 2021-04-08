#include "mainwindow.h"

#define POINT_SIZE 20

namespace GUI {

/**
 * @brief Calls `setPointSize()` for the given font
 *
 * This function also ensures the text fits within the given bounds and performs
 * resizing as needed. The function works for lines with newline characters in
 * that it resizes to fit the longest line in the text string.
 *
 * @param font `QFont`
 * @param text `QString&` text that will be written with the current font
 * @param pointSize `int` desired point size
 * @param boxWidth `int` width of the container as returned by `Widget.width()`
 * @param padding `size_t` Desired padding (in pixels) around the text
 */
void setPointSizeAndResize(QFont *font, QString &text, int pointSize,
                           int boxWidth, size_t padding) {
  font->setPointSize(pointSize);
  QFontMetrics fm(*font);
  int pixelWidth = 0;
  // Subtitles may be broken over multiple lines and we only need the longest of
  // these measurements
  for (auto line : text.split("\n")) {
    if (fm.width(line) > pixelWidth) {
      pixelWidth = fm.width(line);
    }
  }
  boxWidth = boxWidth - padding * 2;
  if (pixelWidth > boxWidth) {
    font->setPointSize((pointSize * boxWidth) / pixelWidth);
  }
}

void Label::constructor(const QString &title, const QString &subtitle,
                        QWidget *parent) {
  this->title = title;
  this->subtitle = subtitle;
  setMinimumSize(200, 80);
}

Label::Label(QWidget *parent) : QLabel(parent) { constructor("", ""); }

Label::Label(const QString &title, QWidget *parent) : QLabel(parent) {
  constructor(title, "");
}

Label::Label(const QString &title, const QString &subtitle, QWidget *parent)
    : QLabel(parent) {
  constructor(title, subtitle);
}

void Label::setText(const QString &title) { setText(title, ""); }

void Label::setText(const QString &title, const QString &subtitle) {
  this->title = title;
  this->subtitle = subtitle;
  repaint();
}

void Label::paintEvent(QPaintEvent *p) {
  QWidget::paintEvent(p);
  QPainter paint(this);
  paint.setPen(QColor("white"));

  QFont titleFont = QApplication::font();
  titleFont.setBold(true);
  setPointSizeAndResize(&titleFont, title, POINT_SIZE, width(), padding);

  if (subtitle.length() > 0) {
    QFont subtitleFont = QApplication::font();
    setPointSizeAndResize(&subtitleFont, subtitle, (3 * POINT_SIZE) / 5,
                          width(), padding);

    paint.setFont(subtitleFont);
    paint.drawText(QRectF(QPoint(padding, padding),
                          QPoint(width() - padding, height() - padding)),
                   Qt::AlignHCenter | Qt::AlignBottom, subtitle);
    paint.setFont(titleFont);
    paint.drawText(QRectF(QPoint(padding, padding),
                          QPoint(width() - padding, height() - padding)),
                   Qt::AlignHCenter | Qt::AlignTop, title);
  } else {
    paint.setFont(titleFont);
    paint.drawText(QRectF(QPoint(padding, padding),
                          QPoint(width() - padding, height() - padding)),
                   Qt::AlignCenter, title);
  }
}

void Button::constructor(const QString &title, const QString &subtitle,
                         QWidget *parent) {
  this->title = title;
  this->subtitle = subtitle;
  setMinimumSize(200, 80);
}

Button::Button(const QString &title, QWidget *parent) : QPushButton(parent) {
  constructor(title, "");
}

Button::Button(const QString &title, const QString &subtitle, QWidget *parent)
    : QPushButton(parent) {
  constructor(title, subtitle);
}

void Button::setText(const QString &title) { setText(title, ""); }

void Button::setText(const QString &title, const QString &subtitle) {
  this->title = title;
  this->subtitle = subtitle;
  repaint();
}

void Button::paintEvent(QPaintEvent *p) {
  QPushButton::paintEvent(p);
  QPainter paint(this);
  paint.setPen(QColor("white"));

  QFont titleFont = QApplication::font();
  titleFont.setBold(true);
  setPointSizeAndResize(&titleFont, title, POINT_SIZE, width(), padding);

  if (subtitle.length() > 0) {
    QFont subtitleFont = QApplication::font();
    setPointSizeAndResize(&subtitleFont, subtitle, (3 * POINT_SIZE) / 5,
                          width(), padding);

    paint.setFont(subtitleFont);
    paint.drawText(QRectF(QPoint(padding, padding),
                          QPoint(width() - padding, height() - padding)),
                   Qt::AlignHCenter | Qt::AlignBottom, subtitle);

    paint.setFont(titleFont);
    paint.drawText(QRectF(QPoint(padding, padding),
                          QPoint(width() - padding, height() - padding)),
                   Qt::AlignHCenter | Qt::AlignTop, title);
  } else {
    paint.setFont(titleFont);
    paint.drawText(QRectF(QPoint(padding, padding),
                          QPoint(width() - padding, height() - padding)),
                   Qt::AlignCenter, title);
  }
}

}  // namespace GUI
