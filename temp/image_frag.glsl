
    #version 330 core
    out vec4 FragColor;
    
    in vec2 TexCoord;
    
    uniform sampler2D ourTexture;
    uniform float opacity;
    
    void main()
    {
        vec4 texColor = texture(ourTexture, TexCoord);
        FragColor = vec4(texColor.rgb, texColor.a * opacity);
    }
