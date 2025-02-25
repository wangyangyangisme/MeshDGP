//
// Vertex shader for modifying image contrast by
// interpolation and extrapolation
//
// Author: Randi Rost
//
// Copyright (c) 2003-2005: 3Dlabs, Inc.
//
// See 3Dlabs-License.txt for license information
//

varying vec2 TexCoord;

void main()

{
    gl_Position = ftransform();
    if(gl_MultiTexCoord0.s==0&&gl_MultiTexCoord0.t==0)
	     TexCoord = gl_Vertex.xy;
	else
         TexCoord  = gl_MultiTexCoord0.st;
}
