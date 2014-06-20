/*
 * Copyright 2014 Andrew Wason rectalogic@rectalogic.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLSLANG/ShaderLang.h>
#include <frei0r.h>

static GLFWwindow* window = NULL;
static ShBuiltInResources angle_resources;

static pthread_mutex_t gl_mutex;

static const GLchar * const VERTEX_SHADER_SOURCE =
    "attribute vec2 position;"
    "void main() {"
        "gl_Position = vec4(2.0 * position - 1.0, 0.0, 1.0);"
    "}";

//XXX temporary
static const GLchar * const FRAGMENT_SHADER_SOURCE =
    "#ifdef GL_ES\n"
    "precision highp float;\n"
    "#endif\n"
    "uniform sampler2D from, to;"
    "uniform float progress;"
    "uniform vec2 resolution;"
    "void main() {"
        "vec2 p = gl_FragCoord.xy / resolution.xy;"
        "gl_FragColor = mix(texture2D(from, p), texture2D(to, p), progress);"
    "}";

typedef struct shad0r_instance {
    unsigned int width;
    unsigned int height;
    GLuint fbo;
    GLuint rbo;
    GLuint vbo;
    GLuint ebo;
    GLuint program;
    GLuint src_tex;
    GLuint dst_tex;
    GLint progress_location;
} shad0r_instance_t;

// glfw context must be current and mutex locked when calling this
static void destroy(shad0r_instance_t *instance) {
    if (!instance)
        return;
    if (instance->fbo)
        glDeleteFramebuffers(1, &instance->fbo);
    if (instance->rbo)
        glDeleteRenderbuffers(1, &instance->rbo);
    if (instance->vbo)
        glDeleteBuffers(1, &instance->vbo);
    if (instance->ebo)
        glDeleteBuffers(1, &instance->ebo);
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
static void bind_texture_uniform(GLuint program, const GLchar *name, GLuint tex, GLuint unit) {
    GLint location = glGetUniformLocation(program, name);
    glActiveTexture(GL_TEXTURE0 + unit);
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    window = glfwCreateWindow(1, 1, "shad0r", NULL, NULL);

    // Initialize ANGLE and glew
    if (window) {
        glfwMakeContextCurrent(window);

        GLenum err = glewInit();
        if (err == GLEW_OK && GLEW_ARB_framebuffer_object) {
            ShInitialize();
            ShInitBuiltInResources(&angle_resources);
            glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &angle_resources.MaxVertexAttribs);
            glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &angle_resources.MaxVertexUniformVectors);
            glGetIntegerv(GL_MAX_VARYING_FLOATS, &angle_resources.MaxVaryingVectors);
            glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &angle_resources.MaxVertexTextureImageUnits);
            glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &angle_resources.MaxCombinedTextureImageUnits);
            glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &angle_resources.MaxTextureImageUnits);
            glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &angle_resources.MaxFragmentUniformVectors);

            // Always set to 1 for OpenGL ES.
            angle_resources.MaxDrawBuffers = 1;
            angle_resources.FragmentPrecisionHigh = 1;
        }
        else {
            if (err != GLEW_OK)
                fprintf(stderr, "ERROR: shad0r glew error %s\n", glewGetErrorString(err));
            else
                fprintf(stderr, "ERROR: shad0r glew required extensions are not available\n");
        }
    }


    pthread_mutex_init(&gl_mutex, NULL);

finish:
    return window ? 1 : 0;
}

void f0r_deinit() {
    ShFinalize();
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

    ShHandle fragment_compiler = ShConstructCompiler(SH_FRAGMENT_SHADER, SH_WEBGL_SPEC, SH_GLSL_OUTPUT, &angle_resources);
    if (!fragment_compiler)
        goto fail;
    //XXX load webgl shader file
    if (!ShCompile(fragment_compiler, &FRAGMENT_SHADER_SOURCE, 1, SH_OBJECT_CODE)) {
        size_t log_length = 0;
        ShGetInfo(fragment_compiler, SH_INFO_LOG_LENGTH, &log_length);
        char *log = malloc(log_length);
        ShGetInfoLog(fragment_compiler, log);
        fprintf(stderr, "ERROR: shad0r WebGL shader failed to compile\n");
        fprintf(stderr, "%s\n", log);
        free(log);
        goto fail;
    }
    GLuint fragment_shader = 0;
    size_t translated_source_length = 0;
    ShGetInfo(fragment_compiler, SH_OBJECT_CODE_LENGTH, &translated_source_length);
    if (translated_source_length > 1) {
        char *translated_source = malloc(translated_source_length);
        ShGetObjectCode(fragment_compiler, translated_source);
        fragment_shader = compile_shader(GL_FRAGMENT_SHADER, translated_source);
        free(translated_source);
        if (!fragment_shader)
            goto fail;
    }
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE);
    if (!vertex_shader)
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
    bind_texture_uniform(instance->program, "from", instance->src_tex, 0);
    bind_texture_uniform(instance->program, "to", instance->dst_tex, 1);
    GLint location = glGetUniformLocation(instance->program, "resolution");
    glUniform2f(location, width, height);
    instance->progress_location = glGetUniformLocation(instance->program, "progress");

    GLfloat x1 = 0, x2 = width, y1 = 0, y2 = height;
    GLfloat vertices[] = {
        x1, y1,
        x2, y1,
        x1, y2,
        x2, y2,
    };
    glGenBuffers(1, &instance->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, instance->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    GLuint position = glGetAttribLocation(instance->program, "position");
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLushort elements[] = { 0, 1, 2, 3 };
    glGenBuffers(1, &instance->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instance->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "ERROR: shad0r GL error 0x%x\n", error);
        goto fail;
    }

    goto unlock;

fail:
    if (fragment_compiler)
        ShDestruct(fragment_compiler);
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

    glUniform1f(inst->progress_location, time);

    glBindFramebuffer(GL_FRAMEBUFFER, inst->fbo);
    glViewport(0, 0, inst->width, inst->height);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(inst->program);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, inst->ebo);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);

    glReadPixels(0, 0, inst->width, inst->height, GL_RGBA, GL_UNSIGNED_BYTE, outframe);

    pthread_mutex_unlock(&gl_mutex);
}