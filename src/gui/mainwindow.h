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
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QRect>




QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow{
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = 0);


 private:
  /*
  QWidget *centralWidget;
  QVBoxLayout *verticalLayout;
  QFrame *frame_2;
  QVBoxLayout *verticalLayout_3;
  QCustomPlot *customPlot;
  QFrame *frame;
  QVBoxLayout *verticalLayout_2;
  QLabel *label;
  QMenuBar *menuBar;
  QStatusBar *statusBar;
  */   
  QPushButton *m_button;

 private slots:
  void handleButton();

  

};

#endif  // SRC_GUI_MAINWINDOW_H_
