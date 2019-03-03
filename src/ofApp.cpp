#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    initDataFolderFromBundle();

    fps         = 25;
    drawWidth   = ofGetScreenWidth();
    drawHeight  = static_cast<int>(ofGetScreenHeight() * ofGetScreenWidth()/1920);
    drawX       = 0;
    drawY       = static_cast<int>((ofGetScreenHeight()-drawHeight)/2);

    ofSetFrameRate(fps);
    ofSetVerticalSync(true);
    ofHideCursor();

    camWidth    = 1920;
    camHeight   = 1080;

    vidGrabber.listDevices();
    vidGrabber.setDeviceID(0);
    vidGrabber.setDesiredFrameRate(fps);
    vidGrabber.initGrabber(camWidth, camHeight);

    outputFbo = new ofFbo();
    outputFbo->allocate(ofGetScreenWidth(),ofGetScreenHeight(),GL_RGBA);
    outputFbo->begin();
    ofClear(0,0,0,255);
    outputFbo->end();

    warpController = new ofxWarpController();
    warpController->loadSettings("warpingSetting.json");
    /*shared_ptr<ofxWarpPerspectiveBilinear>  warp = warpController->buildWarp<ofxWarpPerspectiveBilinear>();
    warp->setSize(ofGetScreenWidth(),ofGetScreenHeight());
    warp->setEdges(glm::vec4(0.03f, 0.03f, 0.03f, 0.03f));
    warp->setExponent(1.2f);*/

    nDelayFrames    = 25*20; // 20 seconds
    capturedFrame   = 0;

    videoBuffer.setup(nDelayFrames);

    resetTime       = ofGetElapsedTimeMillis();
    wait            = 1000/fps;

    loaded          = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    vidGrabber.update();

    if(vidGrabber.isFrameNew()){
        ofxCvColorImage        colorImage;
        colorImage.allocate(camWidth,camHeight);
        colorImage.setFromPixels(vidGrabber.getPixels());
        colorImage.mirror(false,true);
        colorImage.updateTexture();

        if(ofGetElapsedTimeMillis()-resetTime > wait){
            resetTime       = ofGetElapsedTimeMillis();
            videoBuffer.pushTexture(colorImage.getTexture());

            if(capturedFrame >= nDelayFrames){
                if(delayFrame < nDelayFrames-1){
                    delayFrame++;
                }else{
                    delayFrame = 0;
                }
            }else{
                capturedFrame++;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);

    ofSetColor(255);
    if(capturedFrame >= nDelayFrames){
        outputFbo->begin();
        ofSetColor(255);
        videoBuffer.getDelayedtexture(delayFrame).draw(drawX,drawY,drawWidth,drawHeight);
        outputFbo->end();

        warpController->getWarp(0)->draw(outputFbo->getTexture());
    }

    if(!loaded){
        loaded = true;
        ofToggleFullscreen();
    }

}

//--------------------------------------------------------------
void ofApp::exit(){
    warpController->saveSettings("warpingSetting.json");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    warpController->onKeyPressed(key);

    if(key == 'f'){
        ofToggleFullscreen();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    warpController->onKeyReleased(key);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    warpController->onMouseMoved(x,y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    warpController->onMouseDragged(x,y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    warpController->onMousePressed(x,y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    warpController->onMouseReleased(x,y);
}

//--------------------------------------------------------------
void ofApp::initDataFolderFromBundle(){

    string _bundleDataPath;

    CFURLRef appUrl = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef appPath = CFURLCopyFileSystemPath(appUrl, kCFURLPOSIXPathStyle);

    const CFIndex kCStringSize = 128;
    char temporaryCString[kCStringSize];
    bzero(temporaryCString,kCStringSize);
    CFStringGetCString(appPath, temporaryCString, kCStringSize, kCFStringEncodingUTF8);
    std::string *appPathStr = new std::string(temporaryCString);
    CFRelease(appUrl);
    CFRelease(appPath);

    CFURLRef resourceUrl = CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());
    CFStringRef resourcePath = CFURLCopyFileSystemPath(resourceUrl, kCFURLPOSIXPathStyle);

    bzero(temporaryCString,kCStringSize);
    CFStringGetCString(resourcePath, temporaryCString, kCStringSize, kCFStringEncodingUTF8);
    std::string *resourcePathStr = new std::string(temporaryCString);
    CFRelease(resourcePath);
    CFRelease(resourceUrl);

    _bundleDataPath = *appPathStr + "/" + *resourcePathStr + "/"; // the absolute path to the resources folder

    const char *homeDir = getenv("HOME");

    if(!homeDir){
        struct passwd* pwd;
        pwd = getpwuid(getuid());
        if (pwd){
            homeDir = pwd->pw_dir;
        }
    }

    string _APPDataPath(homeDir);
    userHome = _APPDataPath;

    _APPDataPath += "/Documents/CloseCircuitDelay/data";

    std::filesystem::path tempPath(_APPDataPath.c_str());

    _sysappPath = tempPath;

    ofDirectory APPDir;

    // data directory
    if(!APPDir.doesDirectoryExist(_sysappPath)){
        APPDir.createDirectory(_sysappPath,true,true);

        std::filesystem::path dataPath(_bundleDataPath.c_str());

        ofDirectory dataDir(dataPath);
        dataDir.copyTo(_sysappPath,true,true);
    }

    ofSetDataPathRoot(_sysappPath); // tell OF to look for resources here
}
