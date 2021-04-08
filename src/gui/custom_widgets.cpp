#include "mainwindow.h"

#define POINT_SIZE 20

namespace GUI {

void setPointSizeAndResize(QFont *font, QString &text, int pointSize,
                           int boxWidth, size_t padding) {
  font->setPointSize(pointSize);
  QFontMetrics fm(*font);
  int pixelWidth = 0;
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
