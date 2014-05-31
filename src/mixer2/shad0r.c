#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <frei0r.h>

static GLFWwindow* window = NULL;
static pthread_mutex_t gl_mutex;

static const GLchar * const VERTEX_SHADER_SOURCE =
    "#version 150 core\n"
    "in vec2 position;"
    "void main() {"
        "gl_Position = vec4(2.0 * position - 1.0, 0.0, 1.0);"
    "}";

//XXX temporary
static const GLchar * const FRAGMENT_SHADER_SOURCE =
    "#version 150 core\n"
    "out vec4 color;"
    "uniform sampler2D from, to;"
    "uniform vec2 resolution;"
    "void main() {"
        "vec2 p = gl_FragCoord.xy / resolution.xy;"
        "color = mix(texture(from, p), texture(to, p), 0.5);"
    "}";

typedef struct shad0r_instance {
    unsigned int width;
    unsigned int height;
    GLuint fbo;
    GLuint rbo;
    GLuint vao;
    GLuint vbo;
    GLuint program;
    GLuint src_tex;
    GLuint dst_tex;
} shad0r_instance_t;

// glfw context must be current and mutex locked when calling this
static void destroy(shad0r_instance_t *instance) {
    if (!instance)
        return;
    if (instance->fbo)
        glDeleteFramebuffers(1, &instance->fbo);
    if (instance->rbo)
        glDeleteRenderbuffers(1, &instance->rbo);
    if (instance->vao)
        glDeleteVertexArrays(1, &instance->vao);
    if (instance->vbo)
        glDeleteBuffers(1, &instance->vbo);
    if (instance->program)
        glDeleteProgram(instance->program);
    if (instance->src_tex)
        glDeleteTextures(1, &instance->src_tex);
    if (instance->dst_tex)
        glDeleteTextures(1, &instance->dst_tex);
    free(instance);
}

static GLuint compile_shader(GLenum shader_type, const GLchar *source) {
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE) {
        GLint log_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        GLchar *log = malloc(log_length);
        glGetShaderInfoLog(shader, log_length, &log_length, log);
        fprintf(stderr, "ERROR: shad0r shader failed to compile\n");
        fprintf(stderr, "%s\n", log);
        free(log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static GLuint create_texture() {
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return tex;
}

// program should be in use
static void bind_texture_uniform(GLuint program, const GLchar *name, GLuint tex, GLenum unit) {
    GLint location = glGetUniformLocation(program, name);
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(location, unit);
}

static void error_handler(int code, const char *message) {
    fprintf(stderr, "ERROR: shad0r glfw error code 0x%x: %s\n", code, message);
}

int f0r_init() {
    if (window)
        goto finish;
    glfwSetErrorCallback(error_handler);
    if (!glfwInit())
        goto finish;
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_DEPTH_BITS, 0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    window = glfwCreateWindow(1, 1, "shad0r", NULL, NULL);
    pthread_mutex_init(&gl_mutex, NULL);

finish:
    return window ? 1 : 0;
}

void f0r_deinit() {
    //XXX can only be called from main thread, must not be current on any other threads
    if (window) {
        glfwMakeContextCurrent(window);
        glfwDestroyWindow(window);
        window = NULL;
    }
    glfwTerminate();
    pthread_mutex_destroy(&gl_mutex);
}

void f0r_get_plugin_info(f0r_plugin_info_t* shad0rInfo) {
    shad0rInfo->name = "shad0r";
    shad0rInfo->author = "Andrew Wason";
    shad0rInfo->plugin_type = F0R_PLUGIN_TYPE_MIXER2;
    shad0rInfo->color_model = F0R_COLOR_MODEL_RGBA8888;
    shad0rInfo->frei0r_version = FREI0R_MAJOR_VERSION;
    shad0rInfo->major_version = 0;
    shad0rInfo->minor_version = 9;
    shad0rInfo->num_params =  0; //XXX
    shad0rInfo->explanation = "Applies a WebGL GLSL fragment shader as a transition, see glsl.io";
}

void f0r_get_param_info(f0r_param_info_t* info, int param_index) {
    //XXX need shader, uniforms, from, to, time
}

f0r_instance_t f0r_construct(unsigned int width, unsigned int height) {
    shad0r_instance_t* instance = (shad0r_instance_t *)calloc(1, sizeof(*instance));
    instance->width = width;
    instance->height = height;

    pthread_mutex_lock(&gl_mutex);
    glfwMakeContextCurrent(window);

    glGenFramebuffers(1, &instance->fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, instance->fbo);
 
    glGenRenderbuffers(1, &instance->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, instance->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, instance->rbo);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        goto fail;
 
    instance->src_tex = create_texture();
    if (!instance->src_tex)
        goto fail;
    instance->dst_tex = create_texture();
    if (!instance->dst_tex)
        goto fail;

    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE);
    if (!vertex_shader)
        goto fail;
    //XXX load webgl shader file and translate
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_SOURCE);
    if (!fragment_shader)
        goto fail;

    instance->program = glCreateProgram();
    glAttachShader(instance->program, vertex_shader);
    glAttachShader(instance->program, fragment_shader);
    glLinkProgram(instance->program);

    GLint linked = 0;
    glGetProgramiv(instance->program, GL_LINK_STATUS, &linked);
    if (linked == GL_FALSE) {
        GLint log_length = 0;
        glGetProgramiv(instance->program, GL_INFO_LOG_LENGTH, &log_length);
        GLchar *log = malloc(log_length);
        glGetProgramInfoLog(instance->program, log_length, &log_length, log);
        fprintf(stderr, "ERROR: shad0r program failed to link\n");
        fprintf(stderr, "%s\n", log);
        free(log);
        goto fail;
    }

    // On successful link, detach/delete shaders
    glDetachShader(instance->program, vertex_shader);
    glDeleteShader(vertex_shader);
    vertex_shader = 0;
    glDetachShader(instance->program, fragment_shader);
    glDeleteShader(fragment_shader);
    fragment_shader = 0;

    //XXX use parameterized uniform names
    glUseProgram(instance->program);
    bind_texture_uniform(instance->program, "from", instance->src_tex, GL_TEXTURE0);
    bind_texture_uniform(instance->program, "to", instance->dst_tex, GL_TEXTURE1);
    GLint location = glGetUniformLocation(instance->program, "resolution");
    glUniform2f(location, width, height);

    glGenVertexArrays(1, &instance->vao);
    glBindVertexArray(instance->vao);
    GLfloat x1 = 0, x2 = width, y1 = 0, y2 = height;
    GLfloat vertices[] = {
        x1, y1,
        x2, y1,
        x1, y2,
        x1, y2,
        x2, y1,
        x2, y2,
    };
    glGenBuffers(1, &instance->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, instance->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    GLuint position = glGetAttribLocation(instance->program, "position");
    glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(position);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "ERROR: shad0r GL error %x\n", error);
        goto fail;
    }

    goto unlock;

