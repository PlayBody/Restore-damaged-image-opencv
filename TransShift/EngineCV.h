#pragma once

#include <string>
#include <vector>

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/calib3d.hpp"

using namespace std;
using namespace cv;

class EngineCV
{
private:
	static string path;
	static vector<string> patchPath;
public:
	static void setImagePath(string path);
	static void setpatchsPath(vector<string> patches);
	static bool run(string& err, Mat& matOut, float ratio_threshold = 0.7, int ransac_iter = 5000, double ransac_thresh = 1.0);
private:
	static void showImage(string windowName, Mat mat);
};

