uniform sampler2D texture;

void main()
{
    // lookup the pixel in the texture
    //vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

    // multiply it by the color
    //gl_FragColor = vec4(gl_TexCoord[0].xy);

    
    gl_FragColor = vec4(gl_TexCoord[0].x, gl_TexCoord[0].y, 0, gl_TexCoord[0].y);
}