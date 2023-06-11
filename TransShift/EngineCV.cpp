#include "EngineCV.h"

string EngineCV::path = "";
vector<string> EngineCV::patchPath = vector<string>();

void EngineCV::setImagePath(string path)
{
    EngineCV::path = path;
}

void EngineCV::setpatchsPath(vector<string> patches)
{
    EngineCV::patchPath = patches;
}

bool EngineCV::run(string& err, Mat& matOut, float ratio_threshold, int ransac_iter, double ransac_thresh)
{
    if (path.length() == 0) {
        err = "path undefined";
        return false;
    }

    if (patchPath.size() == 0) {
        err = "patches undefined";
        return false;
    }


    // Step 1: Load the corrupted image and patches
    Mat img = imread(path); // Replace with path to your image file
    // int calcSize = 480000;
    // int w = sqrt(calcSize * 1.0 / (img.cols * img.rows)) * img.cols;
    // int h = sqrt(calcSize * 1.0 / (img.cols * img.rows)) * img.rows;
    // resize(img, img, Size(w, h));

    showImage("input", img);
    vector<Mat> patches;
    int i;
    int n_patches = patchPath.size();
    for (i = 0; i < n_patches; i++) {
        Mat patch = imread(patchPath[i]); // Replace with the naming convention of your patch images
        patches.push_back(patch);
        // showImage(patchPath[i], patch);
    }

    // Step 2: Extract SIFT features from the image and patch images
    Ptr<SIFT> sift = SIFT::create();
    vector<KeyPoint> keypoints_img;
    Mat descriptors_img;
    BFMatcher matcher(NORM_L2);

    sift->detectAndCompute(img, noArray(), keypoints_img, descriptors_img);
    // showImage("descriptor_img", descriptors_img);

    Mat out = img.clone();

    for (i = 0; i < n_patches; i++) {
        vector<KeyPoint> keypoints_patch;
        Mat descriptors_patch;
        vector<DMatch> good_matches;
        sift->detectAndCompute(patches[i], noArray(), keypoints_patch, descriptors_patch);

        // Step 3: Match features between the image and each patch image
        vector<vector<DMatch>> matches;
        matcher.knnMatch(descriptors_patch, descriptors_img, matches, 2);
        for (size_t j = 0; j < matches.size(); j++) {
            if (matches[j][0].distance < ratio_threshold * matches[j][1].distance) {
                good_matches.push_back(matches[j][0]);
            }
        }
        if (good_matches.size() < 2) {
            good_matches.clear();
            flip(patches[i], patches[i], 1);
            sift->detectAndCompute(patches[i], noArray(), keypoints_patch, descriptors_patch);
            matcher.knnMatch(descriptors_patch, descriptors_img, matches, 2);
            for (size_t j = 0; j < matches.size(); j++) {
                if (matches[j][0].distance < ratio_threshold * matches[j][1].distance) {
                    good_matches.push_back(matches[j][0]);
                }
            }
        }
        if (good_matches.size() < 2) {
            continue;
        }

        // Step 4: Find affine transformation between the image and each patch image using RANSAC algorithm

        vector<Point2f> img_pts, patch_pts;
        vector<int> inliers;
        for (size_t j = 0; j < good_matches.size(); j++) {
            int idx_img = good_matches[j].trainIdx;
            int idx_patch = good_matches[j].queryIdx;
            img_pts.push_back(keypoints_img[idx_img].pt);
            patch_pts.push_back(keypoints_patch[idx_patch].pt);
        }
        Mat H = findHomography(patch_pts, img_pts, RANSAC, ransac_thresh, inliers);

        // Step 5: Overlay the patches over the image to fix the corrupted regions
        // Use the warpPerspective function in OpenCV to apply the homography matrix and overlay the patches onto the corresponding regions of the image.

        Mat warped_patch;
        warpPerspective(patches[i], warped_patch, H, img.size());
        // showImage(patchPath[i], warped_patch);
        int w, h;
        for (h = 0; h < warped_patch.rows; h++) {
            for (w = 0; w < warped_patch.cols; w++) {
                Vec3b pixel = warped_patch.at<Vec3b>(Point(w, h));
                if (pixel[0] < 20 && pixel[1] < 20 && pixel[2] < 20) {
                    continue;
                }
                Vec3b pixelO = out.at<Vec3b>(Point(w, h));
                if (pixelO[0] < 250 && pixelO[1] < 250 && pixelO[2] < 250) {
                    continue;
                }
                out.at<Vec3b>(Point(w, h)) = pixel;
            }
        }
    }
    err = "Success";
    showImage("out", out);
    return true;
}

void EngineCV::showImage(string windowName, Mat mat)
{
    Mat m = mat.clone();
    namedWindow(windowName, WINDOW_FREERATIO);
    imshow(windowName, m);
}
