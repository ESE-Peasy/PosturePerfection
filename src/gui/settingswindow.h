/*
 * @file settingswindow.h
 * @brief Interface for representation of user's pose
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

#ifndef SETTINGSWINDOW_H
#define SETTINGSWINODW_H

#include <QMainWindow>

namespace Ui {
class SettingsWindow;
}

/*
 * @brief This class allows for navigation around the application from the
 * fullscreen page.
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

class SettingsWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit SettingsWindow(QWidget *parent = nullptr);
  ~SettingsWindow();
  QGridLayout *mainLayout = new QGridLayout;
  QWidget *central = new QWidget;
  QGroupBox *groupBoxButtons = new QGroupBox();
  QStandardItemModel *model = new QStandardItemModel();
  int getData();

 private slots:
  /**
   * @brief Displays the current date and time.
   */
  void showDateTime();

  void on_mainButton_clicked();

 private:
};

#endif  // SETTINGSWINDOW_H
