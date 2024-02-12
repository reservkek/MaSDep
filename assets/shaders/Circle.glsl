#shader vertex
#version 450 core

layout(location = 0) in vec4 position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

void main()
{
    gl_Position = u_ViewProjection * u_Model * position;
};

#shader fragment
#version 330 core

#define AA 1
#define AA_SAMPLES 4
#define AA_WIDTH 0.004

// Circle parameters
#define CIRCLE_INNER_RADIUS_SQUARED 0.58
#define CIRCLE_OUTER_RADIUS_SQUARED 0.6
#define CIRCLE_COLOR vec3(1.0)
#define CIRCLE_CENTER vec2(0)
#define SEGMENTS 6

#define CLOCKWISE_HIDING 1
#if CLOCKWISE_HIDING
	#define CIRCLE_DIR 0.0
#else
	#define CIRCLE_DIR 1.0
#endif

// Mathematical constants
#define PI_2 6.283185

uniform vec4 u_Color;

layout(location = 0) out vec4 color;

// Shader constants
const float SEGMENT_ANGLE = PI_2 / float(1 << SEGMENTS);
const float SAMPLE_INCREMENT = AA_WIDTH / max(float(AA_SAMPLES - 1), 1.0);
const float INIT_OFFSET = -AA_WIDTH / 2.0;

// Returns angle of point relative to circle
float theta(vec2 point)
{
    float angle = atan(point.y, point.x);
    angle += PI_2 * float(angle < 0.0);
    return angle;
}

// Returns whether or not a pixel should get drawn
bool hide_pixel(vec2 dir)
{
    // use square distance to avoid taking the square root
    float square_dist = dot(dir, dir);
    // use 'or' operation to increase chances of avoiding the last check
    return square_dist > CIRCLE_OUTER_RADIUS_SQUARED || 
           square_dist < CIRCLE_INNER_RADIUS_SQUARED ||
           mod(floor(theta(dir) / SEGMENT_ANGLE), 2.0) != CIRCLE_DIR;
}


void main()
{
    // clamp coordinates from -1 to 1, and account for aspect-ratio
    vec2 coord = (2.0 * fragCoord - iResolution.xy) / iResolution.y;
    
    float color_val
#if AA
 	= 0.0;
	float offset_y = INIT_OFFSET;
    for (int i = 0; i < AA_SAMPLES; ++i)
    {
        float offset_x = INIT_OFFSET;
        for (int j = 0; j < AA_SAMPLES; ++j)
        {
            color_val += float(hide_pixel(coord + vec2(offset_x, offset_y)));
            offset_x += SAMPLE_INCREMENT;
        }
        
        offset_y += SAMPLE_INCREMENT;
    }
    
    color_val /= float(AA_SAMPLES * AA_SAMPLES);
#else
    = float(hide_pixel(coord));
#endif
    
    color = vec4(mix(CIRCLE_COLOR, BG_COLOR, color_val), 1.0);
}