/**
 * @file mainwindow.cpp
 * @brief Main user interface file
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
#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent) {
  //centralWidget = new QWidget();


  // Create the button, make "this" the parent
  m_button = new QPushButton("My Button", this);
  // set size and location of the button
  m_button->setGeometry(QRect(QPoint(100, 100), QSize(200, 50)));
 
  // Connect button signal to appropriate slot
  connect(m_button, &QPushButton::released, this, &MainWindow::handleButton);




  /*
	frame = new QFrame(centralWidget);
	frame->setFrameShape(QFrame::StyledPanel);
	frame->setFrameShadow(QFrame::Raised);

	verticalLayout_2 = new QVBoxLayout(frame);
	verticalLayout_2->setSpacing(6);
	verticalLayout_2->setContentsMargins(11, 11, 11, 11);

	label = new QLabel(frame);
	verticalLayout_2->addWidget(label);
	verticalLayout->addWidget(frame);
	setCentralWidget(centralWidget);
	statusBar = new QStatusBar();
	setStatusBar(statusBar);
  */
    
}

void MainWindow::handleButton()
{
  // change the text
  m_button->setText("Example");
  // resize button
  m_button->resize(100,100);
}
