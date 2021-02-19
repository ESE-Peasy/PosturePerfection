/*
 * @file settingspage.h
 * @brief Interface for the settings page
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
#ifndef SRC_SETTINGSPAGE_H_
#define SRC_SETTINGSPAGE_H_

#include <QMainWindow>

/*
 * @brief This class allows for navigation around the application from the settings page.
 *
 * This class lets the user navigate to the mainwindow, datapage and fullscreen from the settings page.
 *
 * This class handles:
 * Navigating to the main window.
 * Navigating to the data page.
 * Navigating to the full screen.
 *
 */
namespace Ui {
class settingsPage;
}

class settingsPage : public QMainWindow{
    Q_OBJECT

 public:
    /*
      * @brief Initialises the settings page.
      */
    explicit settingsPage(QWidget *parent = nullptr);
    ~settingsPage();

 private slots:
    /*
      * @brief Closes the settings page and opens the main window.
      */
    void on_home_button_clicked();

    /*
      * @brief Closes the settings page and opens the data page.
      */
    void on_data_button_clicked();

    /*
      * @brief Closes the settings page and opens the fullscreen page.
      */
    void on_full_button_clicked();

 private:
    /*
      * @brief Closes the settings page.
      */
    Ui::settingsPage *ui;
};

#endif  // SRC_SETTINGSPAGE_H_
