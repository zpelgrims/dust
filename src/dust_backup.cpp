#include <ai.h>
#include <string>
#include <cstring>
#include <vector>


AI_SHADER_NODE_EXPORT_METHODS(dustMethods);


enum dustParams{
    //p_mode,
};


struct ShaderData {
    AtSampler* sampler;
};



node_parameters {
    //AiParameterBOOL("mode", true);
}

node_initialize {
    //bool mode = AiNodeGetBool(node, "mode");
    ShaderData* data = new ShaderData();
}


node_update {
    ShaderData *data = reinterpret_cast<ShaderData*>(AiNodeGetLocalData(node));

    data->sampler = AiSampler(4, 2);
}


node_finish {
}




shader_evaluate{
    ShaderData *data = reinterpret_cast<ShaderData*>(AiNodeGetLocalData(node));
    //bool mode = AiShaderEvalParamBool(p_mode);

    float threshold = 0.85;
    float depthDecay = 5.0;
    float thickness = 1.0;

    float maxAngle = 45.0;


    AtVector yUp = {0.0f, 1.0f, 0.0f};

    AtRay ray;
    AiMakeRay(&ray, AI_RAY_SHADOW, &sg->P, &yUp, AI_BIG, sg);


    float sample[2];
    AtSamplerIterator* iterator = AiSamplerIterator(data->sampler, sg);

    //while (AiSamplerGetSample(iterator, sample)){

    //}


    if (!AiTraceProbe(&ray, NULL)){
        float nDotY = AiV3Dot(sg->N, yUp) * thickness;
        sg->out.RGB = {nDotY, nDotY, nDotY};
    } else {
        sg->out.RGB = AI_RGB_BLACK;
    }

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