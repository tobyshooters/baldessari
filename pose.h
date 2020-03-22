#include "ofMain.h"

class PoseManager {
    public:
        PoseManager () {}

        void load(string path, int screenWidth, int screenHeight) {
            currFrameIdx = 0;

            // Load poses
            ofBuffer buffer = ofBufferFromFile(path);
            vector<string> lines = ofSplitString(buffer.getText(), "\n");
            int currLineIdx = 0;

            // Process first line
            vector<string> params = ofSplitString(lines[currLineIdx], " ");
            assert (params.size() == 3);
            int numFrames = ofToInt(params[0]);
            frameHeight = ofToInt(params[1]);
            frameWidth = ofToInt(params[2]);

            // Load in all the poses
            for (int frame_id = 0; frame_id < numFrames; frame_id++) {
                unordered_map<int, ofPolyline> frame_poses;

                vector<string> values = ofSplitString(lines[++currLineIdx], " ");
                int num_people = ofToInt(values[0]);

                for (int person = 0; person < num_people; person++) {

                    vector<string> values = ofSplitString(lines[++currLineIdx], " ");
                    int person_id = ofToInt(values[0]);
                    int num_points = ofToInt(values[1]);

                    ofPolyline pose;
                    for (int i = 2; i < 2 * num_points + 1; i+=2) {
                        pose.addVertex(ofToInt(values[i]), ofToInt(values[i+1]));
                    }
                    pose.close();

                    frame_poses[person_id] = pose;
                }
                poses.push_back(frame_poses);
            }

            // Scale poses to screen size!
            xOffset = 0;
            rescalePoses(screenWidth, screenHeight);
        }

        void update() {
            currFrameIdx = (currFrameIdx + 1) % poses.size();
            currPoses = poses[currFrameIdx];
        }

        void rescalePoses(int newScreenWidth, int newScreenHeight) {

            float scale = 1.f * newScreenHeight / frameHeight;
            frameHeight = newScreenHeight;
            frameWidth = scale * frameWidth;

            int newXOffset = (newScreenWidth - frameWidth) / 2;
            glm::vec2 d(newXOffset - xOffset, 0);
            xOffset = newXOffset;

            for (unordered_map<int, ofPolyline> &clip : poses) {
                for (auto &kv : clip) {
                    kv.second.scale(scale, scale);
                    kv.second.translate(d);
                }
            }
        }

        void draw(int screenWidth, int screenHeight, int embodiedIndex) {

            for (auto const& kv : currPoses) {
                if (kv.first == embodiedIndex)
                    continue;

                ofPolyline poly = kv.second;

                ofBeginShape();
                for (auto &v : poly)
                    ofVertex(v);
                ofEndShape();
            }
        }

        bool insideScene(int x, int y) {
            bool inside = false;
            for (auto const& item : currPoses) {
                ofPolyline pose = item.second;
                inside = inside || pose.inside(x, y);
            }
            return inside;
        }

        int size() {
            return poses.size();
        }

        unordered_map<int, ofPolyline> currPoses;
        int frameHeight;
        int frameWidth;
        int xOffset;

    private:
        vector<unordered_map<int, ofPolyline>> poses;
        int currFrameIdx;
};
