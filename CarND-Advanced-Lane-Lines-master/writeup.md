## Writeup for Project 2

### advanced lane finding.

---

**Advanced Lane Finding Project**

The goals / steps of this project are the following:

* Compute the camera calibration matrix and distortion coefficients given a set of chessboard images.
* Apply a distortion correction to raw images.
* Use color transforms, gradients, etc., to create a thresholded binary image.
* Apply a perspective transform to rectify binary image ("birds-eye view").
* Detect lane pixels and fit to find the lane boundary.
* Determine the curvature of the lane and vehicle position with respect to center.
* Warp the detected lane boundaries back onto the original image.
* Output visual display of the lane boundaries and numerical estimation of lane curvature and vehicle position.

[//]: # (Image References)

[image1]: ./output_images/01_rawCamera_chessboard_undistorted.png "camera calibration derived from full set, applied to one of the chessboard images"
[image2]: ./output_images/02_mountedCamera_straightlines_undistorted_and_perspective.png "perspective calibration derived from straight road, applied to both images"
[image3]: ./output_images/03_warped_activated_lanepixels.png "activating pixels to get binary mask"
[video1]: ./out_project_video.mp4 "Video: project, showing reasonable performance"
[video2]: ./out_challenge_video.mp4 "Video: challenge, showing somewhat questionable performance"
[video3]: ./out_harder_challenge_video.mp4 "Video: harder challenge, not good at all, placeholder for future improvement"

## [Rubric](https://review.udacity.com/#!/rubrics/571/view) Points

### Here I will consider the rubric points individually and describe how I addressed each point in my implementation.  

---

### Writeup / README

#### 1. Provide a Writeup / README that includes all the rubric points and how you addressed each one.  You can submit your writeup as markdown or pdf.  [Here](https://github.com/udacity/CarND-Advanced-Lane-Lines/blob/master/writeup_template.md) is a template writeup for this project you can use as a guide and a starting point.  

* IPython notebook is [notebook]: ./project2.ipynb "project 2"

* Pipeline is implemented by the following modules:

1. Camera Calibration: *class Camera*

The code for this step is contained in the 1st code cell of the IPython notebook located in [notebook]: ./project2.ipynb "project 2". 

Camera class contains the intrinsic properties of a particular camera, it is initialized using a folder of chessboard images taken by the same camera.  The calibration is trivially done using the API introduced in class, using: [chessboard image set]: ./camera_cal/ "camera cal images". 

A "test image" is created by applying the calibration parameters to one of the chessboard images.
[image1]: ./output_images/01_rawCamera_chessboard_undistorted.png "camera calibration derived from full set, applied to one of the chessboard images"

2. Perspective Transforms: *class MountingPerspective*

The code for this step is contained in the 2nd code cell of the IPython notebook located in [notebook]: ./project2.ipynb "project 2". 

MountingPerspective class is separate from Camera class, as I see it as dependent on how the camera is mounted on the car.

This as an offline calibration step and provided an offline class method to plot the straight_lines image.  The calibration parameters are just pixel locations read out by the user, when moving cursor on the image.  The implementation is the same as introduced in class, but with actual pixel numbers read out from the two lanes in [notebook]: ./project2.ipynb "project 2". 

I see this as an offline step and implement the code as if the cal parameters are saved and ready to be loaded when initialized.

[image2]: ./output_images/02_mountedCamera_straightlines_undistorted_and_perspective.png "perspective calibration derived from straight road, applied to both images"

3. Binary image: *class LaneActivator*

The code for this step is contained in the 3rd code cell of the IPython notebook located in [notebook]: ./project2.ipynb "project 2". 

LaneActivator class includes image processing steps tailored to the properties of lanes.

Gradient is done in L-channel of the image after converting to HSL, mimicing what's introduced in class, with some experimentation of threshold values;  I ended up applying thresholds to both (dx **2 + dy ** 2) ** 0.5 and the angle arctan, thinking that even with perspective transforms some lanes will still be at an angle to Y-axis.

Intensity threshold is applied to S-channel, following the intuition provided in class.

I added a color enhancing method *highlightLane*, after experimenting with challenge video.  My approach is based on the fact that lanes are either yellow or white, so I can use cv2.inRange() to pick up such colors and artificially suppress the light level of the *rest* of the image.  I implemented this step in HSV space because I am more familiar with that particular channel defition.

I used two test images in ./test_images/ for testing.
[image3]: ./output_images/03_warped_activated_lanepixels.png "activating pixels to get binary mask"

4. A lane of two lines: *class Line*, *class Lane*

The code for this step is contained in the 4-5th code cell of the IPython notebook located in [notebook]: ./project2.ipynb "project 2". 

The idea of using these two classes came from the class.  It does seem natural to have *Line* handle extracting, updating and book-keeping, and *Lane* manages two Line objects.

In particular, *Lane* is responsible to validate two lines, using criterias such as distance between two lines, similarity of their curvatures.  If the two lines do not meet the requirement, the result needs to be discarded.

For this purpose, I implemented in *class Line* two methods, discardLastTick & keepLastTick, so that *Lane* can manage to the two lines.

Each line keeps a history of its previous state, and updates if a confirmation is received via *keepLastTick* sent by *Lane*, or simply discards the current result if *discardLastTick* is ordered by *Lane*.

For simplicity, I did not keep all history pixel points, instead I keep a counter of past frames (max at 10, i.e. ~ 0.3 sec for a 30FPS camera) and use that as a weight factor to update *Line*'s status, putting a higher emphasis on previous knowledge.

The rest of the implementation is rather trivial.

5. Commanding everything: *class Car*

This is the top level class.  All components in previous steps are created as members of this Car class.

The *Car* initializes *Camera*, sets up *MountingPerspective* transforms, creates a handle for *LaneActivator* and manages the *Lane* and its childen *Line*'s.


* Working on the videos using the pipleline:

A *Car* is created, and its method *processImage* is passed as a handle to:
clip = VideoFileClip("project_video.mp4")
out_clip = clip.fl_image(my_car.processImage)

[video1]: ./out_project_video.mp4 "Video: project, showing reasonable performance"
[video2]: ./out_challenge_video.mp4 "Video: challenge, showing somewhat questionable performance"

I could not get any reasonble result for the "harder challenge" video, in fact, mid-way through the video I lost lanes completely, even with a buffer with history.

---

### Discussion

1. What worked well: following instructions and hints provided in class, I feel that separating functionalities into individual class modules really helped, making debugging and testing much easier.

2. What didn't work well:

I still feel all the thresholds (for s-channel, for gradients, for number of pixels required to establish a lane line segment) are very hacky.  I spent a lot of time experimenting but didn't feel like I was going in the right direction, in most of the cases, I couldn't draw any conclusion and decided to revert the threshold values.

I sure hope a more systematic approach will be explained in class.