fail:
    if (vertex_shader) {
        if (instance->program)
            glDetachShader(instance->program, vertex_shader);
        glDeleteShader(vertex_shader);
    }
    if (fragment_shader) {
        if (instance->program)
            glDetachShader(instance->program, fragment_shader);
        glDeleteShader(fragment_shader);
    }
    destroy(instance);
    instance = NULL;

unlock:
    pthread_mutex_unlock(&gl_mutex);
    return (f0r_instance_t)instance;
}

void f0r_destruct(f0r_instance_t instance) {
    pthread_mutex_lock(&gl_mutex);
    glfwMakeContextCurrent(window);
    destroy((shad0r_instance_t *)instance);
    pthread_mutex_unlock(&gl_mutex);
}

void f0r_set_param_value(f0r_instance_t instance, f0r_param_t param, int param_index) {
    //XXX fi
}

void f0r_get_param_value(f0r_instance_t instance, f0r_param_t param, int param_index) {
    //XXX fix
}

void f0r_update2(f0r_instance_t instance,
                 double time,
                 const uint32_t* inframe1,
                 const uint32_t* inframe2,
                 const uint32_t* inframe3,
                 uint32_t* outframe) {
    shad0r_instance_t* inst = (shad0r_instance_t *)instance;

    pthread_mutex_lock(&gl_mutex);
    glfwMakeContextCurrent(window);

    glBindTexture(GL_TEXTURE_2D, inst->src_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, inst->width, inst->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, inframe1);

    glBindTexture(GL_TEXTURE_2D, inst->dst_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, inst->width, inst->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, inframe2);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, inst->fbo);
    glViewport(0, 0, inst->width, inst->height);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(inst->program);
    glBindVertexArray(inst->vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glReadPixels(0, 0, inst->width, inst->height, GL_RGBA, GL_UNSIGNED_BYTE, outframe);

    pthread_mutex_unlock(&gl_mutex);
}