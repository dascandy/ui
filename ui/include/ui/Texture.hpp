#pragma once

#include <GL/glew.h>
#include <map>

class Texture {
public:
    static std::shared_ptr<Texture> Create(int w, int h, int format = GL_RGBA8);
    static std::shared_ptr<Texture> Create(const std::string& fileName);
    Texture();
    Texture(int w, int h, int format = GL_RGBA8);
    Texture(std::string fileName);
    ~Texture();
    int width() { return w; }
    int height() { return h; }
    void SetContent(int w, int h, const unsigned char *data);
    void SaveAs(std::string fileName);
    void Clear();
    inline unsigned int gl_id() { return textureId; }
private:
    unsigned int textureId;
    int w, h;
    int type;
    int format;
};


