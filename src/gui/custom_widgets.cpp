/**
 * @copyright Copyright (C) 2021  Miklas Riechmann
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "mainwindow.h"

#define POINT_SIZE 20
#define POINT_SIZE_SUB ((4 * POINT_SIZE) / 5)

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
void setPointSizeAndResize(QFont *font, QString *text, float pointSize,
                           QRectF rect) {
  font->setPointSizeF(pointSize);
  QFontMetrics fm(*font);
  int pixelWidth = 0;
  int pixelHeight = 0;
  // Subtitles may be broken over multiple lines and we need the widest line and
  // overall height
  for (auto line : text->split("\n")) {
    if (fm.width(line) > pixelWidth) {
      pixelWidth = fm.width(line);
    }
    pixelHeight += fm.lineSpacing();
  }

  auto boxWidth = rect.width();
  auto boxHeight = rect.height();

  if ((pixelWidth > boxWidth) && (pixelHeight > boxHeight)) {
    // If both dimensions are too large decrease by the one that requires the
    // largest change
    if (boxHeight / pixelHeight < boxWidth / pixelWidth) {
      font->setPointSize((pointSize * boxHeight) / pixelHeight);
    } else {
      font->setPointSize((pointSize * boxWidth) / pixelWidth);
    }
  } else if (pixelWidth > boxWidth) {
    // If too wide, scale down
    font->setPointSize((pointSize * boxWidth) / pixelWidth);
  } else if (pixelHeight > boxHeight) {
    // If too tall, scale down
    font->setPointSize((pointSize * boxHeight) / pixelHeight);
  }
}

QRectF fullRect(int width, int height, size_t padding) {
  return QRectF(QPoint(padding, padding),
                QPoint(width - padding, height - padding));
}

QRectF halfRectTop(int width, int height, size_t padding) {
  return QRectF(QPoint(padding, padding),
                QPoint(width - padding, (height - padding) / 2));
}

QRectF halfRectBot(int width, int height, size_t padding) {
  return QRectF(QPoint(padding, (height + padding) / 2),
                QPoint(width - padding, height - padding));
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

  if (subtitle.length() > 0) {
    auto titleRect = halfRectTop(width(), height(), padding);
    setPointSizeAndResize(&titleFont, &title, POINT_SIZE, titleRect);
    paint.setFont(titleFont);
    paint.drawText(titleRect, Qt::AlignCenter, title);

    auto subtitleRect = halfRectBot(width(), height(), padding);
    QFont subtitleFont = QApplication::font();
    setPointSizeAndResize(&subtitleFont, &subtitle, POINT_SIZE_SUB,
                          subtitleRect);
    paint.setFont(subtitleFont);
    paint.drawText(subtitleRect, Qt::AlignCenter, subtitle);
  } else {
    auto titleRect = fullRect(width(), height(), padding);
    setPointSizeAndResize(&titleFont, &title, POINT_SIZE, titleRect);
    paint.setFont(titleFont);
    paint.drawText(titleRect, Qt::AlignCenter, title);
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

  if (subtitle.length() > 0) {
    auto titleRect = halfRectTop(width(), height(), padding);
    setPointSizeAndResize(&titleFont, &title, POINT_SIZE, titleRect);
    paint.setFont(titleFont);
    paint.drawText(titleRect, Qt::AlignCenter, title);

    auto subtitleRect = halfRectBot(width(), height(), padding);
    QFont subtitleFont = QApplication::font();
    setPointSizeAndResize(&subtitleFont, &subtitle, POINT_SIZE_SUB,
                          subtitleRect);
    paint.setFont(subtitleFont);
    paint.drawText(subtitleRect, Qt::AlignCenter, subtitle);
  } else {
    auto titleRect = fullRect(width(), height(), padding);
    setPointSizeAndResize(&titleFont, &title, POINT_SIZE, titleRect);
    paint.setFont(titleFont);
    paint.drawText(titleRect, Qt::AlignCenter, title);
  }
}

}  // namespace GUI
