#version 450 core
in vec2 texCoord;
uniform float StepSize = 0.0025;
uniform float BaseDistance = 0.0025;
uniform float ambient = 0.5;
uniform float diffuse = 0.5;
uniform float specular = 0.5;
uniform float shininess = 32.0;
uniform float isovalue = 0.5;
uniform bool enableShading = true;

layout (binding = 0) uniform sampler2D frontFaceTexMap;
layout (binding = 1) uniform sampler2D backFaceTexMap;
layout (binding = 2) uniform sampler3D volumeData;
layout (binding = 3) uniform sampler1D transferFunction;

layout (location = 0) out vec4 fColor;

void main()
{
    vec3 I_ambient = vec3(ambient);
    vec3 I_diffuse = vec3(0.5);
    vec3 I_specular = vec3(0.5);
    float delta = StepSize / 2.0;

    vec3 startVolumeCoord = texture(frontFaceTexMap, texCoord).rgb;
    vec3 endVolumeCoord = texture(backFaceTexMap, texCoord).rgb;

    vec3 rayDir = normalize(endVolumeCoord - startVolumeCoord);
    vec3 position = startVolumeCoord;
    bool needOpacityCorrection = StepSize != BaseDistance;

    vec4 compositeColor = vec4(0.0);
    int maxMarchingSteps = int(length(endVolumeCoord - startVolumeCoord) / StepSize);

    for (int i=0;i < maxMarchingSteps; i++){
        float scalar = texture(volumeData, position).r;
        if (scalar >= isovalue){
            vec4 src = texture(transferFunction, scalar);// get non-associative color
            float opacity = src.a;
            vec4 newSrc = vec4(src.rgb, 1.0);
            vec4 finalColor;
            if (enableShading){
                vec3 normal;
                normal.x = texture(volumeData, position + vec3(delta, 0.0, 0.0)).r - texture(volumeData, position - vec3(delta, 0.0, 0.0)).r;
                normal.y = texture(volumeData, position + vec3(0.0, delta, 0.0)).r - texture(volumeData, position - vec3(0.0, delta, 0.0)).r;
                normal.z = texture(volumeData, position + vec3(0.0, 0.0, delta)).r - texture(volumeData, position - vec3(0.0, 0.0, delta)).r;
                normal = normalize(normal);

                float dirDotNorm = dot(rayDir, normal);
                vec3 specularColor = vec3(0.0);
                vec3 diffuseColor = vec3(0.0);
                if (dirDotNorm>0.0){
                    diffuseColor = dirDotNorm * I_diffuse;
                    vec3 v = normalize(-position);
                    vec3 r = reflect(-rayDir, normal);
                    float R_dot_V= max(dot(r, v), 0.0);
                    float pf = (R_dot_V == 0.0)? 0.0: pow(R_dot_V, shininess);
                    specularColor = I_specular * pf;
                }
                finalColor = vec4(I_ambient + diffuseColor + specularColor, 1.0) * newSrc;
            } else {
                finalColor = newSrc;
            }
            compositeColor = finalColor;// front-to-back compositing
            break;
        }
        position += rayDir * StepSize;
    }
    fColor = compositeColor;
}
