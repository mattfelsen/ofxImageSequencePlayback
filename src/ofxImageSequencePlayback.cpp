/**
 *  ofxImageSequencePlayback.cpp
 *
 * Created by Jamie Kosoy, http://jkosoy.com
 * Special thanks to Red Paper Heart. http://redpaperheart.com
 * Heavily inspired by TextureSequence by Red Paper Heart. https://github.com/redpaperheart/rph-TextureSequence
 *
 *
 * Dependant on ofxImageSequence library
 * Created by James George, http://www.jamesgeorge.org
 * in collaboration with FlightPhase http://www.flightphase.com
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * ----------------------
 *
 *  ofxImageSequencePlayback is a class to allow one to easily control an ofxImageSequence.
 *  Features include:
 *      Looping a sequence infinitely.
 *      Ping Ponging (play forward until the last frame, then play backward)
 *      Play sequences forward or backward.
 *      An isComplete() check to detect when your sequence has finished playing.
 Caveat :: This will always be false if ping ponging or looping.
 
 *  This class makes use of it's own timing mechanism, leveraging ofxImageSequence for loading and managing frame indexes only.
 *
 */


#include "ofxImageSequencePlayback.h"

template<typename SequenceType>
ofxImageSequencePlayback_<SequenceType>::ofxImageSequencePlayback_() {
    bPlaying = false;
    bPaused = false;
    bReversed = false;
    bPingPong = false;
    bComplete = false;
    bPingPongForwardComplete = false;
    
    mFrameIncrement = 1;
}

template<typename SequenceType>
ofxImageSequencePlayback_<SequenceType>::~ofxImageSequencePlayback_() { mSequence.unloadSequence(); }

template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::setSize(float width, float height) { mSequence.setSize(width, height); }

template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::setImageType(ofImageType imageType) { mSequence.setImageType(imageType); }

// loading
template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::setSequence(const SequenceType &sequence) { mSequence = sequence; newSequenceSetup(); }
template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::setSequence(const SequenceType &sequence,float fps) { setSequence(sequence); setFPS(fps); }
template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::loadSequence(string pathToDir) { mSequence.loadSequence(pathToDir); newSequenceSetup();  }
template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::loadSequence(string prefix, string filetype, int startIndex, int endIndex) { mSequence.loadSequence(prefix, filetype, startIndex, endIndex); newSequenceSetup();  }
template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::loadSequence(string prefix, string filetype, int startIndex, int endIndex, int numDigits) { mSequence.loadSequence(prefix, filetype, startIndex, endIndex, numDigits); newSequenceSetup();  }
template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::loadSequence(string pathToDir,float fps) { loadSequence(pathToDir); setFPS(fps); }
template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::loadSequence(string prefix, string filetype, int startIndex, int endIndex,float fps) { loadSequence(prefix,filetype,startIndex,endIndex); setFPS(fps); }
template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::loadSequence(string prefix, string filetype, int startIndex, int endIndex, int numDigits,float fps) { loadSequence(prefix, filetype, startIndex, endIndex, numDigits); setFPS(fps); }


template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::newSequenceSetup() {
    mLastUpdateTime = ofGetElapsedTimef();
}

// playhead
template<typename SequenceType>
int ofxImageSequencePlayback_<SequenceType>::getCurrentFrameIndex() { return mSequence.getCurrentFrame(); }
template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::setCurrentFrameIndex(int index) { mSequence.setFrame(index); }
template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::setCurrentFramePercentage(float p) { mSequence.setFrameAtPercent(p); }
template<typename SequenceType>
int ofxImageSequencePlayback_<SequenceType>::getTotalFrames() { return mSequence.getTotalFrames(); }

// playing
template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::play() {
    bPlaying = true;
    bPaused = false;
    bComplete = false;
    bPingPongForwardComplete = false;
    playTime = ofGetElapsedTimef();
}

template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::playInReverse() {
    reverse();
    play();
}

template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::playForward() {
	unreverse();
	play();
}

