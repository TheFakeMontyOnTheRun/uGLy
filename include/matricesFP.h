#ifndef MATRICES_H
#define MATRICES_H

typedef GLfixed t_mat4x4[16];
typedef GLfixed t_vec4[4];

void
mat4x4_transformVec(t_vec4 out,
                    t_mat4x4 m, t_vec4 v);

void mat4x4_transform(t_mat4x4 out, GLfixed ox, GLfixed oy, GLfixed oz, GLfixed sx, GLfixed sy, GLfixed sz);

void mat4x4_transpose(t_mat4x4 mat);

void mat4x4_mul(t_mat4x4 m1, t_mat4x4 m2, t_mat4x4 out);

void mat4x4_identity(t_mat4x4 mat);

GLfixed dotVec( t_vec4 v1,  t_vec4 v2);

uint8_t normalizeVec( t_vec4 v,  t_vec4 out);

#endif /* MATRICES_H */
