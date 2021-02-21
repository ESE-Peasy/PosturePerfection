/*
 * @file mainwindow.h
 * @brief Interface for the main page
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
#ifndef SRC_MAINWINDOW_H_
#define SRC_MAINWINDOW_H_

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


/*
 * @brief This class allows for navigation around the application from the main page.
 *
 * This class lets the user navigate to the data page, settings page and fullscreen from the data page.
 *
 * This class handles:
 * Navigating to the data page.
 * Navigating to the settings page.
 * Navigating to the full screen.
 * Triggering posture reseting functionality
 *
 */
class MainWindow : public QMainWindow{
    Q_OBJECT

 public:
    /*
      * @brief Initialises the main page.
      */
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


 private slots:
    /*
      * @brief Closes the main page and opens the settings page.
      */
    void on_settings_button_clicked();

    /*
      * @brief Closes the main page and opens the data page.
      */
    void on_data_button_clicked();

    /*
      * @brief Closes the main page and opens the fullscreen page.
      */
    void on_full_button_clicked();

    /*
      * @brief Outputs the current time.
      */
    void showTime();

    /*
      * @brief Triggers the posture reset functionality.
      */
    void on_reset_button_clicked();


 private:
    /*
      * @brief Closes the main window.
      */
    Ui::MainWindow *ui;
};
#endif  // SRC_MAINWINDOW_H_
