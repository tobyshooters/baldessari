#include "ofMain.h"

typedef struct MotionDirectory {
    ofDirectory d;
    size_t idx;
    size_t num;
} MotionDirectory;


class ImageSequenceManager {
    public:
        ImageSequenceManager() {}

        void load(string path) {
            ofDirectory d(path);
            d.listDir();
            d.allowExt("png");
            d.allowExt("jpg");
            d.listDir();
            d.sort();

            m = { d, 0, d.size() };
        }

        void update() {
            currImg.load(m.d[m.idx]);
            m.idx = (m.idx + 1) % m.num;
        }

        void draw(int screenWidth, int screenHeight) {
            float scale = 1.f * screenHeight / currImg.getHeight();
            float videoHeight = screenHeight;
            float videoWidth = scale * currImg.getWidth();
            currImg.draw((screenWidth - videoWidth) / 2, 0, videoWidth, videoHeight);
        }

        int size() {
            return m.num;
        }

        ofImage currImg;

    private:
        MotionDirectory m;
};
