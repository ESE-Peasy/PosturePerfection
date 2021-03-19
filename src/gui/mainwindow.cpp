/**
 * @file mainwindow.cpp
 * @brief Main user interface file
 *
 * @copyright Copyright (C) 2021  Andrew Ritchie, Miklas Riechmann
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

#include <iostream>
#include <string>

#include "../intermediate_structures.h"
#include "../posture_estimator.h"
#include "settingswindow.h"

// #include "opencv/cv.hpp"

GUI::MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  central->setStyleSheet("background-color:#0d1117;");

  // create three buttons
  QPushButton *resetButton = new QPushButton("&Reset Posture");

  resetButton->setStyleSheet(
      "background-color:rgb(10, 187, 228); border: none;");

  QPushButton *modeButton = new QPushButton("&Modes");
  modeButton->setStyleSheet(
      "background-color:rgb(10, 187, 228); border: none;");
  QPushButton *settingsButton = new QPushButton("&Settings");
  settingsButton->setStyleSheet(
      "background-color:rgb(10, 187, 228); border: none;");
  QVBoxLayout *buttonBox = new QVBoxLayout;
  buttonBox->addWidget(resetButton);
  buttonBox->addWidget(modeButton);
  buttonBox->addWidget(settingsButton);
  groupBoxButtons->setLayout(buttonBox);

  // Create a title
  QLabel *title = new QLabel();
  title->setBackgroundRole(QPalette::Dark);
  title->setScaledContents(true);
  QPixmap pix("src/gui/posture-logo.png");
  title->setPixmap(pix);
  title->setMinimumSize(10, 10);
  title->setMaximumSize(250, 125);

  // Run the timer
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(showDateTime()));
  timer->start();

  // Create dummy labels to allow for the cleaning of old widgets
  QLabel *updateLabel = new QLabel();
  QLabel *deleteLabel = new QLabel();

  // Output widgets to the user interface
  mainLayout->addWidget(title, 0, 0);
  mainLayout->addWidget(groupBoxButtons, 1, 1);
  mainLayout->addWidget(deleteLabel, 3, 1);
  mainLayout->addWidget(updateLabel, 3, 0);

  // Display all of the produced widgets on the user's screen
  central->setLayout(mainLayout);
  setCentralWidget(central);
  setWindowTitle(tr("Posture Perfection"));

  // this->generateTable();
  this->initalFrame();

  connect(settingsButton, SIGNAL(clicked()), this,
          SLOT(on_settingsButton_clicked()));
}

void GUI::MainWindow::showDateTime() {
  QGroupBox *groupDateTime = new QGroupBox();
  QVBoxLayout *dtBox = new QVBoxLayout;

  // Get the current date and add it to the Date/Time vertical box
  QLabel *dateLabel = new QLabel;
  QDate date = QDate::currentDate();
  QString OutputDate = date.toString("ddd d MMM yyyy");
  dateLabel->setScaledContents(true);
  dateLabel->setText(OutputDate);
  dateLabel->setAlignment(Qt::AlignRight);
  dateLabel->setStyleSheet("color:rgb(255, 255, 255); font-weight: bold;");
  dtBox->addWidget(dateLabel);

  // Get the current time and add it to the Date/Time vertical box
  QLabel *clock = new QLabel;
  QTime time = QTime::currentTime();
  QString time_text = time.toString("hh : mm : ss");
  clock->setText(time_text);
  clock->setStyleSheet("color:rgb(255, 255, 255); font-weight: bold;");
  clock->setAlignment(Qt::AlignRight);
  dtBox->addWidget(clock);

  // Assign vertical box to a group box
  groupDateTime->setLayout(dtBox);

  // output the current date/time and clear the previous outputted value
  mainLayout->addWidget(groupDateTime, 0, 1);
  mainLayout->itemAt(3)->widget()->deleteLater();
}

GUI::MainWindow::~MainWindow() { delete mainLayout; }

void GUI::MainWindow::initalFrame() {
  QLabel *frame = new QLabel();
  cv::Mat img = cv::imread("src/gui/posture-logo.png");
  QImage imgIn = QImage((uchar *)  // NOLINT [readability/casting]
                        img.data,
                        img.cols, img.rows, img.step, QImage::Format_RGB888);
  QPixmap pixmap = QPixmap::fromImage(imgIn);
  frame->setPixmap(pixmap);
  mainLayout->addWidget(frame, 1, 0);
}

void GUI::MainWindow::updateFrame(cv::Mat currentFrame) {
  QImage imgIn = QImage((uchar *)  // NOLINT [readability/casting]
                        currentFrame.data,
                        currentFrame.cols, currentFrame.rows, currentFrame.step,
                        QImage::Format_RGB888);
  QPixmap pixmap = QPixmap::fromImage(imgIn);
  frame->setPixmap(pixmap);
  mainLayout->addWidget(frame, 1, 0);
}

void GUI::MainWindow::generateTable(void) {
  QTableView *view = new QTableView;
  model = new QStandardItemModel(0, 4);
  view->setModel(model);
  model->setHeaderData(0, Qt::Horizontal, QObject::tr("Position"));
  model->setHeaderData(1, Qt::Horizontal, QObject::tr("X coordinate"));
  model->setHeaderData(2, Qt::Horizontal, QObject::tr("Y coordinate"));
  model->setHeaderData(3, Qt::Horizontal, QObject::tr("Trustworthy"));

  // Populate the table
  for (int i = JointMin; i <= JointMax; i++) {
    QList<QStandardItem *> newRow;
    QStandardItem *joint = new QStandardItem(QString(""));
    QStandardItem *x = new QStandardItem(QString(""));
    QStandardItem *y = new QStandardItem(QString(""));
    QStandardItem *status = new QStandardItem(QString(""));

    joint->setForeground(QColor(Qt::white));
    x->setForeground(QColor(Qt::white));
    y->setForeground(QColor(Qt::white));
    status->setForeground(QColor(Qt::white));

    newRow.append(joint);
    newRow.append(x);
    newRow.append(y);
    newRow.append(status);
    model->insertRow(i, newRow);
  }

  // Add the table to the GUI
  mainLayout->addWidget(view, 1, 0);
}

void GUI::MainWindow::updateTable(PostureEstimating::PoseStatus pose_status) {
  uint8_t i = 0;
  for (auto joint : pose_status.current_pose.joints) {
    model->item(i, 0)->setData(
        PostureEstimating::stringJoint(joint.joint).c_str(), Qt::DisplayRole);
    model->item(i, 1)->setData(QString("%1").arg(joint.coord.x),
                               Qt::DisplayRole);
    model->item(i, 2)->setData(QString("%1").arg(joint.coord.y),
                               Qt::DisplayRole);
    model->item(i, 3)->setData(QString("%1").arg(joint.coord.status),
                               Qt::DisplayRole);
    i++;
  }
}

void GUI::MainWindow::on_settingsButton_clicked() {
  SettingsWindow *full = new SettingsWindow;
  full->showMaximized();
  QCoreApplication::processEvents();
}
