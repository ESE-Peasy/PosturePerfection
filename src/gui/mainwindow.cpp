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

GUI::MainWindow::MainWindow(Pipeline::Pipeline *pipeline, QWidget *parent)
    : QMainWindow(parent) {
  // Create the different GUI pages
  createMainPage();
  createSettingsPage(pipeline);
  pipelinePtr = pipeline;

  // Stack the pages within the mainwindow
  QStackedWidget *stackedWidget = new QStackedWidget;
  stackedWidget->addWidget(firstPageWidget);
  stackedWidget->addWidget(secondPageWidget);
  stackedWidget->addWidget(thirdPageWidget);

  pageComboBox->addItem(tr("Video Feed"));
  pageComboBox->addItem(tr("Settings Page"));
  pageComboBox->addItem(tr("Data Page"));
  pageComboBox->setStyleSheet(
      "QListView{color:white; background-color:#0d1117;}");
  pageComboBox->setStyleSheet(
      "QComboBox {color:white; background-color: #0d1117; border-color: "
      "rgba(255,255,255,200); border-width: 1px; border-style: solid; padding: "
      "1px 0px 1px 3px;}");

  connect(pageComboBox, QOverload<int>::of(&QComboBox::activated),
          stackedWidget, &QStackedWidget::setCurrentIndex);

  central->setStyleSheet("background-color:#0d1117;");

  // create three buttons
  QPushButton *resetButton = new QPushButton("&Reset Posture");

  resetButton->setStyleSheet(
      "background-color:rgb(10, 187, 228); border: none;");

  QVBoxLayout *buttonBox = new QVBoxLayout;
  buttonBox->addWidget(pageComboBox);
  buttonBox->addWidget(resetButton);
  groupBoxButtons->setLayout(buttonBox);

  // Create a title
  QLabel *title = new QLabel();
  title->setBackgroundRole(QPalette::Dark);
  title->setScaledContents(true);
  QPixmap pix("images/logo.png");
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

  connect(pageComboBox, QOverload<int>::of(&QComboBox::activated),
          stackedWidget, &QStackedWidget::setCurrentIndex);

  // Output widgets to the user interface
  mainLayout->addWidget(title, 0, 0);
  mainLayout->addWidget(groupBoxButtons, 1, 1);
  mainLayout->addWidget(deleteLabel, 3, 1);
  mainLayout->addWidget(updateLabel, 3, 0);
  mainLayout->addWidget(stackedWidget, 1, 0);

  // Display all of the produced widgets on the user's screen
  central->setLayout(mainLayout);
  setCentralWidget(central);
  setWindowTitle(tr("Posture Perfection"));

  qRegisterMetaType<cv::Mat>("cv::Mat");
  connect(this, SIGNAL(currentFrameSignal(cv::Mat)), this,
          SLOT(updateVideoFrame(cv::Mat)));
}

void GUI::MainWindow::updatePose(PostureEstimating::PoseStatus poseStatus) {
  currentPoseStatus = poseStatus;
}

void GUI::MainWindow::createMainPage() {
  initalFrame();
  firstPageWidget->setLayout(mainPageLayout);
}

