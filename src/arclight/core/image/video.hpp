#pragma once

#include "image/pixel.hpp"
#include "image/image.hpp"
#include "math/math.hpp"
#include "util/log.hpp"

#include <vector>
#include <stdexcept>




template<Pixel P = Pixel::RGB8>
class Video {

public:

    class Frame {

    public:

        constexpr Frame() : timestamp(0) {}
        constexpr Frame(const Image<P>& image, double timestamp) : image(image), timestamp(timestamp) {}

        constexpr auto operator<=>(const Frame& frame) const {
            return timestamp <=> frame.timestamp;
        }

        constexpr auto operator<=>(double timestamp) const {
            return this->timestamp <=> timestamp;
        }

        constexpr double getTimestamp() const {
            return timestamp;
        }

        constexpr const Image<P>& getImage() const {
            return image;
        }

    private:

        double timestamp;
        Image<P> image;

    };

    constexpr Video() : currentFrame(0), looping(false), reverse(false), speed(1.0), time(0.0), width(1), height(1) {}

    constexpr Video(Image<P> startFrame) : currentFrame(0), looping(false), reverse(false), speed(1.0), time(0.0) {
        addFrame(startFrame, 0);
    }

    constexpr void addFrame(const Image<P>& image, double time) {

        if(!getFrameCount()) {

            width = image.getWidth();
            height = image.getHeight();
            frames.emplace(std::upper_bound(frames.begin(), frames.end(), time), image, time);

        } else if (image.getWidth() == width && image.getHeight() == height) {

            frames.emplace(std::upper_bound(frames.begin(), frames.end(), time), image, time);

        } else {

            Image<P> resizedImage = image;
            resizedImage.resize(ImageScaling::Nearest, width, height);
            frames.emplace(std::upper_bound(frames.begin(), frames.end(), time), resizedImage, time);

        }

    }

    constexpr const Frame& getFrame(u32 frameID) const {
        
        if(frameID >= getFrameCount()) {
            return dummyFrame;
        }

        return frames[frameID];

    }

    constexpr const Frame& getCurrentFrame() const {
        return getFrame(currentFrame);
    }

    constexpr const Frame& operator[](SizeT i) const {
        return getFrame(i);
    }

    constexpr void deleteFrame(u32 frameID) {

        if(frameID >= getFrameCount) {
            Log::error("Video", "Failed to delete frame at position %d", frameID);
            return;
        }

        if(getFrameCount == 1) {
            width = 1;
            height = 1;
        }

        frames.erase(frames.begin() + frameID);

    }

    constexpr void clear() {

        width = 1;
        height = 1;
        frames.clear();
        restart();

    }

    constexpr void restart() {

        time = getFirstFrame().getTimestamp();
        currentFrame = getFirstFrameID();

    }

    constexpr void setCurrentFrame(u32 frame) {

        if(frame >= getFrameCount()) {
            currentFrame = frame ? getFrameCount() - 1 : 0;
        } else {
            currentFrame = frame;
        }

        time = getCurrentFrame().getTimestamp();

    }

    constexpr void setFrameImage(u32 frameID, const Image<P>& image) {

        if(frameID >= getFrameCount()) {
            Log::error("Video", "Failed to set frame image at position %d", frameID);
            return;
        }

        frames[frameID] = Frame(image, getFrame(frameID).getTimestamp());

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
        return static_cast<u32>(frames.size());
    }

    constexpr u32 getFirstFrameID() const {
        return reverse ? Math::max(getFrameCount(), 1) - 1 : 0;
    }

    constexpr u32 getLastFrameID() const {
        return reverse ? 0 : Math::max(getFrameCount(), 1) - 1;
    }

    constexpr u32 getCurrentFrameID() const {
        return currentFrame;
    }

    constexpr const Frame& getFirstFrame() const {
        return getFrame(getFirstFrameID());
    }

    constexpr const Frame& getLastFrame() const {
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

                if(looping) {
                    time = Math::mod((-time / getPlaybackTime() + 1) * getPlaybackTime() + time, getPlaybackTime());
                } else {
                    time = 0;
                }

            }

            currentFrame = static_cast<u32>(std::distance(frames.begin(), std::upper_bound(frames.begin(), frames.end(), time)));

        } else {

            time += dt;

            if(time > getLastFrame().getTimestamp()) {

                if(looping) {
                    time = Math::mod(time, getPlaybackTime());
                } else {
                    time = getLastFrame().getTimestamp();
                }

            }

            currentFrame = static_cast<u32>(std::distance(frames.begin(), std::lower_bound(frames.begin(), frames.end(), time)));

        }

    }

    constexpr u32 getWidth() const {
        return width;
    }

    constexpr u32 getHeight() const {
        return height;
    }

    constexpr double getPlaybackTime() const {
        return getFrame(getFrameCount() - 1).getTimestamp();
    }

    constexpr double getTime() const {
        return time;
    }

private:

    constexpr static Frame dummyFrame = Frame(Image<P>(1, 1), 0);
    
    std::vector<Frame> frames;
    u32 currentFrame;
    u32 width;
    u32 height;

    double speed;
    double time;
    bool reverse;
    bool looping;

};