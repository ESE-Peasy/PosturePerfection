/**
 * @file main.cpp
 * @brief Main file for the user interface
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

#include <QApplication>

#include "mainwindow.h"

/**
 * @brief Initialise and run the user interface
 */

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  GUI::MainWindow w;
  w.showMaximized();

  return a.exec();
}