void GUI::MainWindow::createSettingsPage(Pipeline::Pipeline *pipeline) {
  QGroupBox *groupThreshold = new QGroupBox();
  QVBoxLayout *vertThreshold = new QVBoxLayout;

  // Create Setting's page title
  QLabel *settingsTitle = new QLabel();
  settingsTitle->setText("Settings Page");
  settingsTitle->setStyleSheet("QLabel {color : white; }");
  QFont font = settingsTitle->font();
  font.setPointSize(37);
  font.setBold(true);
  settingsTitle->setFont(font);
  settingsPageLayout->addWidget(settingsTitle, 0, 0, Qt::AlignCenter);

  // Allow user to select the confidence threshold
  QLabel *confidenceLabel = new QLabel();
  confidenceLabel->setText("Posture Estimating Sensitivity");
  confidenceLabel->setStyleSheet("QLabel {color : white; }");
  QSlider *slider = new QSlider(Qt::Horizontal, this);
  slider->setMinimum(0);
  slider->setMaximum(10);
  slider->setTickInterval(1);
  vertThreshold->setSpacing(0);
  vertThreshold->setMargin(0);
  vertThreshold->addWidget(confidenceLabel, 0, Qt::AlignBottom);
  vertThreshold->addWidget(slider, 0, Qt::AlignTop);
  groupThreshold->setLayout(vertThreshold);

  settingsPageLayout->addWidget(groupThreshold, 1, 0);

  connect(slider, SIGNAL(valueChanged(int)), this,
          SLOT(setThresholdValue(int)));

  // Let user adjust the video framerate
  framerate->setSpacing(0);
  framerate->setMargin(0);
  QPushButton *upFramerate = new QPushButton("&Up");
  QPushButton *downFramerate = new QPushButton("&Down");
  framerate->addWidget(upFramerate, 0, 1, 1, 1, Qt::AlignLeft);
  framerate->addWidget(downFramerate, 2, 1, 1, 1, Qt::AlignLeft);

  connect(upFramerate, SIGNAL(clicked()), this, SLOT(increaseVideoFramerate()));
  connect(downFramerate, SIGNAL(clicked()), this,
          SLOT(decreaseVideoFramerate()));

  currentFrame->setText("Frame Rate: 1 fps");
  currentFrame->setStyleSheet("QLabel {color : white; }");
  framerate->addWidget(currentFrame, 1, 1, 1, 1, Qt::AlignLeft);

  // Let the user take their current posture as the ideal posture
  QLabel *idealLabel = new QLabel();
  idealLabel->setText("Set Ideal Posture ->");
  idealLabel->setStyleSheet("QLabel {color : white; }");
  framerate->addWidget(idealLabel, 1, 2, 1, 1, Qt::AlignRight);

  QPushButton *idealPosture = new QPushButton("&Ideal Posture");
  framerate->addWidget(idealPosture, 1, 3, 1, 1, Qt::AlignCenter);
  connect(idealPosture, SIGNAL(clicked()), this, SLOT(setIdealPosture()));

  QGroupBox *groupFramerate = new QGroupBox();
  groupFramerate->setLayout(framerate);
  settingsPageLayout->addWidget(groupFramerate, 2, 0);

  secondPageWidget->setLayout(settingsPageLayout);
}

void GUI::MainWindow::setIdealPosture() {
  pipelinePtr->set_ideal_posture(currentPoseStatus.current_pose);
}

void GUI::MainWindow::setThresholdValue(int scaledValue) {
  float value = static_cast<float>(scaledValue) / 10.0;
  pipelinePtr->set_confidence_threshold(value);
}

void GUI::MainWindow::increaseVideoFramerate() {
  pipelinePtr->increase_framerate();
  setOutputFramerate();
}

void GUI::MainWindow::setOutputFramerate() {
  float newFramerate = pipelinePtr->get_framerate();
  QString output = "Frame Rate: " + QString::number(newFramerate) + " fps";
  currentFrame->setText(output);
}

void GUI::MainWindow::decreaseVideoFramerate() {
  pipelinePtr->decrease_framerate();
  setOutputFramerate();
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
  cv::Mat img = cv::imread("images/logo.png");
  // Switch from BGR to RGB
  cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
  QImage imgIn = QImage((uchar *)  // NOLINT [readability/casting]
                        img.data,
                        img.cols, img.rows, img.step, QImage::Format_RGB888);
  QPixmap pixmap = QPixmap::fromImage(imgIn);
  frame->setPixmap(pixmap);
  mainPageLayout->addWidget(frame, 1, 0);
}

void GUI::MainWindow::emitNewFrame(cv::Mat currentFrame) {
  emit currentFrameSignal(currentFrame);
}

void GUI::MainWindow::updateVideoFrame(cv::Mat currentFrame) {
  QImage imgIn = QImage((uchar *)  // NOLINT [readability/casting]
                        currentFrame.data,
                        currentFrame.cols, currentFrame.rows, currentFrame.step,
                        QImage::Format_RGB888);
  QPixmap pixmap = QPixmap::fromImage(imgIn);
  frame->setPixmap(pixmap);
  mainPageLayout->addWidget(frame, 1, 0);
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
