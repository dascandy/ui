#include <ui/ShaderProgram.hpp>
#include <ui/Texture.hpp>
#include <vector>
#define MAX_TEXTURE_UNITS 8

static inline void compileShader(const std::string &filename, int prog, int &shader, const char **source, int type)
{
  shader = glCreateShader(type);
  glAttachShader(prog, shader);

  glShaderSource(shader, 1, (const char **)source, NULL);
  glCompileShader(shader);
  int ok = true;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
  if (!ok)
  {
    int length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    char buffer[1024];
    glGetShaderInfoLog(shader, length, NULL, buffer);
    throw std::runtime_error("Error while compiling " + filename + "\n" + buffer);
  }
}

ShaderProgram::ShaderProgram(const std::string &filename, const char *vss, const char *fss, const char *gss, const char *css, const char *ess)
: filename(filename)
, curtex(0)
, vs(0)
, fs(0)
, gs(0)
, cs(0)
, es(0)
, prog(glCreateProgram())
{
  compileShader(filename, prog, vs, &vss, GL_VERTEX_SHADER);
  compileShader(filename, prog, fs, &fss, GL_FRAGMENT_SHADER);
  if (gss) {
    compileShader(filename, prog, gs, &gss, GL_GEOMETRY_SHADER);
  }
  if (ess) {
    if (css) {
      compileShader(filename, prog, cs, &css, GL_TESS_CONTROL_SHADER);
    }
    compileShader(filename, prog, es, &ess, GL_TESS_EVALUATION_SHADER);
  }

  int ok = false;
  glLinkProgram(prog);
  glGetProgramiv(prog, GL_LINK_STATUS, &ok);
  if (!ok)
  {
    int length = 0;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &length);
    std::string errors;
    errors.reserve(length);
    glGetProgramInfoLog(prog, length, NULL, &errors[0]);
    throw std::runtime_error("Error while linking " + filename + "\n" + errors);
  }
}

ShaderProgram::~ShaderProgram()
{
  glDeleteShader(this->vs);
  glDeleteShader(this->fs);
  if (this->gs) glDeleteShader(this->gs);
  if (this->cs) glDeleteShader(this->cs);
  if (this->es) glDeleteShader(this->es);
  glDeleteProgram(this->prog);
}

static int usedProg = 0;

void ShaderProgram::Set(const char *name, const glm::mat4 &mat)
{
  int uniform = getUniformLocation(name);
  if (uniform == -1) return;

  glUniformMatrix4fv(uniform, 1, GL_FALSE, &mat[0][0]);
}

void ShaderProgram::Set(const char *name, const glm::vec2 &vec)
{
  int uniform = getUniformLocation(name);
  if (uniform == -1) return;

  glUniform2f(uniform, vec.x, vec.y);
}

void ShaderProgram::Set(const char *name, int value)
{
  int uniform = getUniformLocation(name);
  if (uniform == -1) return;

  glUniform1i(uniform, value);
}

void ShaderProgram::Set(const char *name, unsigned int value)
{
  int uniform = getUniformLocation(name);
  if (uniform == -1) return;

  glUniform1ui(uniform, value);
}

void ShaderProgram::Set(const char *name, Texture &tex)
{
  if (texUnits.find(name) == texUnits.end()) {
    int uniform = getUniformLocation(name);
    if (uniform == -1) return;
    glUniform1i(uniform, curtex);
    texUnits[name] = curtex;
    curtex++;
  }
  if (curtex >= MAX_TEXTURE_UNITS)
    throw std::runtime_error("More textures registered to shader than hardware supports!");

  glActiveTexture(GL_TEXTURE0+texUnits[name]);
  glBindTexture(GL_TEXTURE_2D, tex.gl_id());
}

void ShaderProgram::Set(const char *name, float value)
{
  int uniform = getUniformLocation(name);
  if (uniform == -1) return;

  glUniform1f(uniform, value);
}

void ShaderProgram::Set(const char *name, const glm::vec3 &value)
{
  int uniform = getUniformLocation(name);
  if (uniform == -1) return;

  glUniform3f(uniform, value.x, value.y, value.z);
}

void ShaderProgram::SetActive() 
{
  usedProg = prog;
  glUseProgram(prog); 
}

int ShaderProgram::getUniformLocation(const char *name) {
  std::map<std::string, int>::iterator it = uniforms.find(name);
  if (it != uniforms.end()) return it->second;
  int value = uniforms[name] = glGetUniformLocation(prog, name);
  return value;
}

void ShaderProgram::UnbindTextures() {
  for (; curtex; curtex--) {
    glActiveTexture(GL_TEXTURE0+curtex-1);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}
