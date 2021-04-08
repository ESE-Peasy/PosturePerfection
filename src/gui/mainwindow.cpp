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

#include <QFile>
#include <iostream>
#include <string>

#include "../intermediate_structures.h"
#include "../posture_estimator.h"

#define LOGO_HEIGHT_MAX 100

GUI::MainWindow::MainWindow(Pipeline::Pipeline *pipeline, QWidget *parent)
    : QMainWindow(parent) {
  pipelinePtr = pipeline;

  // Load style sheet
  QFile stylesheet("src/gui/stylesheet.qss");
  stylesheet.open(QFile::ReadOnly);
  QString setSheet = QLatin1String(stylesheet.readAll());
  QWidget::setStyleSheet(setSheet);

  // Create the different GUI pages
  createMainPage();
  createSettingsPage();

  // Stack the pages within the mainwindow
  stackedWidget->addWidget(firstPageWidget);
  stackedWidget->addWidget(secondPageWidget);

  // Create a title
  QLabel *title = new QLabel();
  title->setBackgroundRole(QPalette::Dark);
  QPixmap pix("images/logo.png");
  {
    auto width = pix.width();
    auto height = pix.height();

    if (height > LOGO_HEIGHT_MAX) {
      height = LOGO_HEIGHT_MAX;
      width = (LOGO_HEIGHT_MAX * width) / height;
    }

    title->setPixmap(pix.scaled(width, height, Qt::KeepAspectRatio));
  }

  // Run the timer
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(showDateTime()));
  timer->start();

  qRegisterMetaType<PostureEstimating::PostureState>(
      "PostureEstimating::PostureState");
  connect(this, SIGNAL(currentGoodBadPosture(PostureEstimating::PostureState)),
          this,
          SLOT(updatePostureNotification(PostureEstimating::PostureState)));

  // Output widgets to the user interface
  mainLayout->addWidget(title, 0, 0);
  mainLayout->addWidget(stackedWidget, 1, 0, 1, 2);

  // Display all of the produced widgets on the user's screen
  central->setLayout(mainLayout);
  setCentralWidget(central);
  setWindowTitle(tr("Posture Perfection"));

  qRegisterMetaType<cv::Mat>("cv::Mat");
  connect(this, SIGNAL(currentFrameSignal(cv::Mat)), this,
          SLOT(updateVideoFrame(cv::Mat)));
}

void GUI::MainWindow::openMainPage(void) { stackedWidget->setCurrentIndex(0); }

void GUI::MainWindow::openSettingsPage(void) {
  stackedWidget->setCurrentIndex(1);
}

void GUI::MainWindow::updatePostureNotification(
    PostureEstimating::PostureState postureState) {
  // Check if the ideal pose has been set and if so, display notification
  // according to the posture state
  switch (postureState) {
    case PostureEstimating::PostureState::UndefinedAndUnset: {
      postureNotificationBox->setStyleSheet("background-color: orange");
      postureNotification->setText("Undefined");
      break;
    }
    case PostureEstimating::PostureState::Unset: {
      postureNotification->setStyleSheet("background-color: #eda333");
      postureNotification->setText("No Set Posture",
                                   "Use the Set Ideal Posture\nbutton below");
      break;
    }
    case PostureEstimating::PostureState::Good: {
      postureNotification->setStyleSheet("background-color: #96cb2f");
      postureNotification->setText("PosturePerfection!");
      break;
    }
    case PostureEstimating::PostureState::Undefined: {
      postureNotification->setStyleSheet("background-color: #909090");
      postureNotification->setText("Unknown Posture",
                                   "Hmm... I can't find your posture");
      break;
    }
    case PostureEstimating::PostureState::Bad: {
      postureNotification->setStyleSheet("background-color: #ed3833");
      postureNotification->setText(
          "Imperfect Posture",
          "Follow the guides to\nachieve PosturePerfection!");
      break;
    }
  }
}

void GUI::MainWindow::updatePose(PostureEstimating::PoseStatus poseStatus) {
  currentPoseStatus = poseStatus;
  emit currentGoodBadPosture(poseStatus.posture_state);
}

