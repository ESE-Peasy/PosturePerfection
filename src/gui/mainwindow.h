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

#include <qcustomplot.h>

#include <QApplication>
#include <QDialog>
#include <QFrame>
#include <QInputDialog>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QPushButton>
#include <QRect>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QtCore/QVariant>
#include <opencv2/imgcodecs.hpp>
#include <QStackedWidget>

#include "../intermediate_structures.h"
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
  MainWindow(QWidget *parent = 0);  // NOLINT [runtime/explicit]
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

 public slots:
  void slotDisplayFen();

 signals:
  void changeStackedWidgetIndex(int);

 private:
  QGridLayout *mainLayout = new QGridLayout;
  QWidget *central = new QWidget;
  QGroupBox *groupBoxButtons = new QGroupBox();
  QStandardItemModel *model = new QStandardItemModel();
  void generateTable();
  void initalFrame();
  QLabel *frame = new QLabel();
  QStackedWidget *stackedWidget = new QStackedWidget;


 private slots:
  /**
   * @brief Displays the current date and time.
   */
  void showDateTime();

  /**
   * @brief Navigates to the settings page.
   */
  void on_settingsButton_clicked();
};
}  // namespace GUI
QT_END_NAMESPACE
#endif  // SRC_GUI_MAINWINDOW_H_
