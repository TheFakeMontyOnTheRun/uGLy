#ifndef MATRICES_H
#define MATRICES_H

typedef GLfixed t_mat4x4[16];

void mat4x4_ortho(t_mat4x4 out, GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed znear,
                  GLfixed zfar);

void mat4x4_view(t_mat4x4 out, GLfixed cx, GLfixed cy, GLfixed cz, GLfixed tx, GLfixed ty, GLfixed tz, GLfixed ux,
                 GLfixed uy, GLfixed uz);

void mat4x4_perspective(t_mat4x4 out, GLfixed fov, GLfixed ratio, GLfixed znear, GLfixed zfar);

void mat4x4_transform( t_mat4x4 out, GLfixed ox, GLfixed oy, GLfixed oz, GLfixed sx, GLfixed sy, GLfixed sz );

void mat4x4_rotateX( t_mat4x4 out, GLfixed deg );

void mat4x4_rotateY( t_mat4x4 out, GLfixed deg );

void mat4x4_rotateZ( t_mat4x4 out, GLfixed deg );

void mat4x4_transpose( t_mat4x4 mat);

void mat4x4_mul( t_mat4x4 m1, t_mat4x4 m2, t_mat4x4 out);

void mat4x4_identity(t_mat4x4 mat);
#endif /* MATRICES_H */
