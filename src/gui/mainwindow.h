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

#include <QMainWindow>
#include <QInputDialog>
#include <qcustomplot.h>
#include <QtCore/QVariant>
#include <QApplication>
#include <QFrame>
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QRect>
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace GUi {
class MainWindow;
}
QT_END_NAMESPACE

/*
 * @brief This class allows for navigation around the application from the main page.
 *
 * This class lets the user navigate to the data page, settings page and fullscreen page from the main window.
 *
 * This class handles:
 * Navigating to the data page.
 * Navigating to the settings page.
 * Navigating to the full screen.
 * Triggering posture reseting functionality
 *
 */
class MainWindow : public QMainWindow{
  Q_OBJECT

 public:
   /*
      * @brief Initialises the main page.
      * 
      * @param *parent Pointer to the parent interface.
      */
  MainWindow(QWidget *parent = 0);

 private:
  QGridLayout *mainLayout = new QGridLayout;
  QWidget *central = new QWidget;
  QGroupBox *groupBoxButtons = new QGroupBox();

 private slots:
  /*
    * @brief Displays the current date and time.
    */
  void showDateTime();
};

#endif  // SRC_GUI_MAINWINDOW_H_
