 /**
 * dst-info.c
 *   Print out information on a .dst file (or so I hope).
 */

// +---------+-------------------------------------------------------
// | Headers |
// +---------+

#include <stdio.h>

// +-----------+-----------------------------------------------------
// | Constants |
// +-----------+

#define BIT7 128
#define BIT6  64
#define BIT5  32
#define BIT4  16
#define BIT3   8
#define BIT2   4
#define BIT1   2
#define BIT0   1

// +-----------------+-----------------------------------------------
// | Predeclarations |
// +-----------------+


// +---------+-------------------------------------------------------
// | Helpers |
// +---------+

int
checkbit (int byte, int mask, int val)
{
  if (byte & mask)
    return val;
  else
    return 0;
} // checkbit

void
print_bits (int ch)
{
  int mask = BIT7;
  while (mask)
    {
      if (mask & ch)
        printf ("1");
      else
        printf ("0");
      mask = mask >> 1;
    } // while
} // print_bits

/**
 * Read one stitch.  Returns 1 if it succeeded and 0 for end of pattern.
 */
int
process_stitch (FILE *source)
{
  // Stitches are three bytes
  int byte1 = fgetc (source);
  int byte2 = fgetc (source);
  int byte3 = fgetc (source);

  // If we've hit the end of file, return
  if ((byte1 == -1) || (byte2 == -1) || (byte3 == -1))
    {
      fprintf (stderr, "EOF\n");
      return 0;
    } // If EOF

  // If we've hit the end of the pattern, return
  if ((byte1 == 0) && (byte2 == 0) && (byte3 == 0xF3))
    {
      fprintf (stderr, "end-of-pattern\n");
      return 0;
    } // if end-of-pattern

  // Display the bits 
  print_bits (byte1);
  printf (" ");
  print_bits (byte2);
  printf (" ");
  print_bits (byte3);
  printf ("\t");

  // Interpret the bits
  int y = 0;
  int x = 0;

  y += checkbit (byte1, BIT7, 1);
  y += checkbit (byte1, BIT6, -1);
  y += checkbit (byte1, BIT5, 9);
  y += checkbit (byte1, BIT4, -9);

  x += checkbit (byte1, BIT3, 9);
  x += checkbit (byte1, BIT2, -9);
  x += checkbit (byte1, BIT1, 1);
  x += checkbit (byte1, BIT0, -1);

  y += checkbit (byte2, BIT7, 3);
  y += checkbit (byte2, BIT6, -3);
  y += checkbit (byte2, BIT5, 27);
  y += checkbit (byte2, BIT4, -27);

  x += checkbit (byte2, BIT3, 27);
  x += checkbit (byte2, BIT2, -27);
  x += checkbit (byte2, BIT1, 3);
  x += checkbit (byte2, BIT0, -3);

  int jump = checkbit (byte3, BIT7, 1);
  int change = checkbit (byte3, BIT6, 1);

  printf ("x += %d, y += %d", x, y);
  if (jump) 
    printf (" jump");
  if (change)
    printf (" stop");
  printf ("\n");

  return 1;
}

/**
 * Read .dst data and print out information.
 */
void
process_data (FILE *source)
{
  int count = 0;

  while ((count++ < 20) && (process_stitch (source)))
    ;
} // process_data

/**
 * Read one of the fields in a header.
 */
void
process_field (FILE *source, int length)
{
  int i;
  for (i = 0; i < length-1; i++)
    {
      int ch = fgetc (source);
      putchar (ch);
    } // for
  fgetc (source);
  putchar ('\n');
} // process_field

/**
 * Read a .dst header and print out the information.
 */
void
process_header (FILE *source)
{
  process_field (source, 20);   // Label
  process_field (source, 11);   // Stitch count
  process_field (source, 7);    // Color count
  process_field (source, 9);    // Maximum X extent
  process_field (source, 9);    // Minimum X extent
  process_field (source, 9);    // Maximum Y extent
  process_field (source, 9);    // Minimum Y extent
  process_field (source, 10);   // End X
  process_field (source, 10);   // End Y
  process_field (source, 10);   // Previous File End X
  process_field (source, 10);   // Previous File End Y
  process_field (source, 10);   // Previous File
  process_field (source, 4);    // End Header
  process_field (source, 384);  // Junk
} // process_header

// +------+----------------------------------------------------------
// | Main |
// +------+

int
main (int argc, char *argv[])
{
  FILE *source = fopen("Star.DST","rb");
  process_header (source);
  process_data (source);
  return 0;
} // main
