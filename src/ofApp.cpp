#include "ofApp.h"
ofTrueTypeFont font;

//--------------------------------------------------------------
void ofApp::setup(){
    unique_ptr<AgentSource> agentSource = make_unique<MeshRovingAgentSource>();
    unique_ptr<VisualisationSource> visualisationSource = make_unique<TornPaperVisualisationSource>();
    agentSource->setup();
    visualisationSource->setup();
    int maxAgents = 1000;

    agents.setup(move(agentSource), move(visualisationSource), maxAgents);
    music.setup("ArTeaser_Edit04.wav");
    ofBackground(255);
    
    blur = make_unique<RandomBlur>();
    blur->setup(ofGetWidth(), ofGetHeight());

    font.load("Ubuntu-R.ttf", 600, true, false, true);
}

//--------------------------------------------------------------
void ofApp::update(){
    music.update();
    agents.update(music.getLevel());
}

//--------------------------------------------------------------
void ofApp::draw(){
    blur->begin();

    cam.begin();
    agents.draw();
    ofPushStyle();
    ofSetColor(126, 46, 23);
    font.drawString("ARLEQUINO", 0.f, 0.f);
    ofPopStyle();
    cam.end();
    blur->end();
    
    blur->draw(0.f, 0.f);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == '1'){
    } else if (key == '2') {
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
