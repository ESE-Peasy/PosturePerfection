#include "mainwindow.h"

namespace GUI {

void Label::constructor(const QString &title, const QString &subtitle,
                        QWidget *parent) {
  this->title = title;
  this->subtitle = subtitle;
  setMinimumSize(200, 80);
  setMaximumSize(200, 80);
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
}

void Label::paintEvent(QPaintEvent *p) {
  QWidget::paintEvent(p);
  QPainter paint(this);
  paint.setPen(QColor("white"));

  QFont titleFont = QApplication::font();
  titleFont.setBold(true);
  titleFont.setPixelSize(height() / 5);

  if (subtitle.length() > 0) {
    QFont subtitleFont = QApplication::font();
    subtitleFont.setPixelSize(height() / 8);

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

Button::Button(const QString &title, QWidget *parent)
    : QPushButton(parent), title(title), subtitle("") {
  setMinimumSize(200, 80);
}

Button::Button(const QString &title, const QString &subtitle, QWidget *parent)
    : QPushButton(parent), title(title), subtitle(subtitle) {
  setMinimumSize(200, 80);
}

void Button::paintEvent(QPaintEvent *p) {
  QPushButton::paintEvent(p);
  QPainter paint(this);
  paint.setPen(QColor("white"));

  QFont titleFont = QApplication::font();
  titleFont.setBold(true);
  titleFont.setPixelSize(height() / 5);

  if (subtitle.length() > 0) {
    QFont subtitleFont = QApplication::font();
    subtitleFont.setPixelSize(height() / 8);

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
