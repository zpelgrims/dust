#include <ai.h>
#include <string>
#include <cstring>

AI_SHADER_NODE_EXPORT_METHODS(dustMethods);


struct ShaderData {
    AtSampler* sampler;
    std::string trace_set;
};


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
    AiParameterFLT("spread", 1.0);
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

    const AtRGB color = AiShaderEvalParamRGB(p_color);
    const float radius = AiShaderEvalParamFlt(p_radius);
    const float spread = AiShaderEvalParamFlt(p_spread);
    const AtVector y = {0.0, 1.0, 0.0};
    AtRGB result = AI_RGB_BLACK;

    sg->out.RGB = (AI_RGB_WHITE - AiOcclusion(&y, &y, sg, 0.0, radius, spread, 0.0, data->sampler, NULL)) * color;

    AiShaderGlobalsUnsetTraceSet(sg);
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