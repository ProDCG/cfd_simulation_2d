//
// Created by mason on 12/10/2024.
//
class vec2 {
private:
    float x, y;
public:
    vec2():x(0.0f),y(0.0f) {}
    vec2(float _x, float _y):x(_x),y(_y) {}

    vec2 operator-(vec2& other) {
        return {x-other.x, y-other.y};
    }
};