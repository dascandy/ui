#include <ui/Texture.hpp>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>

Texture::Texture() 
: textureId(0)
, w(0)
, h(0)
{
    glGenTextures(1, &textureId);
}

std::shared_ptr<Texture> Texture::Create(int w, int h, int format)
{
  return std::make_shared<Texture>(w, h, format);
}

std::shared_ptr<Texture> Texture::Create(const std::string& fileName) 
{
  return std::make_shared<Texture>(fileName);
}

Texture::Texture(std::string fileName)
: Texture()
{
    SDL_Surface* surf = IMG_Load(fileName.c_str());
    format = GL_RGBA8;
    SetContent(surf->w, surf->h, (uint8_t*)surf->pixels);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, 0);
    SDL_FreeSurface(surf);
}

Texture::Texture(int w, int h, int format)
: Texture()
{
    glBindTexture( GL_TEXTURE_2D, textureId );
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture( GL_TEXTURE_2D, 0);
} 

Texture::~Texture()
{
    glDeleteTextures( 1, &textureId );
}

void Texture::SetContent(int w, int h, const unsigned char *data)
{
    glBindTexture( GL_TEXTURE_2D, textureId );
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture( GL_TEXTURE_2D, 0);
}

void Texture::SaveAs(std::string fileName) {
    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
    glBindTexture( GL_TEXTURE_2D, textureId );
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);
    glBindTexture( GL_TEXTURE_2D, 0);
    IMG_SavePNG(surf, fileName.c_str());
    SDL_FreeSurface(surf);
}


