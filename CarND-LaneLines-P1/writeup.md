**Finding Lane Lines on the Road**

The goals / steps of this project are the following:
* Make a pipeline that finds lane lines on the road
* Reflect on your work in a written report


[//]: # (Image References)

[image1]: ./examples/grayscale.jpg "Grayscale"

---

### Reflection

### 1. Describe your pipeline. As part of the description, explain how you modified the draw_lines() function.

My pipeline consisted of 5 steps. First, I converted the images to grayscale, then I .... 

1) convert the image to grayscale
   [notes of an abandoned filter step] my intuition was to also apply a filter here to pick out 'white' and 'yellow' colors when i saw the sample images all have white/yellow lanes.  after a few tests, i found that a threshold of >200 in grayscale roughly captures to white & yellow.  i tried applying this as a filter (cv2.inRange, then cv2.bitwise_and), and it seemed to work.  however, i later realized that the cv2.canny step actually implies the same, i.e. a strong edge can only be formed when there are pixels with high grayscale values (max_possible_gradient = max_pixel_grayscale - 0).  in the end, i stayed with a simple bgr->grayscale conversion
2) 
In order to draw a single line on the left and right lanes, I modified the draw_lines() function by ...

If you'd like to include images to show how the pipeline works, here is how to include an image: 

![alt text][image1]


### 2. Identify potential shortcomings with your current pipeline


One potential shortcoming would be what would happen when ... 

Another shortcoming could be ...


### 3. Suggest possible improvements to your pipeline

A possible improvement would be to ...

Another potential improvement could be to ...
