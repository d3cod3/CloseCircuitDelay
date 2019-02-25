#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxWarp.h"

#include <pwd.h>
#include <unistd.h>

class circularTextureBuffer{
public:
    circularTextureBuffer(){
        currentIndex = 0;
    }
    void setup(int numFrames){
        frames.resize(numFrames);
    }
    void pushTexture(ofTexture& tex){
        frames[currentIndex] = tex;
        if(currentIndex < frames.size()-1 ){
            currentIndex++;
        }else{
            currentIndex = 0;
        }

    }

    ofTexture& getDelayedtexture(size_t delay){
        if(delay < frames.size()){
            return frames[delay];
        }
        return frames[0];
    }

protected:
    int currentIndex;
    vector<ofTexture> frames;
};

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);

    void initDataFolderFromBundle();

    // CORE
    std::filesystem::path       _sysappPath;
    string                      userHome;

    ofxWarpController       *warpController;
    ofFbo                   *outputFbo;
    ofVideoGrabber          vidGrabber;
    int                     camWidth, camHeight;
    int                     drawX, drawY;
    int                     drawWidth, drawHeight;

    circularTextureBuffer   videoBuffer;
    int                     nDelayFrames;
    int                     delayFrame;


    size_t                  resetTime;
    size_t                  wait;

    int                     fps;
    size_t                  capturedFrame;

    bool                    loaded;

};
