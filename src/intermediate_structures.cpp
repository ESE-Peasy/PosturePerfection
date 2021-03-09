/**
 * @file intermediate_structures.cpp
 * @brief Support functions for the intermediate structures
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


#include "intermediate_structures.h"


std::string PostProcessing::stringTrustworthy(Status s) {
  switch (s) {
    case Trustworthy:
      return "Yes";
    case Untrustworthy:
      return "No";
    default:
      return "unknown";
  }
}
