#pragma once

#include "image/pixel.h"
#include "image/image.h"
#include "util/log.h"
#include "math/math.h"

#include <vector>
#include <stdexcept>


class FrameNotFoundException : public std::runtime_error {

public:
	FrameNotFoundException(const std::string& msg) : std::runtime_error(msg) {}

};



template<Pixel P = Pixel::RGB8>
class Video {

public:

    struct Frame {

        double timestamp;
        Image<P> image;

        constexpr Frame() : timestamp(0) {}

        constexpr Frame(Image<P> image, u32 timestamp) : image(image), timestamp(timestamp) {}

        constexpr auto operator<=>(const Frame& frame) const {
            return timestamp <=> frame.timestamp;
        }

    };

    constexpr Video() : currentFrame(0), looping(false), reverse(false), speed(1.0), time(0.0) {}

    constexpr Video(Image<P> startFrame) : currentFrame(0), looping(false), reverse(false), speed(1.0), time(0.0) {
        addFrame(startFrame, 0);
    }

    constexpr void addFrame(Image<P> image, double time) {

        Frame frame(image, time);
        frames.insert(std::upper_bound(frames.begin(), frames.end(), frame), frame);

    }

    constexpr const Frame& getFrame(u32 frameID) const {
        
        if(frameID >= frames.size()) {
            throw FrameNotFoundException("Frame " + std::to_string(frameID) + " does not exist");
        }

        return frames[frameID];

    }

    constexpr Frame& getFrame(u32 frameID) {
        
        if(frameID >= frames.size()) {
            throw FrameNotFoundException("Frame " + std::to_string(frameID) + " does not exist");
        }

        return frames[frameID];

    }

    constexpr const Frame& getCurrentFrame() const {
        return getFrame(currentFrame);
    }

    constexpr Frame& getCurrentFrame() {
        return getFrame(currentFrame);
    }

    constexpr const Frame& operator[](SizeT i) const {
        return getFrame(i);
    }

    constexpr Frame& operator[](SizeT i) {
        return getFrame(i);
    }

    constexpr void deleteFrame(u32 frameID) {

        if(frameID >= frames.size()) {
            Log::error("Video", "Failed to delete frame at position %d", frameID);
            return;
        }

        frames.erase(frames.begin() + frameID);

    }

    constexpr void clear() {

        frames.clear();
        restart();

    }

    constexpr void restart() {

        time = getFirstFrame().timestamp;
        currentFrame = getFirstFrameID();

    }

    constexpr void setCurrentFrame(u32 frame) {

        if(frame >= frames.size()) {
            currentFrame = frame ? getFrameCount() - 1 : 0;
        } else {
            currentFrame = frame;
        }

        time = getCurrentFrame().timestamp;

    }

    constexpr void setReversed(bool reversed) {
        reverse = reversed;
    }

    constexpr bool isReversed() const {
        return reverse;
    }

    constexpr void setLooping(bool loop) {
        looping = loop;
    }

    constexpr bool isLooping() const {
        return looping;
    }

    constexpr u32 getFrameCount() const {
        return frames.size();
    }

    constexpr u32 getFirstFrameID() const {
        return reverse ? Math::min(getFrameCount(), 1) - 1 : 0;
    }

    constexpr u32 getLastFrameID() const {
        return reverse ? 0 : Math::min(getFrameCount(), 1) - 1;
    }

    constexpr u32 getCurrentFrameID() const {
        return currentFrame;
    }

    constexpr const Frame& getFirstFrame() const {
        return getFrame(getFirstFrameID());
    }

    constexpr Frame& getFirstFrame() {
        return getFrame(getFirstFrameID());
    }

    constexpr const Frame& getLastFrame() const {
        return getFrame(getLastFrameID());
    }

    constexpr Frame& getLastFrame() {
        return getFrame(getLastFrameID());
    }

    constexpr void setSpeed(double speed) {
        this->speed = speed;
    }

    constexpr double getSpeed() const {
        return speed;
    }

    constexpr void step(double dt) {

        dt *= speed;

        if(reverse) {

            time -= dt;

            if(time < 0) {
                time = 0;
            }

            currentFrame = std::upper_bound(frames.begin(), frames.end(), time, [](double t, const Frame& frame) {
                return t < frame.timestamp;
            });

        } else {

            time += dt;

            if(time > getLastFrame().timestamp) {
                time = getLastFrame().timestamp;
            }

            currentFrame = std::lower_bound(frames.begin(), frames.end(), time, [](double t, const Frame& frame) {
                return t < frame.timestamp;
            });

        }

    }

private:
    
    std::vector<Frame> frames;
    u32 currentFrame;

    double speed;
    double time;
    bool reverse;
    bool looping;

};