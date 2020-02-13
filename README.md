# box2dCamera
an attempt to have ofxBox2d objects collide with ofxOpenCV blobs in openframeworks v0.10.1

Based on the many examples in Vanderlin's Openframeworks addon ofxBox2d
https://github.com/vanderlin/ofxBox2d

mashed up with the opencv example in openframeworks
https://openframeworks.cc/

For more about computer vision techniques, read this:
https://github.com/openframeworks/ofBook/blob/master/chapters/image_processing_computer_vision/chapter.md

All of the examples for using ofxBox2d with ofxOpenCv on the web were older than the new glm syntax,
and no longer worked.

I'm not a c++ programmer, so i am sure there is a lot in the code that could be changed or optimized.
If you have any suggestions, please let me know.

I have tried using similar code with streaming video, and it always quits after a few minutes. I would love to figure that out.

Getting back to this after a while, i realized that triangulating the contours was an uneccesary step.
Also i found a couple of examples (like this https://www.youtube.com/watch?v=d3URBJLciEw by Lewis Lepton) that turned the opencv contour into a box2d polyline. Much better and dependable  performance.
