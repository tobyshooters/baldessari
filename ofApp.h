#pragma once

#include "ofMain.h"
#include "ofxClipper.h"
#include "pose.h"
#include "ism.h"
#include "tsps.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void keyPressed(int key);
		void windowResized(int w, int h);
		
        unordered_map<string, ofColor> colors;
        int embodiedIndex;

        PoseManager pm;
        ImageSequenceManager ism;
        TSPS tsps;
        vector<ofPolyline> intersections;
};
