#include <ai.h>
#include <string>
#include <cstring>
#include <vector>

// TODO
// add max angle the dust is able to fall on
// take radius into account, if an object far away gets hit, it shouldn't have the same influence as nearby objects


struct ShaderData
{
    AtSampler* sampler;
    int sampleCount;
};


AI_SHADER_NODE_EXPORT_METHODS(dustMethods);


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
    }
    else {
        r = b;
        phi = (AI_PIOVER2)-(0.78539816339f) * (a / b);
    }

    *lens = { r * fastCos(phi), r * fastSin(phi) };
}


enum DustParams 
{

};


node_parameters
{

}


node_initialize
{
    ShaderData* data = new ShaderData;
    AiNodeSetLocalData(node, data);
    data->sampler = NULL;
}


node_update
{
    ShaderData *data = (ShaderData*)AiNodeGetLocalData(node);
    AtNode *options = AiUniverseGetOptions();
    //data->sampleCount = AiNodeGetInt(options, "sampleCount");
    data->sampleCount = 6;
    data->sampler = AiSamplerSeeded(1, data->sampleCount, 2);
}


node_finish
{
    if (AiNodeGetLocalData(node))
    {
        ShaderData* data = (ShaderData*)AiNodeGetLocalData(node);
        AiSamplerDestroy(data->sampler);
        AiNodeSetLocalData(node, NULL);
        delete data;
    }
}


shader_evaluate
{
    ShaderData* data = (ShaderData*)AiNodeGetLocalData(node);


    AtRGB result = AI_RGB_BLACK;
    AtPoint2 unitDiskCoords = {0.0, 0.0};
    float samples[2];
    AtRay ray;
    AtVector yUp = {0.0, 1.0, 0.0};
    AtVector randomConeVector = {0.0, 1.0, 0.0};


    AtSamplerIterator* sampit = AiSamplerIterator(data->sampler, sg);
    AiMakeRay(&ray, AI_RAY_DIFFUSE, &sg->P, NULL, AI_BIG, sg);


    while (AiSamplerGetSample(sampit, samples)){
        concentricDiskSample(samples[0], samples[1], &unitDiskCoords);
        randomConeVector.x = unitDiskCoords.x / 4.0;
        randomConeVector.z = unitDiskCoords.y / 4.0;

        ray.dir = randomConeVector;

        if (!AiTraceProbe(&ray, NULL)){
            float nDotY = AiV3Dot(sg->N, yUp);
            result += {nDotY, nDotY, nDotY};
        } else {
            result += AI_RGB_BLACK;
        }
    }

    result *= AiSamplerGetSampleInvCount(sampit);
    
    sg->out.RGB = result;

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