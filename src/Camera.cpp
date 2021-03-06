#include "Camera.hpp"

Camera::Camera(Vector3f position,
               Vector3f origin,
               Vector2f tileSize,
               float gridSize,
               float windowWidth,
               float windowHeight) : position_(position),
                                     origin_(origin),
                                     tileSize_(tileSize),
                                     gridSize_(gridSize),
                                     groundNormal_(0, 0, 1),
                                     cameraDirection_(0, 0, 1),
                                     windowView_(),
                                     windowWidth_(windowWidth),
                                     windowHeight_(windowHeight),
                                     zoomFactor_(1),
                                     maxZoom_(1.5),
                                     minZoom_(0.5),
                                     rotation_(0)
{
    float w_h = tileSize_.x / 2.0;
    float h_h = tileSize_.y / 2.0;
    float l = w_h / cos((45.0 / 180.0) * M_PI);
    Vector3f s = Vector3f(gridSize_, gridSize_, gridSize_);

    transformMatrix_ = Matrix4(
        w_h / s.x, -1 * w_h / s.y, 0, 0,
        h_h / s.x, h_h / s.y, -1 * l / s.z, 0,
        w_h * tan((45.0 / 180.0) * M_PI) / s.x, w_h * tan((45.0 / 180.0) * M_PI) / s.y, 0, 0,
        0, 0, 0, 1);
    updateTransforms_();

    cameraDirection_ = matMultipy(inverseTransform_, cameraDirection_, 0);
}

const Vector3f &Camera::getPosition() const
{
    return position_;
}

void Camera::setPosition(const Vector3f &position)
{
    position_ = position;
    updateTransforms_();
}

void Camera::setPosition(const float &x, const float &y, const float &z)
{
    position_.x = x;
    position_.y = y;
    position_.z = z;
    updateTransforms_();
}

void Camera::move(const Vector3f &velocity)
{
    position_ += velocity;
    updateTransforms_();
}

void Camera::move(const float &x, const float &y, const float &z)
{
    position_.x += x;
    position_.y += y;
    position_.z += z;
    updateTransforms_();
}

void Camera::pan(const Vector2f &direction)
{
    pan(direction.x, direction.y);
}

void Camera::pan(const float &x, const float &y)
{
    Vector3f screenPosition = transform(position_);
    screenPosition.x += x * zoomFactor_;
    screenPosition.y += y * zoomFactor_;
    setPosition(projectGround(screenPosition));
}

void Camera::setZoom(const float &z)
{
    if (z > maxZoom_)
    {
        zoomFactor_ = maxZoom_;
        return;
    }
    if (z < minZoom_)
    {
        zoomFactor_ = minZoom_;
        return;
    }
    zoomFactor_ = z;
}

void Camera::setZoomRange(const float &min, const float &max)

{
    minZoom_ = min;
    maxZoom_ = max;
}

void Camera::updateWindow(sf::RenderWindow &window)
{
    windowView_.setCenter(windowWidth_ / 2.f, windowHeight_ / 2.f);
    windowView_.setSize(windowWidth_, windowHeight_);
    windowView_.zoom(zoomFactor_);
    windowView_.setRotation(rotation_);
    window.setView(windowView_);
}

Vector3f Camera::transform(const Vector3f &point) const
{
    return matMultipy(transformMatrix_, point);
}

Vector3f Camera::itransform(const Vector3f &point) const
{
    return matMultipy(inverseTransform_, point);
}

Vector3f Camera::transform(const Vector3f &point, const float &w) const
{
    return matMultipy(transformMatrix_, point, w);
}

Vector2f Camera::transform2(const Vector3f &point, const float &w) const
{
    Vector3f p = transform(point, w);
    return Vector2f(p.x, p.y);
}

Vector2f Camera::transform2(const Vector3f &point) const
{
    Vector3f p = transform(point);
    return Vector2f(p.x, p.y);
}

Vector3f Camera::itransform(const Vector3f &point, const float &w) const
{
    return matMultipy(inverseTransform_, point, w);
}

Vector3f Camera::projectGround(const Vector3f &point) const
{
    float groundElevation = 0;

    Vector3f intPoint;
    bool intersected = intersetPlane(
        groundNormal_,
        Vector3f(0, 0, groundElevation),
        itransform(point),
        cameraDirection_,
        &intPoint);

    return intPoint;
}

Vector3f Camera::projectGround(const Vector2f &point) const
{
    return projectGround(
        Vector3f(point.x, point.y, 0));
}

Vector3f Camera::projectGround(const Vector2i &point) const
{
    return projectGround(
        Vector3f((float)point.x, (float)point.y, 0));
}

void Camera::updateTransforms_()
{
    translation_ = origin_ - matMultipy(transformMatrix_, position_, 0);

    transformMatrix_[0][3] = translation_.x;
    transformMatrix_[1][3] = translation_.y;
    transformMatrix_[2][3] = translation_.z;

    transformMatrix_.getInverse(inverseTransform_);
}