void GUI::MainWindow::createMainPage() {
  initalFrame();
  firstPageWidget->setLayout(mainPageLayout);
  connect(idealPostureButton, SIGNAL(clicked()), this, SLOT(setIdealPosture()));
  auto *settingsButton = new Button("Settings");
  connect(settingsButton, SIGNAL(clicked()), this, SLOT(openSettingsPage()));

  QGroupBox *mainPageButtons = new QGroupBox();
  QVBoxLayout *mainPageButtonsBox = new QVBoxLayout;
  mainPageButtonsBox->addWidget(postureNotification);
  mainPageButtonsBox->addWidget(settingsButton);
  mainPageButtonsBox->addWidget(idealPostureButton);
  mainPageButtons->setLayout(mainPageButtonsBox);
  mainPageLayout->addWidget(mainPageButtons, 1, 1);
}

void GUI::MainWindow::createSettingsPage() {
  QGroupBox *groupSettings = new QGroupBox();
  QGridLayout *settings = new QGridLayout;

  // Create Setting's page title
  auto *settingsTitle = new Label("Settings Page");
  QFont font = settingsTitle->font();
  font.setPointSize(37);
  font.setBold(true);
  settingsTitle->setFont(font);

  // Set layout
  settingsPageLayout->addWidget(settingsTitle, 0, 0, 1, 5, Qt::AlignLeft);
  settingsPageLayout->addWidget(groupSettings, 1, 0, 6, 4);
  settings->setSpacing(0);
  settings->setMargin(0);

  // Allow user to select the confidence threshold
  auto *confidenceLabel = new Label("Confidence Threshold");
  QSlider *confidenceSlider = new QSlider(Qt::Horizontal, this);
  confidenceSlider->setMinimum(0);
  confidenceSlider->setMaximum(100);
  confidenceSlider->setTickInterval(1);
  int initalThreshold =
      static_cast<int>(pipelinePtr->get_confidence_threshold() * 100);
  confidenceSlider->setValue(initalThreshold);
  settings->addWidget(confidenceLabel, 0, 0, 1, 3, Qt::AlignBottom);
  settings->addWidget(confidenceSlider, 1, 0, 1, 3, Qt::AlignTop);
  connect(confidenceSlider, SIGNAL(valueChanged(int)), this,
          SLOT(setThresholdValue(int)));

  // Allow user to select the pose change threshold
  auto *poseChangeThresholdLabel = new Label("Slouch Sensitivity");
  QSlider *poseChangeThresholdSlider = new QSlider(Qt::Horizontal, this);
  poseChangeThresholdSlider->setMinimum(0);
  poseChangeThresholdSlider->setMaximum(5);
  poseChangeThresholdSlider->setTickInterval(1);
  int initalPoseChangeThreshold =
      static_cast<int>(pipelinePtr->get_pose_change_threshold() * 10);
  poseChangeThresholdSlider->setValue(initalPoseChangeThreshold);
  settings->addWidget(poseChangeThresholdLabel, 2, 0, 1, 3, Qt::AlignBottom);
  settings->addWidget(poseChangeThresholdSlider, 3, 0, 1, 3, Qt::AlignTop);
  connect(poseChangeThresholdSlider, SIGNAL(valueChanged(int)), this,
          SLOT(setPoseChangeThresholdValue(int)));

  // Let user adjust the video framerate
  auto *upFramerate = new Button("Up");
  auto *downFramerate = new Button("Down");
  settings->addWidget(upFramerate, 4, 0, Qt::AlignLeft);
  settings->addWidget(downFramerate, 4, 2, Qt::AlignRight);
  settings->addWidget(currentFrameRate, 4, 1, Qt::AlignHCenter);
  connect(upFramerate, SIGNAL(clicked()), this, SLOT(increaseVideoFramerate()));
  connect(downFramerate, SIGNAL(clicked()), this,
          SLOT(decreaseVideoFramerate()));
  setOutputFramerate();

  groupSettings->setLayout(settings);
  secondPageWidget->setLayout(settingsPageLayout);

  auto *homeButton = new Button("Back to video");
  connect(homeButton, SIGNAL(clicked()), this, SLOT(openMainPage()));
  QGroupBox *settingsPageButtons = new QGroupBox();
  QVBoxLayout *settingsPageButtonsBox = new QVBoxLayout;
  settingsPageButtonsBox->addWidget(homeButton);
  settingsPageButtons->setLayout(settingsPageButtonsBox);
  settingsPageLayout->addWidget(settingsPageButtons, 1, 4, 6, 1);
}

void GUI::MainWindow::setIdealPosture() {
  pipelinePtr->set_ideal_posture(currentPoseStatus.current_pose);
  idealPostureButton->setText("Reset Ideal Posture",
                              "Set current\nposture as my target");
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
  currentFrameRate->setText(output);
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
  mainLayout->itemAt(1)->widget()->deleteLater();
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
