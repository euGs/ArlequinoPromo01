#pragma once

#include "ofMain.h"
#include "AgentSource.h"
#include "VisualisationSource.h"
#include "Agent.h"
#include "Visualisation.h"

// Handles setting up agents (with their visualisations), generating noise and scaling
// values for agents in the update loop and transitioning all agents from one type to another.
class Agents {
public:
    void setup(AgentSource &agentSource, VisualisationSource &visualisationSource, int maxAgents){
        isTransitioning = false;
        
        while (visualisationSource.hasMoreVisualisations() && agents.size() < maxAgents){
            unique_ptr<Agent> agent = move(agentSource.getAgent());

            agent->setVisualisation(move(visualisationSource.getVisualisation()));
            agent->setup();
            agents.push_back(move(agent));
        }
    }

    void update(float scalingFactor){
        // Generate noise values for move data.
        float noiseScale = .5f;//ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, 1.f);
        float noiseVel = ofGetElapsedTimef();

        for (int i=0; i<agents.size(); i++){
            MoveData md;
            
            md.normalisedValue1 = ofNoise(i * noiseScale, 1 * noiseScale, noiseVel);
            md.normalisedValue2 = ofNoise(i * noiseScale, 1000 * noiseScale, noiseVel);
            md.globalScaling = .05f + scalingFactor;
            agents[i]->update(md);
        }

        if (isTransitioning){
            // Calculate lerp value for LerpingAgent - as normalised time from start (zerp) to end (one)
            // of the transition time.
            float normalisedTime = (ofGetElapsedTimef() - startTransitionTime) / (endTransitionTime - startTransitionTime);
            
            for (int i=0; i<lerpingAgents.size(); i++){
                MoveData md;
                md.normalisedValue1 = normalisedTime;
                // End position could be constantly moving so keep updating the lerping agent here.
                lerpingAgents[i].setEndPosition(agents[i]->getPosition());
                lerpingAgents[i].update(md);
            }
            
            if (ofGetElapsedTimef() > endTransitionTime){
                isTransitioning = false;

                for (size_t i = 0; i < agents.size(); i++){
                    agents[i]->setVisualisation(lerpingAgents[i].getVisualisation());
                }
            }
        }
        
        if (isAnimatingVisualisation){
            float animationNormalisedTime = (ofGetElapsedTimef() - startVisualisationTime)
            / (endVisualisationTime - startVisualisationTime);
            float animationPosition = ofMap(ofGetElapsedTimef(), startVisualisationTime, endVisualisationTime, this->fromAnimationPosition, this->toAnimationPosition);
            
            for (auto i=agents.begin(); i!=agents.end(); i++){
                (*i)->bringVisualisationHome(animationPosition);
            }
            
            if (ofGetElapsedTimef() > endVisualisationTime){
                isAnimatingVisualisation = false;
            }
        }
    }
    
    void transitionAgents(AgentSource &agentSource, float durationSeconds){
        if (isTransitioning){
            return;
        }
        
        isTransitioning = true;

        lerpingAgents.clear();
        
        for (size_t i = 0; i < agents.size(); i++){
            LerpingAgent lerpingAgent;
            lerpingAgent.setStartPosition(agents[i]->getPosition());
            unique_ptr<Agent> newAgent = move(agentSource.getAgent());
            newAgent->setup();
            lerpingAgent.setVisualisation(agents[i]->getVisualisation());
            lerpingAgents.push_back(move(lerpingAgent));
            agents[i] = move(newAgent);
        }
        
        startTransitionTime = ofGetElapsedTimef();
        endTransitionTime = startTransitionTime + durationSeconds;
    }
    
    bool getIsTransitioning(){
        return isTransitioning;
    }
    
    void animateVisualisations(float durationSeconds, float fromAnimationPosition, float toAnimationPosition){
        startVisualisationTime = ofGetElapsedTimef();
        endVisualisationTime = startVisualisationTime + durationSeconds;
        isAnimatingVisualisation = true;
        this->fromAnimationPosition = fromAnimationPosition;
        this->toAnimationPosition = toAnimationPosition;
    }
    
    void draw(){
        if (!isTransitioning){
            for (int i=0; i<agents.size(); i++){
                agents[i]->draw();
            }
        }else{
            for (int i=0; i<lerpingAgents.size(); i++){
                lerpingAgents[i].draw();
            }
        }
    }
    
    void drawUntextured(int increment){
        if (!isTransitioning){
            for (int i=0; i<agents.size(); i+=increment){
                agents[i]->drawUntextured();
            }
        }else{
            for (int i=0; i<lerpingAgents.size(); i+=increment){
                lerpingAgents[i].drawUntextured();
            }
        }
    }
    
protected:
    vector< unique_ptr<Agent> > agents;
    vector< LerpingAgent > lerpingAgents;
    bool isTransitioning;
    bool isAnimatingVisualisation;
    float fromAnimationPosition, toAnimationPosition;
    float startTransitionTime, endTransitionTime;
    float startVisualisationTime, endVisualisationTime;
};