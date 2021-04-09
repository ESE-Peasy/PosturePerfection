# Colour variables
GREEN='\033[0;32m'
B_GREEN='\033[1;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NO_COLOUR='\033[0m'

# Download assets
printf "Downloading additional assets...\n"
mkdir -p assets && cd assets
wget https://github.com/ESE-Peasy/PosturePerfection/raw/main/assets/EfficientPoseRT_LITE.tflite
wget https://github.com/ESE-Peasy/PosturePerfection/raw/main/assets/logo.png  
# TODO: Update for QSS Style Sheet
cd ..

# Download and install the PosturePerfection build of OpenCV
# https://github.com/ESE-Peasy/opencv/releases/tag/v4.5.1-pp
printf "Downloading and installing PosturePerfection and OpenCV...\n"
OS="$(uname -a)"

case $OS  in
    *"armv7"*)
        wget "https://github.com/ESE-Peasy/opencv/releases/download/v4.5.1-pp/opencv-4.5.1_armhf.deb"
        sudo apt install ./opencv-4.5.1_armhf.deb
        wget "https://github.com/ESE-Peasy/PosturePerfection/releases/download/v0.4.0/PosturePerfection_0.4.0_armv7l"
        chmod u+x PosturePerfection_0.4.0_armv7l 
        ;;
    *"x86"*)
        wget "https://github.com/ESE-Peasy/opencv/releases/download/v4.5.1-pp/opencv-4.5.1_x86.deb"
        sudo apt install ./opencv-4.5.1_x86.deb
        wget "https://github.com/ESE-Peasy/PosturePerfection/releases/download/v0.4.0/PosturePerfection_0.4.0_x86_64"
        chmod u+x PosturePerfection_0.4.0_x86_64
        ;;
    *)
        printf -e "${RED}Unfortunately PosturePerfection is not yet supported on your system.${NO_COLOUR}\n"
        rm -rf models
        exit 1
esac

sudo ldconfig

printf "${B_GREEN}PosturePerfection has been successfully installed!\n\n"

printf "${GREEN}Run ./PosturePerfection_0.4.0_xxx to run our graphical user interface which displays real time posture estimation. Lines are drawn between
your Head, Neck, Shoulder and Hip to demonstrate your detected posture! See the `About \& Help` page for more information about usage.${NO_COLOUR}\n\n"
