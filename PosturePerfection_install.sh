# Colour variables
GREEN='\033[0;32m'
B_GREEN='\033[1;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NO_COLOUR='\033[0m'

# Download the model
printf "Downloading the EfficientPoseRT_Lite.tflite model..\n"
mkdir models && cd models
wget https://github.com/ESE-Peasy/PosturePerfection/raw/main/models/EfficientPoseRT_LITE.tflite
cd ..

# Download and install the PosturePerfection build of OpenCV
# https://github.com/ESE-Peasy/opencv/releases/tag/v4.5.1-pp
printf "Downloading and installing PosturePerfection and OpenCV...\n"
OS="$(uname -a)"

case $OS  in
    *"armv7"*)
        wget "https://github.com/ESE-Peasy/opencv/releases/download/v4.5.1-pp/opencv-4.5.1_armhf.deb"
        sudo apt install ./opencv-4.5.1_armhf.deb
        wget "https://github.com/ESE-Peasy/PosturePerfection/releases/download/v0.2.0/PosturePerfection_0.2.0_armv7l"
        chmod u+x PosturePerfection_0.2.0_armv7l 
        ;;
    *"x86"*)
        wget "https://github.com/ESE-Peasy/opencv/releases/download/v4.5.1-pp/opencv-4.5.1_x86.deb"
        sudo apt install ./opencv-4.5.1_x86.deb
        wget "https://github.com/ESE-Peasy/PosturePerfection/releases/download/v0.2.0/PosturePerfection_0.2.0_x86_64"
        chmod u+x PosturePerfection_0.2.0_x86_64
        ;;
    *)
        printf -e "${RED}Unfortunately PosturePerfection is not yet supported on your system.${NO_COLOUR}\n"
        rm -rf models
        exit 1
esac

sudo ldconfig

printf "${B_GREEN}PosturePerfection has been successfully installed!\n\n"

printf "${GREEN}Run ./PosturePerfection_0.2.0_xxx to run our graphical user interface which displays real time posture estimation. You will see the values for the co-ordinate and trustworthiness change as you move around your camera!${NO_COLOUR}\n\n"
