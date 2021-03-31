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
#include <QDialog>
#include <QFrame>
#include <QInputDialog>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QPushButton>
#include <QRect>
#include <QStackedWidget>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QtCore/QVariant>
#include <opencv2/imgcodecs.hpp>
#include "opencv2/opencv.hpp"
#include <QGroupBox>
#include <QStandardItemModel>
#include <QComboBox>
#include <QTime>
#include <QDate>
#include <QTableView>
#include <QTimer>

#include "../intermediate_structures.h"
#include "../pipeline.h"
#include "../posture_estimator.h"

QT_BEGIN_NAMESPACE

/**
 * @brief All code related to the graphical user interface for
 * PosturePerfection
 *
 */
namespace GUI {

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
   * @brief Refresh the contents of the data table with the most recent data
   *
   * @param pose_status a `PostureEstimating::PoseStatus` structure
   */
  void updateTable(PostureEstimating::PoseStatus pose_status);

  /**
   * @brief Refresh the video feed to the most recent frame
   *
   * @param currentFrame a `cv::Mat` object
   */
  void updateFrame(cv::Mat currentFrame);

  /**
   * @brief Creates the main page within the application
   *
   */
  void createMainPage();

  /**
   * @brief Creates the settings page within the application
   *
   */
  void createSettingsPage(Pipeline::Pipeline *pipeline);

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

 signals:

 private:
  /**
   * @brief Create a table containing the numeric posture values
   */
  void generateTable();

  /**
   * @brief Create the inital video feed frame with the PosturePerfection logo
   */
  void initalFrame();

  Pipeline::Pipeline *pipelinePtr = nullptr;
  PostureEstimating::PoseStatus currentPoseStatus;
  QGridLayout *framerate = new QGridLayout;
  QLabel *currentFrame = new QLabel();

  QGridLayout *mainLayout = new QGridLayout;
  QWidget *central = new QWidget;
  QGroupBox *groupBoxButtons = new QGroupBox();
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
