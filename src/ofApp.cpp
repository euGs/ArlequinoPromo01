#include "ofApp.h"

/*
 Definitely
 ----------
 Add some light flickering as if the light source is a candle or affected by wind or whatever.

 Start off with a single color for the agents, then start bringing the final texture in.
 Do it via a fragment shader.

 Do camera movements in principle like unnamed sound sculpture. Start a bit farther
 back than the average distance to the origin and move around the origin a bit.

 Glitch, incl. just changing seeding on movements
 
 Glitch: make it look like the camera has accidentally moved on the horizontal -
 not an orbit but a short translation on the horizontal - and back again - gives
 the sense of a camera actually being there
 
 Add easing to transitions
 
 Background colour changing - probably responding to music. But nice and smooth. Maybe
 with some random/noise gradations.

 Maybe
 -----
 Give agents random orientations: it looks like they are all aligned to the same plane.
 
 Get agents following text even better.
 
 Get particles back in - try outside of camera rendering (always view oriented)
 
 Not any more
 ------------
 As agents switch from sphere roving to text roving and back, they always move
 to a new location and the camera has to pan to follow. Finally when they
 move into position for the poster the camera has to tilt.
 
 Wind effects: Have the paper respond to wind. The strength and direction of the wind
 is driven by noise. Each tear would then need to respond to both wind and music,
 which probably means a force-based system (which is difficult, because the response
 to music isn't so).
 
 Agent type for one that gets pushed along as if by wind: can accept a force
 that pushes it forward and up a bit, while a second force (due to gravity) is
 pulling it down.
 
 Script/control from ofApp / create a Director class / just do it live for
 starting with one agent that gets pushed along as if by wind. It flies out
 of frame and camera pans to follow / get back in frame. Then we see that
 it's heading to a particular location and many other agents are going too.
 When they all arrive they start following the music (SphereRovingAgent with
 sphere radius set to music sound intensity). As the animation goes on and
 agents switch from sphere roving to text roving and back, they always move
 to a new location and the camera has to pan to follow. Finally when they 
 move into position for the poster the camera has to tilt.
 
 Visual test
 -----------
 Check speed of paper tears (also on text)
 
 Mastering
 ---------
 Colorisation options
 
 Redo image so you can see his legs.
 
 Rendering
 ---------
 Try export to 3D
 
 Try import to Blender and render there
 
 Extras
 -----
 Flapping paper tears, like birds. First try didn't work. Killed frame rate. But I
 had timing values/calculations in each of the 100 tears. With only one calculation
 leading to a normalised value (in Agents class), it might ease the load a bit. Flapping
 could also be binary-state/basic animation, i.e. only have 2 frames.
 
 Nicer
 ----
 Use time literals for animations: 1s or 1.s whatever C++14 will allow
 
 Make it explicit that transitions are non-interruptable. Interrupting one now will
 result in a null pointer exception.
 
 Have state on Animator instead of bool - reads better
 
 */

//--------------------------------------------------------------
void ofApp::setup(){
    visualisationSource.setImageFilename("Cover01.jpg");
    visualisationSource.setGridDimensions(Cols, Rows);
    visualisationSource.setup();
    sphereRovingAgentSource.setup();

    agents.setup(sphereRovingAgentSource, visualisationSource, MaxAgents);
    music.setup("ArTeaser_Edit04.wav");
    shader.load("shaders_gl3/topLighting");
    
    textRovingAgentSource.setup();
    gridAgentSource.setDimensions(Cols, Rows, visualisationSource.getColWidth(), visualisationSource.getRowHeight());
    
    poster.setup("Cover01.jpg");
    
    blur.setup(ofGetWidth()*DesiredCamDistance/DefaultCamDistance, ofGetHeight()*DesiredCamDistance/DefaultCamDistance);
    blur.setBlurStrength(1.f);

    texts.setup();
    texts.addText("ARLEQUINO", "Ubuntu-R.ttf", 380);
    texts.addText("DEBUT EP\nOUT NOW", "Ubuntu-R.ttf", 450);
    texts.addText("WWW.ARLEQUINO.BAND", "Ubuntu-R.ttf", 200);

    cam.setPosition(0.f, 0.f, DesiredCamDistance);
    shadowCam.setPosition(0.f, -DesiredCamDistance, 0.f);
    shadowCam.setOrientation({90.f, 0.f, 0.f});
    shadowFbo.allocate(1600, 1600, GL_RGBA);
    shadowPlane.set(1600, 1600);
    shadowPlane.setPosition(shadowPosition);
    shadowPlane.setOrientation(shadowOrientation);
    shadowPlane.mapTexCoords(0, 0, 1600, 1600);
    
    ofBackground(255.f);
}

