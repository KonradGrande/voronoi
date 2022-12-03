#include "ppm/ppm.h"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct point {
  int x, y;
  color_t color;
} point_t;

color_t theme[] = {{0xFF, 0x6C, 0x6B}, {0xDA, 0x85, 0x48}, {0x98, 0xBE, 0x65},
                   {0x4D, 0xB5, 0xBD}, {0xEC, 0xBE, 0x7B}, {0x51, 0xAF, 0xEF},
                   {0x22, 0x57, 0xA0}, {0xC6, 0x78, 0xDD}, {0xA9, 0xA1, 0xE1},
                   {0x46, 0xD9, 0xFF}, {0x56, 0x99, 0xAF}, {0xFC, 0x6A, 0x5D},
                   {0xFD, 0x8F, 0x3F}, {0xD0, 0xBF, 0x68}, {0x67, 0xB7, 0xA4},
                   {0x5D, 0xD8, 0xFF}, {0x59, 0xB0, 0xCF}, {0xD0, 0xA8, 0xFF},
                   {0x8a, 0xbe, 0xb7}};

int THEME_SIZE = sizeof(theme) / sizeof(theme[0]);

point_t *generate_points(image_t *img, int num_points) {
  point_t *points = malloc(sizeof(point_t) * num_points);
  if (!points) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  srand(time(NULL));

  for (int i = 0; i < num_points; i++) {
    points[i].x = rand() % img->width;
    points[i].y = rand() % img->height;
    points[i].color = theme[i % THEME_SIZE];
  }

  return points;
}

int euclid_dist(int x1, int y1, int x2, int y2) {
  int xd = x1 - x2;
  int yd = y1 - y2;

  return xd * xd + yd * yd;
}

int snake_dist(int x1, int y1, int x2, int y2) {
  int xd = x1 - x2;
  if (xd < 0)
    xd += -2 * xd;

  int yd = y1 - y2;
  if (yd < 0)
    yd += -2 * yd;

  return xd + yd;
}

void draw_circle(image_t *img, int cx, int cy, int r, color_t color) {
  // starting in the left corner
  for (int y = cy - r; y < cy + r; y++)
    for (int x = cx - r; x < cx + r; x++)
      if (euclid_dist(x, y, cx, cy) <= r * r)
        image_pixel_set(img, x, y, color);
}

void draw_ring(image_t *img, int cx, int cy, int outer_r, int inner_r,
               color_t color) {
  // starting in the left corner
  for (int y = cy - outer_r; y < cy + outer_r; y++)
    for (int x = cx - outer_r; x < cx + outer_r; x++) {
      int dist = euclid_dist(x, y, cx, cy);
      if (dist <= outer_r * outer_r && dist >= inner_r * inner_r)
        image_pixel_set(img, x, y, color);
    }
}

void draw_points(image_t *img, point_t *points, int num_points) {
  int radius = 3;

  for (int i = 0; i < num_points; i++)
    draw_circle(img, points[i].x, points[i].y, radius, color(0xffffff));
}

void draw_points_as_rings(image_t *img, point_t *points, int num_points) {
  int outer_radius = 5;
  int inner_radius = 3;

  for (int i = 0; i < num_points; i++)
    draw_ring(img, points[i].x, points[i].y, outer_radius, inner_radius,
              color(0x0f0f0f));
}

void draw_vonoroi(image_t *img, point_t *points, int num_points,
                  int (*dist)(int, int, int, int)) {
#pragma omp parallel for num_threads(8)
  for (int y = 0; y < img->height; y++)
    for (int x = 0; x < img->width; x++) {
      int closest_point = 0;
      int closest_dist = dist(x, y, points[0].x, points[0].y);

      for (int i = 1; i < num_points; i++) {
        int dist_to_i = dist(x, y, points[i].x, points[i].y);
        if (dist_to_i < closest_dist) {
          closest_point = i;
          closest_dist = dist_to_i;
        }
      }

      image_pixel_set(img, x, y, points[closest_point].color);
    }
}

int main() {
  char *path = "voronoi.ppm";
  int width = 1920;
  int height = 1080;
  int num_points = 100;

  image_t *img = image_init(width, height);
  point_t *points = generate_points(img, num_points);

  draw_vonoroi(img, points, num_points, snake_dist);
  draw_points(img, points, num_points);

  image_write(path, img);

  image_free(img);
  free(points);

  return 0;
}
