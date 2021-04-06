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

GUI::MainWindow::MainWindow(Pipeline::Pipeline *pipeline, QWidget *parent)
    : QMainWindow(parent) {
  // Create the different GUI pages
  pipelinePtr = pipeline;
  createMainPage();
  createSettingsPage();

  // Stack the pages within the mainwindow
  QStackedWidget *stackedWidget = new QStackedWidget;
  stackedWidget->addWidget(firstPageWidget);
  stackedWidget->addWidget(secondPageWidget);
  stackedWidget->addWidget(thirdPageWidget);

  pageComboBox->addItem(tr("Video Feed"));
  pageComboBox->addItem(tr("Settings Page"));
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
  QPushButton *idealPosture = new QPushButton("&Ideal Posture");
  framerate->addWidget(idealPosture, 1, 3, 1, 1, Qt::AlignCenter);
  connect(idealPosture, SIGNAL(clicked()), this, SLOT(setIdealPosture()));

  idealPosture->setStyleSheet(
      "background-color:rgb(10, 187, 228); border: none;");

  QVBoxLayout *buttonBox = new QVBoxLayout;
  buttonBox->addWidget(pageComboBox);
  buttonBox->addWidget(idealPosture);
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

  qRegisterMetaType<PostureEstimating::PostureState>(
      "PostureEstimating::PostureState");
  connect(this, SIGNAL(currentGoodBadPosture(PostureEstimating::PostureState)),
          this, SLOT(updatePostureNotification(PostureEstimating::PostureState)));

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

void GUI::MainWindow::updatePostureNotification(
    PostureEstimating::PostureState postureState) {
  // Create notification widgets and set the minimum size shown 
  QWidget *postureNotificationBox = new QWidget;
  postureNotificationBox->setMinimumSize(200, 40);
  QGridLayout *postureNotificationLayout = new QGridLayout;
  QLabel *postureNotification = new QLabel();

  // Check if the ideal pose has been set and if so, display notification
  // according to the posture state
  if (postureState != 2) {
    if (postureState == 0) {
      postureNotificationBox->setStyleSheet("background-color: green");
      postureNotification->setText("Good Posture");
    } else {
      postureNotificationBox->setStyleSheet("background-color: red");
      postureNotification->setText("Bad Posture");
    }
  } else {
    postureNotificationBox->setStyleSheet("background-color: orange");
    postureNotification->setText("Undefined");
  }
  postureNotification->setStyleSheet("QLabel {color : white; }");
  postureNotificationLayout->addWidget(postureNotification, 0, 0,
                                       Qt::AlignCenter);
  postureNotificationBox->setLayout(postureNotificationLayout);
  mainPageLayout->addWidget(postureNotificationBox, 2, 0, Qt::AlignCenter);
}

void GUI::MainWindow::updatePose(PostureEstimating::PoseStatus poseStatus) {
  currentPoseStatus = poseStatus;
  emit currentGoodBadPosture(poseStatus.posture_state);
}

void GUI::MainWindow::createMainPage() {
  initalFrame();
  firstPageWidget->setLayout(mainPageLayout);
}

void GUI::MainWindow::createSettingsPage() {
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
  QSlider *confidenceSlider = new QSlider(Qt::Horizontal, this);
  confidenceSlider->setMinimum(0);
  confidenceSlider->setMaximum(100);
  confidenceSlider->setTickInterval(1);
  int initalThreshold =
      static_cast<int>(pipelinePtr->get_confidence_threshold() * 100);
  confidenceSlider->setValue(initalThreshold);
  vertThreshold->setSpacing(0);
  vertThreshold->setMargin(0);
  vertThreshold->addWidget(confidenceLabel, 0, Qt::AlignBottom);
  vertThreshold->addWidget(confidenceSlider, 0, Qt::AlignTop);

  settingsPageLayout->addWidget(groupThreshold, 1, 0);

  connect(confidenceSlider, SIGNAL(valueChanged(int)), this,
          SLOT(setThresholdValue(int)));

  // Allow user to select the pose change threshold
  QLabel *poseChangeThresholdLabel = new QLabel();
  poseChangeThresholdLabel->setText("Pose Change Threshold");
  poseChangeThresholdLabel->setStyleSheet("QLabel {color : white; }");

  QSlider *poseChangeThresholdSlider = new QSlider(Qt::Horizontal, this);
  poseChangeThresholdSlider->setMinimum(0);
  poseChangeThresholdSlider->setMaximum(5);
  poseChangeThresholdSlider->setTickInterval(1);
  int initalPoseChangeThreshold =
      static_cast<int>(pipelinePtr->get_pose_change_threshold() * 10);
  poseChangeThresholdSlider->setValue(initalPoseChangeThreshold);
  vertThreshold->addWidget(poseChangeThresholdLabel, 0, Qt::AlignBottom);
  vertThreshold->addWidget(poseChangeThresholdSlider, 0, Qt::AlignTop);
  groupThreshold->setLayout(vertThreshold);

  settingsPageLayout->addWidget(groupThreshold, 1, 0);

  connect(poseChangeThresholdSlider, SIGNAL(valueChanged(int)), this,
          SLOT(setPoseChangeThresholdValue(int)));

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

  setOutputFramerate();

  currentFrame->setStyleSheet("QLabel {color : white; }");
  framerate->addWidget(currentFrame, 1, 1, 1, 1, Qt::AlignLeft);

  QGroupBox *groupFramerate = new QGroupBox();
  groupFramerate->setLayout(framerate);
  settingsPageLayout->addWidget(groupFramerate, 2, 0);

  secondPageWidget->setLayout(settingsPageLayout);
}

void GUI::MainWindow::setIdealPosture() {
  pipelinePtr->set_ideal_posture(currentPoseStatus.current_pose);
}

void GUI::MainWindow::setThresholdValue(int scaledValue) {
  float value = static_cast<float>(scaledValue) / 100.0;
  pipelinePtr->set_confidence_threshold(value);
}

void GUI::MainWindow::increaseVideoFramerate() {
  pipelinePtr->increase_framerate();
  setOutputFramerate();
}

void GUI::MainWindow::setOutputFramerate() {
  float newFramerate = pipelinePtr->get_framerate();
  QString output =
      "Frame Rate: " + QString::number(newFramerate, 'f', 1) + " fps";
  currentFrame->setText(output);
}

void GUI::MainWindow::decreaseVideoFramerate() {
  pipelinePtr->decrease_framerate();
  setOutputFramerate();
}

void GUI::MainWindow::setPoseChangeThresholdValue(int scaledValue) {
  float value = static_cast<float>(scaledValue) / 10.0;
  pipelinePtr->set_confidence_threshold(value);
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