//--------------------------------------------------------------
void ofApp::update(){
    music.update();
    float visualScaling = music.getLevel() * 25.f;
    agents.update(visualScaling);
    cam.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    // Render agents into shadow FBO.
    shadowFbo.begin();
    ofClear(255.f, 0.f);
    shadowCam.begin();
    shader.begin();
    shader.setUniform1f("alpha", ofMap(music.getLevel(), 0.f, 0.15f, 0.f, 1.f, true));
    agents.drawUntextured();
    shader.end();
    shadowCam.end();
    shadowFbo.end();
    
    cam.begin();

    // Draw agents.
    shader.begin();
    shader.setUniform1f("alpha", ofMap(music.getLevel(), 0.f, 0.15f, 0.f, 1.f, true));
    shader.setUniform1f("toplightStartY", 800.f);
    shader.setUniform1f("toplightIntensity", .45f);
    shader.setUniform1f("topLightEndY", -800.f);
    shader.setUniform1f("ambientLight", .8f);
    agents.draw();
    shader.end();
    
    texts.draw();
    poster.draw();
    
    // Draw shadow.
    shadowFbo.getTexture().bind();
    shadowPlane.draw();
    shadowFbo.getTexture().unbind();
    
    cam.end();

    // Draw debug.
    ofPushStyle();
    ofSetColor(0, 0, 0);
    ofDrawBitmapString(ofGetFrameRate(), 20, 20);
    ofDrawBitmapString(ofGetMouseX(), 20, 40);
    ofDrawBitmapString(ofGetMouseY(), 20, 60);
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == 't'){
        texts.cycleText();
        textRovingAgentSource.setLetterPaths(texts.getLetterPaths(), texts.getDrawPosition());
        agents.transitionAgents(textRovingAgentSource, 1.f);
        texts.animateIn();
    }else if (key == 's'){
        agents.transitionAgents(sphereRovingAgentSource, 1.f);
    }else if (key == 'p'){
        float posterDistanceFromCamera = poster.getWidth() / tan(ofDegToRad(cam.getFov()));

        ofVec3f posterPosition = cam.getPosition() + cam.getLookAtDir() * posterDistanceFromCamera;
        ofVec3f posterOrientationEuler = cam.getOrientationEuler();
        
        gridAgentSource.setPosition(posterPosition);
        gridAgentSource.setOrientationEuler(posterOrientationEuler);
        poster.setPosition(posterPosition);
        poster.setOrientation(posterOrientationEuler + ofVec3f(180.f, 0, 0));
        gridAgentSource.reset();

        agents.transitionAgents(gridAgentSource, 1.f);
    }else if (key == 'v'){
        agents.animateVisualisations(1.f, 0.f, 1.f);
    }else if (key == 'c'){
        agents.animateVisualisations(1.f, 1.f, 0.f);
    }else if (key == 'i'){
        poster.animate(Animator::Direction::In);
    }else if (key == 'o'){
        poster.animate(Animator::Direction::Out);
    }else if (key == 'u'){
        cam.startRotateUp();
    }
    
    if (key != 't'){
        texts.animateOutIfVisible();
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
