/*
 * @file datapage.h 
 * @brief Interface for the data page
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
#ifndef SRC_GUI_INTERFACE_HEADER_FILES_DATAPAGE_H_
#define SRC_GUI_INTERFACE_HEADER_FILES_DATAPAGE_H_

#include <QMainWindow>

namespace GUI {

/*
 * @brief This class allows for navigation around the application from the data page.
 *
 * This class lets the user navigate to the mainwindow, settingspage and fullscreen from the data page.
 *
 * This class handles:
 * Navigating to the main window.
 * Navigating to the settings page.
 * Navigating to the full screen.
 *
 */
class DataPage : public QMainWindow{
    Q_OBJECT

 public:
    /*
      * @brief Initialises the data page.
      * 
      * @param *parent Pointer to the parent interface.
      */
    explicit DataPage(QWidget *parent = nullptr);
    ~DataPage();

 private slots:
    /*
      * @brief Closes the data page and opens the main window.
      */
    void on_home_button_clicked();

    /*
      * @brief Triggers functionality letting the user reset their posture.
      */
    void on_reset_button_clicked();

    /*
      * @brief Closes the data page and opens the fullscreen page.
      */
    void on_full_button_clicked();

    /*
      * @brief Closes the data page and opens the settings page.
      */
    void on_settings_button_clicked();



 private:
    /*
      * @brief Closes the data page.
      */
    GUI::DataPage *ui;
};
}  // namespace GUI

#endif  // SRC_GUI_INTERFACE_HEADER_FILES_DATAPAGE_H_
