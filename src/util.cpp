//#include <algorithm>
//#include <cmath>
//#include "vec2.cpp"
//using namespace std;
//
//float SmoothingKernelDerivative(float dst, float radius) {
//    if (dst >= radius) return 0;
//
//    float f = radius * radius - dst * dst;
//    float scale = -24.0f / (M_PI * pow(radius, 8));
//    return scale * dst * f * f;
//}
//
//float SmoothingKernel(float dst, float radius) {
//    float volume = M_PI * pow(radius, 8) / 4;
//    float value = max(0.0f, radius * radius - dst * dst);
//    return value * value * value / volume;
//}
//
//vec2 CalculatePropertyGradient(vec2 SamplePoint) {
//    vec2 propertyGradient = vec2();
//
//    for (int i = 0; i < numParticles; i++) {
//        float dst = (positions[i] - samplePoint)
//    }
//}
//
