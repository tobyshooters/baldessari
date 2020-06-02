#include "ofApp.h"

void ofApp::setup() {
    ofSetFrameRate(30);
    ofBackground(0);
    ofFill();

    scene = "leandre";
    pm.load(scene + "_poses.txt", ofGetWidth(), ofGetHeight());
    ism.load(scene + "_frames");
    tsps.setup();

    colors["pink"]   = ofColor(250, 29, 177);
    colors["green"]  = colors["pink"].getInverted();
    colors["orange"] = ofColor(233, 87, 30);
    colors["purple"] = ofColor(155, 93, 229);
    colors["yellow"] = ofColor(255, 211, 0);
    colors["blue"] = ofColor(80, 219, 255);
    colors["red"]  = ofColor(245, 15, 50);

    shader.load("shaders/shader");

    saveFrames = false;
    embodiedIndex = -1;
}


void ofApp::update(){
    pm.update();
    ism.update();
    tsps.getNextPose();

    if (!tsps.haveBlob) return;

    /* intersections.clear(); */
    /* for (auto const &kv : pm.currPeople) { */
    /*     if (kv.first == embodiedIndex) */
    /*         continue; */

    /*     ofx::Clipper clipper; */
    /*     clipper.addPolyline(kv.second.poly, ClipperLib::ptSubject); */
    /*     clipper.addPolyline(tsps.currBlob.pline, ClipperLib::ptClip); */
    /*     vector<ofPolyline> isects = clipper.getClipped(ClipperLib::ClipType::ctIntersection); */

    /*     if (isects.size() > 0) { */
    /*         float I = 0; */
    /*         for (ofPolyline &p : isects) */
    /*             I += fabs(p.getArea()); */
    /*         float U = fabs(kv.second.poly.getArea()) + fabs(tsps.currBlob.pline.getArea()) - I; */
    /*         float IOU = I / U; */

    /*         // TODO: once embodied, can't embody again! Need to check once embodied character dissappears */
    /*         if (IOU > 0.5f) { */
    /*             cout << "EMBODIED THE CHARACTER!" << endl; */
    /*             embodiedIndex = kv.first; */
    /*         } */

    /*         intersections.reserve(isects.size()); */
    /*         intersections.insert(intersections.end(), isects.begin(), isects.end()); */
    /*     } */
    /* } */
}


void ofApp::draw(){
    ofSetColor(255);
    ism.draw(ofGetWidth(), ofGetHeight());

    ofSetColor(colors["orange"]);
    shader.begin();
    pm.draw(ofGetWidth(), ofGetHeight(), embodiedIndex);
    shader.end();

    /* ofSetColor(colors["purple"]); */
    /* tsps.draw(); */

    /* ofSetColor(colors["pink"]); */
    /* for (auto& line: intersections) { */
    /*     ofBeginShape(); */
    /*     for (auto &v : line) */
    /*         ofVertex(v); */
    /*     ofEndShape(); */
    /* } */

    if (saveFrames) {
        string frame_num = ofToString(ofGetFrameNum());
        string padded_frame_num = string(5 - frame_num.length(), '0') + frame_num;
        ofSaveScreen(scene + "_output/" + padded_frame_num + ".png");
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
