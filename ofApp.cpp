#include "ofApp.h"

void ofApp::setup(){
    ofSetFullscreen(true);

    ofSetFrameRate(25);
    ofBackground(0);
    ofFill();

    string scene = "encounter";
    pm.load(scene + "_poses.txt", ofGetWidth(), ofGetHeight());
    ism.load(scene + "_inpaint");
    tsps.setup();

    assert(pm.size() == ism.size());

    // Triadic: ORANGE, PURPLE, GREEN
    // Complementary: PINK, GREEN
    // PINK = ORANGE + PURPLE

    colors["pink"]   = ofColor(250, 29, 177);
    colors["green"]  = colors["pink"].getInverted();
    colors["orange"] = ofColor(233, 87, 30);
    colors["purple"] = ofColor(133, 89, 250);
    colors["red"]    = ofColor(245, 15, 50);

    embodiedIndex = -1;
}


void ofApp::update(){
    pm.update();        // pm.currPoses  -> unordered_map<int, ofPolyline>
    ism.update();       // ism.currImg   -> ofImage
    tsps.getNextPose(); // tsps.currBlob -> ofPolyline via currBlob.pline;

    if (!tsps.haveBlob) return;

    // Find all intersections, check IOU for embodiment!
    intersections.clear();
    for (auto const &kv : pm.currPoses) {
        if (kv.first == embodiedIndex)
            continue;

        ofx::Clipper clipper;
        clipper.addPolyline(kv.second, ClipperLib::ptSubject);
        clipper.addPolyline(tsps.currBlob.pline, ClipperLib::ptClip);
        vector<ofPolyline> isects = clipper.getClipped(ClipperLib::ClipType::ctIntersection);

        // if found intersection
        if (isects.size() > 0) {

            // Compute IOU
            float I = 0;
            for (ofPolyline &p : isects)
                I += fabs(p.getArea());
            float U = fabs(kv.second.getArea())
                + fabs(tsps.currBlob.pline.getArea()) - I;
            float IOU = I / U;

            // TODO: once embodied, can't embody again! Need to check once embodied character dissappears
            // Check if should embody!
            if (IOU > 0.5f) {
                cout << "EMBODIED THE CHARACTER!" << endl;
                embodiedIndex = kv.first;
            }

            // add to `intersections` to be drawn
            intersections.reserve(isects.size());
            intersections.insert(intersections.end(), isects.begin(), isects.end());
        }
    }
}


void ofApp::draw(){
    ofSetColor(colors["green"]); // Can do some cool stuff with this!
    ism.draw(ofGetWidth(), ofGetHeight());

    ofSetColor(colors["orange"]);
    pm.draw(ofGetWidth(), ofGetHeight(), embodiedIndex);

    ofSetColor(colors["purple"]);
    tsps.draw();

    ofSetColor(colors["pink"]);
    for (auto& line: intersections) {
        ofBeginShape();
        for (auto &v : line)
            ofVertex(v);
        ofEndShape();
    }
}


void ofApp::keyPressed(int key){
    if (key == 'r') {
        embodiedIndex = -1;
    }
}


void ofApp::windowResized(int w, int h){
    pm.rescalePoses(w, h);
}
