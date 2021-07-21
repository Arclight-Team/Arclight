#pragma once

#include "util/vector.h"



class WorldTransform {

public:

    constexpr WorldTransform() noexcept = default;
    constexpr explicit WorldTransform(const Vec3x& translation, const Vec3x& rotation = Vec3x()) noexcept : translation(translation), rotation(rotation) {}

    constexpr Vec3x getTranslation() const noexcept {
        return translation;
    }

    constexpr Vec3x getRotation() const noexcept {
        return rotation;
    }

    constexpr void setTransform(const Vec3x& translation, const Vec3x& rotation) noexcept {
        setTranslation(translation);
        setRotation(rotation);
    }

    constexpr void setTranslation(const Vec3x& translation) noexcept {
        this->translation = translation;
    }

    constexpr void setRotation(const Vec3x& rotation) noexcept {
        this->rotation = rotation;
    }

private:

    Vec3x translation;
    Vec3x rotation;

};
