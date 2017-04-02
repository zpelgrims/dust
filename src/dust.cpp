#include <ai.h>
#include <string>
#include <cstring>
#include <vector>

// TODO

/* to compile and kick on my machine
g++ -std=c++11 -O3 -I"C:/ilionData/Users/zeno.pelgrims/Desktop/Arnold-4.2.12.0-windows/include" -L"C:/ilionData/Users/zeno.pelgrims/Desktop/Arnold-4.2.12.0-windows/bin" -lai --shared C:/ilionData/Users/zeno.pelgrims/Documents/lightfilter/src/lightColorAttenuation.cpp -o C:/ilionData/Users/zeno.pelgrims/Documents/lightfilter/bin/lightColorAttenuation.dll
C:/ilionData/Users/zeno.pelgrims/Desktop/Arnold-4.2.13.4-windows/bin/kick -l "C:\ilionData\Users\zeno.pelgrims\Documents\lightfilter\bin" -set options.skip_license_check on -i "C:/ilionData/Users/zeno.pelgrims/Documents/lightfilter/lightfilter_own_02.ass" -g 2.2 -v 2 -l "C:/Program Files/Ilion/IlionMayaFramework/2015/modules/mtoa/1.2.7.2.2-4.2.13.6/shaders" -o "C:/ilionData/Users/zeno.pelgrims/Documents/lightfilter/lightColorAttenuation.exr" -dp
*/


AI_SHADER_NODE_EXPORT_METHODS(dustMethods);


enum dustParams{
    //p_mode,
    //p_colorStart,
};



node_parameters {
    //AiParameterBOOL("mode", true);
    //AiParameterRGB("colorStart", 1.0, 1.0, 1.0);
}

node_initialize {
    //bool mode = AiNodeGetBool(node, "mode");
}


node_update {

}


node_finish {

}




shader_evaluate{
    //bool mode = AiShaderEvalParamBool(p_mode);

    float threshold = 0.85;
    float depthDecay = 5.0;
    float thickness = 1.0;

    float maxAngle = 45.0;


    AtVector yUp = {0.0f, 1.0f, 0.0f};

    AtRay ray;
    AiMakeRay(&ray, AI_RAY_SHADOW, &sg->P, &yUp, AI_BIG, sg);

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