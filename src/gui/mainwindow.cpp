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
#include "QLabel"
#include "QString"

void fetchImage(QString filePath);

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent) {
    auto central = new QWidget;
    QLabel *nameLabel = new QLabel(tr("Name:"));
    nameLine = new QLineEdit;
    //central->setStyleSheet("background-color:#0d1117;");

    //set image
    /*
    QImage("posture-logo.png") myImage;
    myImage.load("posture-logo.png");
    QLabel myLabel;
    myLabel.setPixmap(QPixmap::fromImage(myImage));



    QGroupBox *imageBox = new QGroupBox();
    QVBoxLayout *lay = new QVBoxLayout;

    lay->addWidget(myLabel);
    imageBox->setLayout(lay);
    */
    //QString("posture-logo.png") filename;
    //QImage(filename) myImage;


    /*
    QGraphicsScene *scene;
    QPixmap image;  
    QGroupBox *test = new QGroupBox();
    image.load("posture-logo.png");
    scene = new QGraphicsScene(this);
    scene->addPixmap(image);
    scene->setSceneRect(image.rect());
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(scene);
    test->setLayout(vbox);
    */





    QGroupBox *groupBox = new QGroupBox();
    QRadioButton *radio1 = new QRadioButton(tr("&Radio button 1"));
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(radio1);
    groupBox->setLayout(vbox);


    //create three buttons
    QGroupBox *groupBoxButtons = new QGroupBox();
    QPushButton *resetButton = new QPushButton("&Reset Posture");
    QPushButton *modeButton = new QPushButton("&Modes");
    QPushButton *settingsButton = new QPushButton("&Settings");
    QVBoxLayout *buttonBox = new QVBoxLayout;
    buttonBox->addWidget(resetButton);
    buttonBox->addWidget(modeButton);
    buttonBox->addWidget(settingsButton);
    groupBoxButtons->setLayout(buttonBox);


    /*
    QVBoxLayout *buttonBox = new QVBoxLayout;
    buttonBox->addWidget(resetButton);
    buttonBox->addWidget(modeButton);
    buttonBox->addWidget(settingsButton);
    groupBox->setLayout(buttonBox)
    */

    QWidget *frame = new QWidget(this);
    //frame->setGeometry(x, y, width, height);
    //dataSender *test = new dataSender();
    //dataSender->fetchImage("posture-logo.png");

    frame->setStyleSheet("background-image: url(:/posture-log.png)");
    fetchImage("posture-logo.png");

    

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(frame, 0, 0);
    mainLayout->addWidget(groupBox, 1, 0);
    mainLayout->addWidget(groupBoxButtons, 1, 1);

    

    /*
    mainLayout->addWidget(resetButton, 0, 5, 2, 2);
    mainLayout->addWidget(modeButton, 2, 5);
    mainLayout->addWidget(settingsButton, 10, 5);

    mainLayout->addWidget(nameLabel, 0, 0, Qt::AlignRight);
    mainLayout->addWidget(nameLine, 0, 1);
    mainLayout->addWidget(addressLabel, 1, 0, Qt::AlignTop);
    mainLayout->addWidget(addressText, 2, 4);
    */
    

    //mainLayout->addWidget()

    central->setLayout(mainLayout);

    setCentralWidget(central);
    setWindowTitle(tr("Posture Perfection"));
    
    
}
void fetchImage(QString filePath)
{
const QUrl url =filePath;
if (url.isValid()) {
qDebug() << "Valid URL: " ;
QString test = url.path();
QImage myImage(test);
qDebug() << "Image Size " << myImage.size();
}
else{
qDebug() << "the url is invalid";
}
}


/*
QGroupBox *MainWindow::createFirstExclusiveGroup()
{
    QGroupBox *groupBox = new QGroupBox(tr("Exclusive Radio Buttons"));

    QRadioButton *radio1 = new QRadioButton(tr("&Radio button 1"));
    QRadioButton *radio2 = new QRadioButton(tr("R&adio button 2"));
    QRadioButton *radio3 = new QRadioButton(tr("Ra&dio button 3"));

    radio1->setChecked(true);
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(radio1);
    vbox->addWidget(radio2);
    vbox->addWidget(radio3);
    vbox->addStretch(1);
    groupBox->setLayout(vbox);

    return groupBox;
}
*/
