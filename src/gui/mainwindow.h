/**
 * @file mainwindow.h
 * @brief Header file for the main window
 *
 * @copyright Copyright (C) 2021  Andrew Ritchie
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
#ifndef SRC_GUI_MAINWINDOW_H_
#define SRC_GUI_MAINWINDOW_H_

#include <QApplication>
#include <QComboBox>
#include <QDate>
#include <QDialog>
#include <QFrame>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QPainter>
#include <QPushButton>
#include <QRect>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QTableView>
#include <QTime>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QtCore/QVariant>
#include <opencv2/imgcodecs.hpp>

#include "../intermediate_structures.h"
#include "../pipeline.h"
#include "../posture_estimator.h"
#include "opencv2/opencv.hpp"

#define WIDGET_PADDING 10

QT_BEGIN_NAMESPACE

/**
 * @brief All code related to the graphical user interface for
 * PosturePerfection
 *
 */
namespace GUI {

class Button : public QPushButton {
  Q_OBJECT
 private:
  const QString title;
  const QString subtitle;
  size_t padding = WIDGET_PADDING;

 public:
  Button(const QString &title, QWidget *parent = 0)
      : QPushButton(parent), title(title), subtitle("") {
    setMinimumSize(200, 80);
  }

  Button(const QString &title, const QString &subtitle, QWidget *parent = 0)
      : QPushButton(parent), title(title), subtitle(subtitle) {
    setMinimumSize(200, 80);
  }

  void paintEvent(QPaintEvent *p) {
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
};

class Label : public QLabel {
  Q_OBJECT
 private:
  QString title;
  QString subtitle;
  size_t padding = WIDGET_PADDING;

  void constructor(const QString &title, const QString &subtitle,
                   QWidget *parent = 0) {
    this->title = title;
    this->subtitle = subtitle;
    setMinimumSize(200, 80);
    setMaximumSize(200, 80);
    setStyleSheet("background-color: grey");
  }

 public:
  Label(QWidget *parent = 0) : QLabel(parent) { constructor("", ""); }

  Label(const QString &title, QWidget *parent = 0) : QLabel(parent) {
    constructor(title, "");
  }

  Label(const QString &title, const QString &subtitle, QWidget *parent = 0)
      : QLabel(parent) {
    constructor(title, subtitle);
  }

  void setText(const QString &title) { setText(title, ""); }

  void setText(const QString &title, const QString &subtitle) {
    this->title = title;
    this->subtitle = subtitle;
  }

  void paintEvent(QPaintEvent *p) {
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
};

/**
 * @brief Allows for navigation around the application from the main
 * page by letting the user navigate to the data page, settings page and
 * fullscreen page from the main window.
 *
 * This class handles:
 *  - Navigating to the data page.
 *  - Navigating to the settings page.
 *  - Navigating to the full screen.
 *  - Triggering posture reseting functionality
 *
 */
class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  /**
   * @brief Initialises the main page.
   *
   * @param *parent Pointer to the parent interface.
   */
  MainWindow(Pipeline::Pipeline *Pipeline,
             QWidget *parent = 0);  // NOLINT [runtime/explicit]
  ~MainWindow();

  /**
   * @brief Refresh the video feed to the most recent frame
   *
   * @param currentFrame a `cv::Mat` object
   */
  void emitNewFrame(cv::Mat currentFrame);

  /**
   * @brief Creates the main page within the application
   *
   */
  void createMainPage();

  /**
   * @brief Creates the settings page within the application
   *
   */
  void createSettingsPage();

  /**
   * @brief Updates the current pose
   *
   */
  void updatePose(PostureEstimating::PoseStatus poseStatus);

  /**
   * @brief Outputs the framerate to the GUI
   *
   */
  void setOutputFramerate();

 public slots:
  /**
   * @brief Updates the threshold value
   *
   */
  void setThresholdValue(int);

  /**
   * @brief Updates the `PostureEstimating::pose_change_threshold` value
   *
   */
  void setPoseChangeThresholdValue(int);

  /**
   * @brief Increases the video frame rate
   *
   */
  void increaseVideoFramerate();

  /**
   * @brief Decreases the video frame rate
   *
   */
  void decreaseVideoFramerate();

  /**
   * @brief sets the current pose to the ideal posture
   *
   */
  void setIdealPosture();

  /**
   * @brief update the video ouput once a new frame has been captured
   *
   */
  void updateVideoFrame(cv::Mat currentFrame);

  /**
   * @brief update the posture notification using the posture status "good"
   * value
   *
   * @param postureState the state of the posture currently captured
   */
  void updatePostureNotification(PostureEstimating::PostureState postureState);

  void openSettingsPage(void);

  void openMainPage(void);

 signals:
  /**
   * @brief emit the newly captured frame
   *
   * @param currentFrame cv::Mat object containing the current captured frame
   */
  void currentFrameSignal(cv::Mat currentFrame);

  /**
   * @brief emit the newly captured good posture value
   *
   * @param postureState the current posture state
   */
  void currentGoodBadPosture(PostureEstimating::PostureState postureState);

 private:
  /**
   * @brief Create the inital video feed frame with the PosturePerfection logo
   */
  void initalFrame();

  Pipeline::Pipeline *pipelinePtr = nullptr;
  PostureEstimating::PoseStatus currentPoseStatus;
  QGridLayout *framerate = new QGridLayout;
  QLabel *currentFrame = new QLabel();

  Label *postureNotification = new Label();

  QGridLayout *mainLayout = new QGridLayout;
  QWidget *central = new QWidget;
  QStandardItemModel *model = new QStandardItemModel();
  QLabel *frame = new QLabel();
  QStackedWidget *stackedWidget = new QStackedWidget;
  QComboBox *pageComboBox = new QComboBox;
  QGroupBox *groupDateTime = new QGroupBox();
  QVBoxLayout *dtBox = new QVBoxLayout;

  QWidget *firstPageWidget = new QWidget;
  QGridLayout *mainPageLayout = new QGridLayout;

  QWidget *secondPageWidget = new QWidget;
  QGridLayout *settingsPageLayout = new QGridLayout;

  QWidget *thirdPageWidget = new QWidget;

 private slots:
  /**
   * @brief Displays the current date and time.
   */
  void showDateTime();
};
}  // namespace GUI
QT_END_NAMESPACE
#endif  // SRC_GUI_MAINWINDOW_H_
