#include "ofMain.h"
#include "ofxOsc.h"

#define PORT 12000

struct TSPSBlob {
    int id;
    float cx;
    float cy;
    float ox;
    float oy;
    ofPolyline pline;

    int t_since_update;
    float distance;
};

class TSPS {
    public:
        TSPS() {};

        void setup() {
            rec.setup(PORT);
            haveBlob = false;
        }

        void draw() {
            ofBeginShape();
            for (auto &v : currBlob.pline)
                ofVertex(v);
            ofEndShape();
        }

        void getNextPose() {
            // Check new TSPS messages
            bool haveNext = false;
            ofxOscMessage next;

            while (rec.hasWaitingMessages()) {
                ofxOscMessage m;
                rec.getNextMessage(m);
                if (m.getAddress() == "/TSPS/personUpdated/") {

                    int pid = m.getArgAsInt(0);
                    if (pid == this->currBlob.id) {
                        next.copy(m);
                        haveNext = true;
                        break;
                    }

                    if (!haveNext) {
                        next.copy(m);
                        haveNext = true;
                    }
                }
            }

            // Consume rest of messages
            ofxOscMessage m;
            while (rec.hasWaitingMessages())
                rec.getNextMessage(m);

            // Process the selected message
            if (haveNext) {
                TSPSBlob nextBlob = this->processBlob(next);
                this->currBlob = nextBlob;
                haveBlob = true;
            }
        }

        bool haveBlob;
        TSPSBlob currBlob;

    private:

        /* Constructs a blob from message, with pline equal to 
           interpolation with current blob */

        TSPSBlob processBlob(ofxOscMessage m) {
            // Get id
            int pid = m.getArgAsInt(0);

            // Get contour
            ofPolyline pline;
            int argc = m.getNumArgs();
            for (int i = 20; i < argc; i += 2) {
                float x = ofMap(m.getArgAsFloat(i),     0, 1, 0, ofGetWidth());
                float y = ofMap(m.getArgAsFloat(i + 1), 0, 1, 0, ofGetHeight());
                pline.addVertex(x, y);
            }

            pline.close();
            pline.simplify(1.0);
            pline.getSmoothed(3, 1);

            // Get centroid
            float cx = ofMap(m.getArgAsFloat(3), 0, 1, 0, ofGetWidth());
            float cy = ofMap(m.getArgAsFloat(4), 0, 1, 0, ofGetHeight());

            // Get optical flow
            float ox = m.getArgAsFloat(18);
            float oy = m.getArgAsFloat(19);

            // Update candidates
            TSPSBlob candidate = { pid, cx, cy, ox, oy, pline, 0, 0 };
            return candidate;
        }

        ofxOscReceiver rec;
};
