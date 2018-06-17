#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "image.h"
#include "dragon.h"

/* x, y: coordinates of turtle */
static long x, y;

/* When drawing a pixel to the image, x and y must be divided by this value.
 * This enables both the dragon curve and twin dragon to rendered without
 * clipping.
 */
static long scale;

/* drawn_pixels: the length of the path travelled. */
static long drawn_pixels;

/* direction: the current direction of the turtle. */
static vector_t direction;

void rotateClockWise();

void rotateAntiClockWise();

/* Returns a vector that describes the initial direction of the turtle. Each
 * iteration corresponds to a 45 degree rotation of the turtle anti-clockwise.  */
vector_t starting_direction(int total_iterations)
{
  switch (total_iterations % 8) {
	  case 0:
          return (vector_t) {1, 0};
	  case 1:
          return (vector_t) {1, 1};

      case 2:
          return (vector_t) {0, 1};

      case 3:
          return (vector_t) {-1, 1};
	  case 4:
          return (vector_t) {-1, 0};

      case 5:
          return (vector_t) {-1, -1};

      case 6:
          return (vector_t) {0, -1};

      case 7:
          return (vector_t) {1, -1};
      default:
          perror("VECTOR NOT CREATED");
          exit(EXIT_FAILURE);
  }
}

/* Draws a pixel to dst at location (x, y). The pixel intensity is chosen as a
 * function of image size and the number of pixels drawn.
 *
 * The gray level changes over entire size of path; the pixel value oscillates
 * along the path to give some contrast between segments close to each other
 * spatially.
 */
void draw_greyscale(image_t *dst,long x, long y)
{
	int level = LEVEL * drawn_pixels / (dst->height * dst->height);

	if (x < 0) {
	    printf("%ld   \n", x);
	}

	switch (level) {
		case 0:
			set_pixel(dst, x, y, 100);
			break;
		case 1:
			set_pixel(dst, x, y, 120);
			break;
		case 2:
			set_pixel(dst, x, y, 150);
			break;
		case 3:
			set_pixel(dst, x, y, 180);
			break;
		case 4:
			set_pixel(dst, x, y, 200);
			break;
		default:
			set_pixel(dst, x, y, 255);
			break;
	}
}
/* Iterates though the characters of str, recusively calling string_iteration()
 * until rules have been applied iterations times, or no rule is applicable.
 * and update the image.
 */
void string_iteration(image_t *dst, const char *str, int iterations)
{
  char c = *str;

//  printf("%d   \n", iterations);

  if (iterations > 0 && *str != '\0') {
      switch (c) {
          case '-':
              rotateClockWise();
              rotateClockWise();
              string_iteration(dst, str + 1, iterations);
              break;
          case '+':
              rotateAntiClockWise();
              rotateAntiClockWise();
              string_iteration(dst, str + 1, iterations);
              break;
          case 'F':
              drawn_pixels++;
              draw_greyscale(dst, x / scale, y / scale);
              x += direction.dx;
              y += direction.dy;
              string_iteration(dst, str + 1, iterations);
              break;
          case 'X':
              string_iteration(dst, "X+YF", iterations - 1);
              string_iteration(dst, str + 1, iterations);
              break;
          case 'Y':
              string_iteration(dst, "FX-Y", iterations - 1);
              string_iteration(dst, str + 1, iterations);
              break;
          default:
              break;
      }
  }
}

void rotateClockWise() {

	if (direction.dx == 1 && (direction.dy == 0 || direction.dy == 1)) {
		direction.dy -= 1;
	} else if ((direction.dx == 0 || direction.dx == 1) && direction.dy == -1) {
		direction.dx -= 1;
	} else if (direction.dx == -1 && (direction.dy == 0 || direction.dy == -1)) {
		direction.dy += 1;
	} else if ((direction.dx == -1 || direction.dx == 0) && direction.dy == 1) {
		direction.dx += 1;
	}



}

void rotateAntiClockWise() {
	if (direction.dx == 1 && (direction.dy == 0 || direction.dy == -1)) {
		direction.dy += 1;
	} else if ((direction.dx == 1 || direction.dx == 0) && direction.dy == 1) {
		direction.dx -= 1;
	}  else if (direction.dx == -1 && (direction.dy == 1 || direction.dy == 0)) {
		direction.dy -= 1;
	}  else if ((direction.dx == -1 || direction.dx == 0) && direction.dy == -1) {
		direction.dx += 1;
	}

}

/* Creates an image of requested size, calls starting_direction() to compute
 * initial turtle direction then calls string_iteration() to construct the
 * image. The constructed image is saved to a file in the output directory.
 */
void dragon(long size, int total_iterations)
{
	image_t **image = malloc(sizeof(image_t *));

	image_error_t image_error = init_image(image, size * 1.5, size, GRAY, DEPTH);

	if (image_error != IMG_OK) {
		image_print_error(image_error);
		exit(EXIT_FAILURE);
	}

//	x = size / 3;
//	y = size / 3;
    x = size;
	y = size;
	scale = 2;

	direction = starting_direction(total_iterations);
	string_iteration(*image, "FX+FX+", total_iterations);

	image_write("jurassicdragon.pgm", *image, PGM_FORMAT);

	image_free(*image);
	free(image);

}

/* The main function. When called with an argument, this should be considered
 * the number of iterations to execute. Otherwise, it is assumed to be 9. Image
 * size is computed from the number of iterations then dragon() is used to
 * generate the dragon image. */
int main(int argc, char **argv)
{

	int total_iterations = argc == 1 ? 9 : atoi(argv[1]);

	dragon((long) pow(2, total_iterations), total_iterations * 2);
}
