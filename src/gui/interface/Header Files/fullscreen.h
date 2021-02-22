/*
 * @file fullscreen.h
 * @brief Interface for the full screen live feed user interface
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

#ifndef SRC_FULLSCREEN_H_
#define SRC_FULLSCREEN_H_

#include <QMainWindow>


namespace Ui {
class fullscreen;
}



/*
 * @brief This class allows for navigation around the application from the fullscreen page.
 *
 * This class lets the user navigate to the main window, settings page and data page from the fullscreen page.
 *
 * This class handles:
 * Navigating to the mainwindow.
 * Navigating to the settingspage.
 * Navigating to the datapage.
 *
 *
 *
 */
class FullScreen : public QMainWindow{
    Q_OBJECT

 public:
    /*
      * @brief Initialises the full screen.
      */
    explicit FullScreen(QWidget *parent = nullptr);
    ~FullScreen();

 private slots:
    /*
      * @brief Closes the full screen and opens the main window.
      */
    void on_home_button_clicked();

    /*
      * @brief Closes the full screen and opens the data page.
      */
    void on_data_button_clicked();

    /*
      * @brief Closes the full screen and opens the settings page.
      */
    void on_settings_button_clicked();




 private:
    /*
      * @brief Closes the full screen page.
      */
    Ui::FullScreen *ui;
};

#endif  // SRC_FULLSCREEN_H_
