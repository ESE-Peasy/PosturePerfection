#include<posture_estimator.h> 

#include <stdio.h>
#include <cmath> 

using namespace std;
  

class PostureEstimating::PostureEstimator {
    void PostureEstimating(){}

    float getLineAngle(PostProcessing::Coordinate coord1,
                     PostProcessing::Coordinate coord2){
                         float x_dif = coord2.x - coord1.x;
                         float y_dif = coord2.y - coord1.y;
                         float slope = y_dif/x_dif;
                         return atan(slope);
                     }
    
}