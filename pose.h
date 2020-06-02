#include "ofMain.h"

typedef struct person {
    ofPolyline poly;
    glm::vec2 head;
    int num;
} Person;

class PoseManager {
    public:
        PoseManager () {
            ofImage red;
            red.load("dots/red.png");
            dots.push_back(red);

            ofImage yellow;
            yellow.load("dots/yellow.png");
            dots.push_back(yellow);

            ofImage green;
            green.load("dots/green.png");
            dots.push_back(green);
        }

        void load(string path, int screenWidth, int screenHeight) {
            currFrameIdx = 0;

            // Load poses
            ofBuffer buffer = ofBufferFromFile(path);
            vector<string> lines = ofSplitString(buffer.getText(), "\n");
            int currLineIdx = 0;

            // Process first line
            vector<string> params = ofSplitString(lines[currLineIdx], " ");
            assert (params.size() == 3);
            numFrames = ofToInt(params[0]);
            frameHeight = ofToInt(params[1]);
            frameWidth = ofToInt(params[2]);

            // Read in ignore ids
            cout << "Kept person IDs: ";
            bool am_keeping = false;
            vector<int> keep;
            for (string s : ofSplitString(lines[++currLineIdx], " ")) {
                cout << s << " ";
                int id = ofToInt(s);
                if (id != -1) {
                    am_keeping = true;
                    keep.push_back(id);
                }
            }
            cout << endl;
            cout << "Using keep-out method: " << (am_keeping ? "true" : "false") << endl;

            // Read all lines, load the poses
            while (currLineIdx != lines.size() - 1) {

                vector<string> values = ofSplitString(lines[++currLineIdx], " ");
                int frame_id = ofToInt(values[0]);
                int num_people = ofToInt(values[1]);

                unordered_map<int, Person> frame_people;
                for (int _p = 0; _p < num_people; _p++) {

                    vector<string> values = ofSplitString(lines[++currLineIdx], " ");
                    int person_id = ofToInt(values[0]);
                    int person_num = ofToInt(values[1]);
                    glm::vec2 head(ofToInt(values[2]), ofToInt(values[3]));

                    if (am_keeping) {
                        if (find(keep.begin(), keep.end(), person_id) == keep.end()) {
                            continue;
                        }
                    }

                    int num_points = ofToInt(values[4]);
                    ofPolyline pose;
                    for (int i = 5; i < 2 * num_points + 1; i+=2) {
                        pose.addVertex(ofToInt(values[i]), ofToInt(values[i+1]));
                    }
                    pose.close();

                    Person person = { pose, head, person_num };
                    frame_people[person_id] = person;
                }
                people[frame_id] = frame_people;
            }

            // Scale poses to screen size!
            xOffset = 0;
            rescalePoses(screenWidth, screenHeight);
        }

        void update() {
            currFrameIdx = (currFrameIdx + 1) % numFrames;

            if (people.find(currFrameIdx) == people.end()) {
                unordered_map<int, Person> emptyMap;
                currPeople = emptyMap;
            } else {
                currPeople = people[currFrameIdx];
            }
        }

        void rescalePoses(int newScreenWidth, int newScreenHeight) {

            float scale = 1.f * newScreenHeight / frameHeight;
            frameHeight = newScreenHeight;
            frameWidth = scale * frameWidth;

            int newXOffset = (newScreenWidth - frameWidth) / 2;
            glm::vec2 d(newXOffset - xOffset, 0);
            xOffset = newXOffset;

            for (auto &clip_kv : people) {
                for (auto &kv : clip_kv.second) {
                    kv.second.poly.scale(scale, scale);
                    kv.second.poly.translate(d);
                    kv.second.head *= scale;
                    kv.second.head += d;
                }
            }
        }

        void draw(int screenWidth, int screenHeight, int embodiedIndex) {

            for (auto const& kv : currPeople) {
                if (kv.first == embodiedIndex)
                    continue;
                ofPolyline poly = kv.second.poly;

                // SHADER
                ofBeginShape();
                for (auto &v : poly)
                    ofVertex(v);
                ofEndShape();

                // DOTS BACKGROUND
                /* ofSetColor(255); */
                /* int step = 105/2; */
                /* int size = 100/2; */
                /* int i = 0; */
                /* for (int x = 0; x < ofGetWidth(); x += step) { */
                /*     for (int y = 0; y < ofGetHeight(); y += step) { */
                /*         i += 1; */

                /*         if (poly.inside(glm::vec3(x, y, 0))) { */
                /*             dots[i % dots.size()].draw(x - size/2, y - size/2, size, size); */
                /*         } */
                /*     } */
                /* } */

                // DEBUG: see person id
                /* ofSetColor(ofColor(255, 255, 255)); */
                /* glm::vec2 com = poly.getCentroid2D(); */
                /* ofDrawBitmapString(to_string(kv.first), com.x, com.y); */

            }
        }

        bool insideScene(int x, int y) {
            bool inside = false;
            for (auto const& item : currPeople) {
                ofPolyline pose = item.second.poly;
                inside = inside || pose.inside(x, y);
            }
            return inside;
        }

        int size() {
            return people.size();
        }

        unordered_map<int, Person> currPeople;
        int frameHeight;
        int frameWidth;
        int xOffset;

    private:
        // frame_id => { person_id => polyline }
        unordered_map<int, unordered_map<int, Person>> people;
        int currFrameIdx;
        int numFrames;
        vector<ofImage> dots;
};
