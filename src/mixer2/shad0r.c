#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <frei0r.h>

static GLFWwindow* window = NULL;
static pthread_mutex_t gl_mutex;

typedef struct shad0r_instance {
    unsigned int width;
    unsigned int height;
    GLuint fbo;
    GLuint rbo;
    GLuint vao;
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

int f0r_init() {
    pthread_mutex_init(&gl_mutex, NULL);
    if (!glfwInit())
        goto error;
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_DEPTH_BITS, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(1, 1, "shad0r", NULL, NULL);

error:
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

    //XXX compile shader, create FBO and textures

    glGenFramebuffers(1, &instance->fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, instance->fbo);
 
    glGenRenderbuffers(1, &instance->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, instance->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, instance->rbo);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        destroy(instance);
        instance = NULL;
        goto error;
    }
 
error:
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
    pthread_mutex_lock(&gl_mutex);
    glfwMakeContextCurrent(window);
    //XXX
    pthread_mutex_unlock(&gl_mutex);
}