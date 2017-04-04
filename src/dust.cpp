#include <ai.h>
#include <string>
#include <cstring>

AI_SHADER_NODE_EXPORT_METHODS(dustMethods);


struct ShaderData {
    AtSampler* sampler;
    std::string trace_set;
};


// sin approximation, not completely accurate but faster than std::sin
inline float fastSin(float x){
    x = fmod(x + AI_PI, AI_PI * 2) - AI_PI; // restrict x so that -AI_PI < x < AI_PI
    const float B = 4.0f / AI_PI;
    const float C = -4.0f / (AI_PI*AI_PI);
    float y = B * x + C * x * std::abs(x);
    const float P = 0.225f;
    return P * (y * std::abs(y) - y) + y;
}


inline float fastCos(float x){
    // conversion from sin to cos
    x += AI_PI * 0.5;

    x = fmod(x + AI_PI, AI_PI * 2) - AI_PI; // restrict x so that -AI_PI < x < AI_PI
    const float B = 4.0f / AI_PI;
    const float C = -4.0f / (AI_PI*AI_PI);
    float y = B * x + C * x * std::abs(x);
    const float P = 0.225f;
    return P * (y * std::abs(y) - y) + y;
}


// Improved concentric mapping code by Dave Cline [peter shirley's blog]
// maps points on the unit square onto the unit disk uniformly
inline void concentricDiskSample(float ox, float oy, AtPoint2 *lens) {
    float phi, r;

    // switch coordinate space from [0, 1] to [-1, 1]
    float a = 2.0 * ox - 1.0;
    float b = 2.0 * oy - 1.0;

    if ((a * a) > (b * b)){
        r = a;
        phi = (0.78539816339f) * (b / a);
    } else {
        r = b;
        phi = (AI_PIOVER2)-(0.78539816339f) * (a / b);
    }

    *lens = { r * fastCos(phi), r * fastSin(phi) };
}


enum DustParams {
    p_color,
    p_radius,
    p_spread,
    p_traceSet,
    p_samples
};


node_parameters {
    AiParameterRGB("color", 1.0, 1.0, 1.0);
    AiParameterFLT("radius", 20.0);
    AiParameterFLT("spread", 0.0);
    AiParameterSTR("traceSet", "");
    AiParameterINT("samples", 4);
}


node_initialize {
    ShaderData* data = new ShaderData;
    AiNodeSetLocalData(node, data);
    data->sampler = NULL;
}


node_update {
    ShaderData *data = (ShaderData*)AiNodeGetLocalData(node);
    AtNode *options = AiUniverseGetOptions();

    data->sampler = AiSamplerSeeded(1, AiNodeGetInt(node, "samples"), 2);

    data->trace_set = std::string();
    data->trace_set = params[p_traceSet].STR;
}


node_finish {
    if (AiNodeGetLocalData(node)){
        ShaderData* data = (ShaderData*)AiNodeGetLocalData(node);
        AiSamplerDestroy(data->sampler);
        AiNodeSetLocalData(node, NULL);
        delete data;
    }
}


shader_evaluate {
    ShaderData* data = (ShaderData*)AiNodeGetLocalData(node);

    AtRGB color = AiShaderEvalParamRGB(p_color);
    float radius = AiShaderEvalParamFlt(p_radius);
    float spreadInv = (1.0 - AiShaderEvalParamFlt(p_spread)) * 100.0;

    AtRGB result = AI_RGB_BLACK;

    AtPoint2 unitDiskCoords = {0.0, 0.0};
    AtRay ray;
    AtVector yUp = {0.0, 1.0, 0.0};
    AtVector randomConeVector = {0.0, 1.0, 0.0};

    if (data->trace_set.length()){
        AiShaderGlobalsSetTraceSet(sg, data->trace_set.c_str(), true);
    }
    
    float samples[2];
    AtSamplerIterator* sampit = AiSamplerIterator(data->sampler, sg);
    AiMakeRay(&ray, AI_RAY_DIFFUSE, &sg->P, NULL, radius, sg);
    
    while (AiSamplerGetSample(sampit, samples)){
        concentricDiskSample(samples[0], samples[1], &unitDiskCoords);
        randomConeVector.x = unitDiskCoords.x / spreadInv;
        randomConeVector.z = unitDiskCoords.y / spreadInv;

        ray.dir = randomConeVector;

        AiTraceProbe(&ray, NULL) ? result -= AI_RGB_BLACK : result += AI_RGB_WHITE;
    }

    AiShaderGlobalsUnsetTraceSet(sg);

    sg->out.RGB = (result * AiSamplerGetSampleInvCount(sampit)) * color;
}



node_loader {
    if (i > 0){return false;}
    node->methods     = dustMethods;
    node->output_type = AI_TYPE_RGB;
    node->name        = "dust";
    node->node_type   = AI_NODE_SHADER;
    strcpy(node->version, AI_VERSION);
    return true;
}