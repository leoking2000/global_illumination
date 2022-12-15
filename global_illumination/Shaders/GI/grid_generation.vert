#version 330 core
layout(location = 0) in vec2 apos;

uniform mat4 u_proj_view_matrix;
uniform ivec3 u_size;
uniform vec3 uniform_bbox_min;
uniform vec3 uniform_bbox_max;

out ivec3 grid_position;

void main()
{
    vec3 grid_pos;
	grid_pos.z = gl_InstanceID / ( u_size.x * u_size.y );
	grid_pos.y = ( gl_InstanceID / u_size.x ) % u_size.y;
	grid_pos.x = gl_InstanceID % u_size.x;

    //ok = (!checkCRCValidityGeo(grid_position)) ? 1 : 0;

    vec3 stratum = (uniform_bbox_max - uniform_bbox_min) / vec3(u_size);

    vec3 pos_wcs = vec3(apos, 1.0);
    pos_wcs += uniform_bbox_min + (grid_pos + 0.5) * stratum;

    gl_Position = vec4(apos, 0.0, 1.0);
}