template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::pause() {
    bPlaying = false;
    bPaused = true;
}

template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::stop() {
    bPlaying = false;
    bPaused = false;
    bPingPongForwardComplete = false;
    bComplete = false;

    if(bReversed) {
        setCurrentFrameIndex(getTotalFrames()-1);
    }
    else {
        setCurrentFrameIndex(0);
    }
}

template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::reverse() {
    bPingPong = false;
    bReversed = true;
}

template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::unreverse() {
    bPingPong = false;
    bReversed = false;
}

template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::setShouldPingPong(bool shouldPingPong) {
    bPingPong = shouldPingPong;

    if(bPingPong) {
        bComplete = false;
    }
}

template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::setShouldLoop(bool shouldLoop) {
    bLooping = shouldLoop;

    if(bLooping) {
        bComplete = false;
    }
}


// update and draw
template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::update() {

	if(!mSequence.isLoaded()) return;

	float currentTime = ofGetElapsedTimef();
	

    if(mFPS && (currentTime-mLastUpdateTime) < 1.0f/mFPS) return;
    
    if(!bPaused && bPlaying) {
        int newFrameIndex;
        int currentFrameIndex = getCurrentFrameIndex();
        int totalFrames = getTotalFrames();
        
        if(bPingPong) {
            newFrameIndex = currentFrameIndex + (bPingPongForwardComplete ? -mFrameIncrement : mFrameIncrement);
        }
        else {
//            newFrameIndex = currentFrameIndex + (bReversed ? -mFrameIncrement : mFrameIncrement);
            float elapsed = ofGetElapsedTimef() - playTime;
            float duration = getTotalFrames() / mFPS;
            float percent = fmod(elapsed, duration) / duration;
            newFrameIndex = percent * totalFrames;
        }
        
        // we're playing of forwards if the new frame > total frames.
        if(newFrameIndex > totalFrames-1) {
            if(bPingPong) {
                bPingPongForwardComplete = !bPingPongForwardComplete;
                newFrameIndex = totalFrames-1;
            }
            else {
                if(bLooping) {
                    newFrameIndex = newFrameIndex - totalFrames;
					dispatchLoopedNotification();
                }
                else {
                    bComplete = true;
                    dispatchCompleteNotification();
                }
            }
        }
        // we're playing backwards, if the new frame < 0.
        else if(newFrameIndex <= 0) {
            if(bPingPong) {
                bPingPongForwardComplete = !bPingPongForwardComplete;
                newFrameIndex = 0;
            }
            else {
                if(bLooping) {
                    newFrameIndex = totalFrames - abs(newFrameIndex);
					dispatchLoopedNotification();
                }
                else {
                    bComplete = true;
                    dispatchCompleteNotification();
                }
            }
        }
        
        if(!bComplete) mSequence.setFrame(newFrameIndex);
    }
    
    mLastUpdateTime = ofGetElapsedTimef();
}

template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::dispatchCompleteNotification() {
    static ofEventArgs args;
    ofNotifyEvent(sequenceCompleted, args, this);
}

template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::dispatchLoopedNotification(){
    static ofEventArgs args;
    ofNotifyEvent(sequenceLooped, args, this);
}

template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::draw() {
	draw(0,0);
}

template<typename SequenceType>
void ofxImageSequencePlayback_<SequenceType>::draw(int x, int y) {
	if(!mSequence.isLoaded()) return;

	mSequence.getTextureForFrame(getCurrentFrameIndex()).draw(x,y);
}

template<typename SequenceType>
ofTexture& ofxImageSequencePlayback_<SequenceType>::getTextureReference(){
	return mSequence.getTextureForFrame(getCurrentFrameIndex());
}

template<typename SequenceType>
SequenceType& ofxImageSequencePlayback_<SequenceType>::getSequence(){
	return mSequence;
}

template class ofxImageSequencePlayback_<ofxImageSequence>;
template class ofxImageSequencePlayback_<ofxShortImageSequence>;
template class ofxImageSequencePlayback_<ofxFloatImageSequence>;