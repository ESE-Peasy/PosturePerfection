/*
 * @file settingswindow.cpp
 * @brief Settings page
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

#include "settingswindow.h"

#include "mainwindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) : QMainWindow(parent) {
  central->setStyleSheet("background-color:#0d1117;");
  // create three buttons
  QPushButton *mainButton = new QPushButton("&Main Window");

  mainButton->setStyleSheet(
      "background-color:rgb(10, 187, 228); border: none;");

  QPushButton *modeButton = new QPushButton("&Modes");
  modeButton->setStyleSheet(
      "background-color:rgb(10, 187, 228); border: none;");
  QPushButton *settingsButton = new QPushButton("&Settings");
  settingsButton->setStyleSheet(
      "background-color:rgb(10, 187, 228); border: none;");
  QVBoxLayout *buttonBox = new QVBoxLayout;
  buttonBox->addWidget(mainButton);
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

  connect(mainButton, SIGNAL(clicked()), this, SLOT(on_mainButton_clicked()));
}

void SettingsWindow::showDateTime() {
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

SettingsWindow::~SettingsWindow() { delete mainLayout; }

void SettingsWindow::on_mainButton_clicked() {
  GUI::MainWindow *full = new GUI::MainWindow;
  full->showMaximized();
  QCoreApplication::processEvents();
  hide();
}
