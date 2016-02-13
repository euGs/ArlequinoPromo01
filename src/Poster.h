#pragma once

class Poster {
public:
    void setup(string imagePath, ofVec3f position, ofVec3f orientationEuler){
        texture.load(imagePath);

        plane.set(texture.getWidth(), texture.getHeight());
        plane.mapTexCoords(0, 0, texture.getWidth(), texture.getHeight());
        plane.setPosition(position);
        plane.setOrientation(orientationEuler);
        animator.setup(0.f, FinalAlpha, AnimationTime);
    }
    
    void draw(){
        ofPushStyle();
        ofSetColor(255, 255, 255, animator.getValue());
        plane.setPosition({0, static_cast<float>(ofGetMouseY()), static_cast<float>(ofGetMouseX())});
        texture.getTexture().bind();
        plane.draw();
        texture.getTexture().unbind();
        ofPopStyle();
    }
    
    void animateIn(){
        animator.animate(Animator::Direction::In);
    }
    
    ofVec3f getPosition(){
        return plane.getPosition();
    }
    
protected:
    const float FinalAlpha = 255.f;
    const float AnimationTime = .8f;
    
    ofTrueTypeFont font;
    ofImage texture;
    ofPlanePrimitive plane;
    Animator animator;
};