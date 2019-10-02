//box2d interacting with shapes derived from opencv blobs
//borrowing code from the opencv example in the openframeworks examples and the box2d examples
//with hints from a couple of threads on the forum
//most of the examples i found were old pre-glm
//https://openframeworks.cc/learning/02_graphics/how_to_use_glm/
//this is my attempt at getting something that works with of_v0.10.1_vs2017


#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofDisableAntiAliasing();
	ofBackgroundHex(0xfdefc2);
	ofSetLogLevel(OF_LOG_NOTICE);
	ofSetVerticalSync(true);

	// Box2d
	box2d.init();
	box2d.setGravity(0, 30);
	box2d.createGround();
	box2d.setFPS(60.0);

	//
	minArea = 0;
	maxArea = 10000;
	numConsidered = 10;
	margin = 20;
	findHoles = false;
	//
	threshold = 80;
	blur = 3;
	bLearnBakground = true;
	//

	bDrawLines = false;

	smoothPct = 0.75f; //is smooth a percent from 0 -1?

	lineFriction = 0.2f;
	lineRestitution = 0.96f;
	lineDensity = 1.0f;


#ifdef _USE_LIVE_VIDEO
	vidGrabber.setVerbose(true);
	vidGrabber.setup(320, 240);
#else
	vidPlayer.load("fingers.mov");
	vidPlayer.play();
	vidPlayer.setLoopState(OF_LOOP_NORMAL);

#endif
	colorImg.allocate(ofGetWidth(), ofGetHeight());
	grayImage.allocate(ofGetWidth(), ofGetHeight());
	grayBg.allocate(ofGetWidth(), ofGetHeight());
	grayDiff.allocate(ofGetWidth(), ofGetHeight());

	bLearnBakground = true;
	threshold = 20;
}

//--------------------------------------------------------------
void ofApp::update() {
	// add some circles every so often
	if ((int)ofRandom(0, 10) == 0) {
		auto c = make_shared<ofxBox2dCircle>();
		c->setPhysics(0.2, 0.2, 0.002);
		c->setup(box2d.getWorld(), ofRandom(1, 320), -20, ofRandom(3, 10));
		c->setVelocity(0, 15); // shoot them down!
		circles.push_back(c);
	}

	bool bNewFrame = false;

#ifdef _USE_LIVE_VIDEO
	vidGrabber.update();
	bNewFrame = vidGrabber.isFrameNew();
#else
	vidPlayer.update();
	bNewFrame = vidPlayer.isFrameNew();
#endif

	if (bNewFrame) {

#ifdef _USE_LIVE_VIDEO
		colorImg.setFromPixels(vidGrabber.getPixels());
#else
		colorImg.setFromPixels(vidPlayer.getPixels());
#endif

		grayImage = colorImg;
		if (bLearnBakground == true) {
			grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
			bLearnBakground = false;
		}

		// take the abs value of the difference between background and incoming and then threshold:
		grayDiff.absDiff(grayBg, grayImage);
		grayDiff.threshold(threshold);

		//clear last frames objects
		contourFinder.blobs.clear();
		triangles.clear();
		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder.findContours(grayDiff, minArea, maxArea, numConsidered, findHoles);	// find holes
	}
	for (int i = contourFinder.blobs.size() - 1; i >= 0; i--)
	{

		auto pts = contourFinder.blobs[i].pts;

		//ofxBox2dPolygon poly;
		auto poly = std::make_shared<ofxBox2dPolygon>();

		//for (int i = 0; i < pts.size(); i++)
		//{
		//	poly->addVertex(pts[i]);
		//}
		//poly.setAsEdge(false);
		poly->addVertices(pts);
		poly->simplify(0.9);
		poly->triangulate();
		poly->setPhysics(1.0, 0.3, 0.3);
		poly->create(box2d.getWorld());
		triangles.push_back(poly);
	}

	// remove shapes offscreen
	ofRemove(circles, [](shared_ptr<ofxBox2dCircle> shape) -> bool {
		return !ofRectangle(0, -400, ofGetWidth(), ofGetHeight() + 400).inside(shape->getPosition());
	});

	box2d.update();
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofBackground(255, 150, 124);
	//colorImg.draw(20, 20);
	grayImage.draw(0,0);
	//grayDiff.draw(0, 0);
	//grayImage.draw(10, 280, ofGetWidth() / 4, ofGetHeight() / 4);
	//grayBg.draw(ofGetWidth() / 2 - ofGetWidth() / 8, 280, ofGetWidth() / 4, ofGetHeight() / 4);
	//grayDiff.draw(ofGetWidth() - ofGetWidth() / 4, 280, ofGetWidth() / 4, ofGetHeight() / 4);


	// or, instead we can draw each blob individually from the blobs vector,
	// this is how to get access to them:
		//for (int i = 0; i < contourFinder.nBlobs; i++) {
		//	contourFinder.blobs[i].draw(0, 0);

		//	// draw over the centroid if the blob is a hole
		//	//ofSetColor(255);
		//	//if (contourFinder.blobs[i].hole) {
		//	//	ofDrawBitmapString("hole",
		//	//		contourFinder.blobs[i].boundingRect.getCenter().x + 360,
		//	//		contourFinder.blobs[i].boundingRect.getCenter().y + 540);
		//	//}
		//}

	for (auto &circle : circles) {
		ofFill();
		ofSetHexColor(0xc0dd3b);
		circle->draw();
	}

	ofSetHexColor(0x444342);
	ofNoFill();
	for (auto &line : lines) {
		line.draw();
	}
	for (auto & edge : edges) {
		edge->draw();
	}

	// we could draw the whole contour finder
	//contourFinder.draw(0, 0);
	// Triangulation
	ofSetHexColor(0x443342);
	//bFill ? ofFill() : ofNoFill();
	for (auto & poly : triangles) {
		poly->isTriangulated() ? poly->drawTriangles() : poly->draw();
	}

	string info = "Draw a shape with the mouse\n";
	info += "Press 1 to add some circles\n";
	info += "Press c to clear everything\n";

	ofSetHexColor(0x444342);
	ofDrawBitmapString(info, 10, 15);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
	case ' ':
		bLearnBakground = true;
		break;
	case '+':
		threshold++;
		if (threshold > 255) threshold = 255;
		break;
	case '-':
		threshold--;
		if (threshold < 0) threshold = 0;
		break;
	case 'c':
		lines.clear();
		edges.clear();
		break;
	case '1':
		auto c = make_shared<ofxBox2dCircle>();
		c->setPhysics(1, 0.5, 0.5);
		c->setup(box2d.getWorld(), mouseX, mouseY, 10);
		circles.push_back(c);
		break;
	}

	/*
	// want to save out some line...
	if(key == ' ') {
		ofstream f;
		f.clear();
		f.open(ofToDataPath("lines.txt").c_str());
		for (int i=0; i<lines.size(); i++) {
			for (int j=0; j<lines[i].size(); j++) {
				float x = lines[i][j].x;
				float y = lines[i][j].y;
				f << x << "," << y << ",";
			}
			f << "\n";
		}
		f.close();lines.clear();
	}*/
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	lines.back().addVertex(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	lines.push_back(ofPolyline());
	lines.back().addVertex(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	auto edge = make_shared<ofxBox2dEdge>();

	lines.back().simplify();

	for (int i = 0; i < lines.back().size(); i++) {
		edge->addVertex(lines.back()[i]);
	}

	// edge->setPhysics(1, .2, 1);  // uncomment this to see it fall!
	edge->create(box2d.getWorld());
	edges.push_back(edge);
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
