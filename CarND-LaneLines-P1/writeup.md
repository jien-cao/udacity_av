**Finding Lane Lines on the Road**

The goals / steps of this project are the following:
* Make a pipeline that finds lane lines on the road
* Reflect on your work in a written report


[//]: # (Image References)

[image1]: ./examples/grayscale.jpg "Grayscale"

---

### Reflection

### 1. Describe your pipeline. As part of the description, explain how you modified the draw_lines() function.

1) convert the image to grayscale

    [note-1] my intuition was to also apply a filter here to pick out 'white' and 'yellow' colors when i saw the sample images all have white/yellow lanes.  after a few tests, i found that a threshold of >200 in grayscale roughly captures to white & yellow.  i tried applying this as a filter (cv2.inRange, then cv2.bitwise_and), and it seemed to work.  however, i later realized that the cv2.canny step actually implies the same, i.e. a strong edge can only be formed when there are pixels with high grayscale values (max_possible_gradient = max_pixel_grayscale - 0).  in the end, i stayed with a simple bgr->grayscale conversion

    [note-2] interestingly, this idea of picking up yellow and white turned out useful for the challenge video; a review of the troublesome frames show that sometimes the yellow lane could have a gray-ish road background, leading to a low contrast in grayscale, which led me to the following step


0) enhancing yellow (this is added after seeing the challenge video, but should be ahead of conversion to grayscale, hence numbered as step 0)

    as explained in [note-2] in (1), i wanted to pick out yellow and make it more "pronounced" in grayscale, for the challenge video, so i added the following
    
    0.a) convert image to HSV space, since colors seem more naturally defined in this space

    0.b) use cv2.inRange() in HSV space to get a mask of all shades of yellows
   
    0.c) enhance pixels at the masked positions in the original image (RGB), by simply setting them to high RGB values

2) canny edge (following the book)

3) houghlinesP (following the book)

    this step involves tweaking some parameters for houghlinesP: the tweaking feels very heuristic to me, the results are good, but i wonder if there's a more systematic approach ???

4) generate two lanes

    intuition is to pick the two most pronounced features, so i attempted to find the two most common "types" of lines from step 3).
    
    4.a) collect (Rho, Theta) values of all lines, and pick the two most popular pairs; my implementation is to "count" all lines from 3) in a (Rho, Theta) grid; it seems natural to follow hough transform convention here instead of (slope, intercept);
    
    4.b) a note here is that the process seems to be a poor duplication of houghlines (not houghlinesP), however the documentation for houghlines is hard to grasp and made me feel like another highly heuristic step of parameter tweaking, i tried it a few times and in the end stayed with my own implementation;
    
    4.c) pick the "hottest" two grids in my collection of (Rho, Theta) over a 2D grid of digitized range of all possible (R, T) values;
    
    4.d) "hottest" was initially defined as a counter, i.e. the number of lines from (3) that fall onto a certain (R, T) grid, however, through some tests i realized this definition incorrectly gave long and short lines the same weight
    
    4.e) "hottest" is implemented as a score, instead of adding a count of 1 to a grid when getting a line, i add the "length" of that line, so long lines contribute more to the score, seemingly more fitting for the goal of finding two lanes;
    
    4.f) "max and min" as the two lanes: it seems safe to assume the two lanes are always "close" to certain angles, one at positive angle to X-axis, the other negative; as a simplistic implementation, i define the "score" in 4.e to be positive for positive angle, negative score for negative angle, so that in this step i can do a numpy.argmax() to get one lane, numpy.argmin() to get the other.  
    
    4.g) validating range of angles: to improve robustness (particularly in view of the challenge video), i implemented a range for lane angles, e.g. the lanes will never to anywhere close to horizontal, so the abs(Theta) values should never be too small, i picked 15-degree as the lower bound, and similarly 60-degree as the upper bound.  the upper bound value was a bit surprising since i initially thought the lanes could certainly be vertical in field-of-view, but based on the videos, what camera sees is a projected view (infinit becomes a point), so the upper bound actually makes sense.


5) update the lane from history, before drawing

    5.a) i added this step to handle the rare cases when a frame might not have two visually pronounced lanes (or perhaps my parameter tuning wasn't that great);
    
    5.b) a static Class LaneState is used to save previously known lane positions
    
    5.c) for each new frame, if valid lanes are extracted, they are used to update the static LaneStates (history has a slightly higher weight, new values lower weight);
    
    5.d) if no valid lanes can be extracted in a frame, i simply re-use the LaneState from previous frame.
    
    5.e) this approach means the camera's FPS limits how fast the output "lanes" adapt to actual lanes at current time.


6) draw the two lanes (following the book)

    6.a) two "very long" lanes are sent to draw(), letting the draw() handles clipping;
    
    6.b) a simple region-of-interest is applied so we don't draw lines into top of the view (the sky)


### 2. Identify potential shortcomings with your current pipeline


Potential issues:

1) robustness: the parameters for houghlinesP are based on trial-and-error, i didn't tweak them extensively but felt this is a very weak step in my implementation

2) performance: my implementation in step 4 involves populating a 2D grid of all possible (Rho, Theta) values, this seems a very expensive step; also it seems to be a poor-man's version of houghlines (the rho, theta version)...


### 3. Suggest possible improvements to your pipeline

i feel it may be more effecient to direct use houghlines (Rho, Theta version) from the very beginning; i had trouble with cv2 documentation so i couldn't complete a reliable pipeline along that direction.
