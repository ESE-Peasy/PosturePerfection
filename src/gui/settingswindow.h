/**
 * @file settingswindow.h
 * @brief Header file for the settings GUI page
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

#include "../intermediate_structures.h"
#include "../posture_estimator.h"

#ifndef SRC_GUI_SETTINGSWINDOW_H_
#define SRC_GUI_SETTINGSWINDOW_H_

QT_BEGIN_NAMESPACE
namespace GUI {

/**
 * @brief This class allows for navigation around the application from the
 * settings page.
 *
 * This class lets the user navigate to the mainwindow, settingspage and
 * datapage from the fullscreen page.
 *
 * This class handles:
 * Navigating to the mainwindow.
 * Navigating to the settingspage.
 * Navigating to the datapage.
 * Triggering posture reseting functionality
 *
 *
 *
 */

class SettingsWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit SettingsWindow(QWidget *parent = nullptr);
  ~SettingsWindow();
  QGridLayout *mainLayout = new QGridLayout;
  QWidget *central = new QWidget;
  QGroupBox *groupBoxButtons = new QGroupBox();
  QStandardItemModel *model = new QStandardItemModel();

 private slots:
  /**
   * @brief Displays the current date and time.
   */
  void showDateTime();

  /**
   * @brief Navigates the user to the main window.
   */
  void on_mainButton_clicked();

 private:
};
}  // namespace GUI
QT_END_NAMESPACE
#endif  // SRC_GUI_SETTINGSWINDOW_H_
