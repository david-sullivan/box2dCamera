#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "ofxOpenCv.h"

//#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
								// otherwise, we'll use a movie file

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		vector <ofPolyline>                  lines;
		ofxBox2d                             box2d;
		vector <shared_ptr<ofxBox2dCircle>>  circles;
		vector <shared_ptr<ofxBox2dEdge>>    edges;

		#ifdef _USE_LIVE_VIDEO
		ofVideoGrabber 		vidGrabber;
		#else
		ofVideoPlayer 		vidPlayer;
		#endif
		ofPixels				pix;
		ofTexture				tex;
		ofxCvColorImage			colorImg;

		ofxCvGrayscaleImage 	grayImage;
		ofxCvGrayscaleImage 	grayBg;
		ofxCvGrayscaleImage 	grayDiff;

		ofxCvContourFinder 	contourFinder;
		ofPolyline                            shape;
		vector <shared_ptr<ofxBox2dPolygon>>  triangles;

		int 				threshold;
		bool				bLearnBakground;

		//
		int minArea;
		int maxArea;
		int numConsidered;
		int margin;
		bool findHoles;
		//
		int blur;
		int numBlobs;
		//

		bool bDrawLines;

		float smoothPct;
		int numSmoothContours;
		int tolerance;
		int boxGridStep;
		float boxMass;
		float boxBounce;
		float boxFriction;

		float strength;
		float damping;
		float minDis;

		float lineFriction;
		float lineRestitution;
		float lineDensity;
};
