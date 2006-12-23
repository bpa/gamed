#include <png.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
    static png_FILE_p read_fp, write_fp;
    int is_png, width, height, i, j, k, a;
    png_byte bit_depth, rowbytes;
    png_bytep header, row, pixel;
    png_bytepp row_pointers;
    png_structp png_ptr, write_ptr;
    png_infop info_ptr;
    png_size_t number = 8;
    header = malloc(number);

    if (argc != 3) {
        printf("Usage: %s <in png> <out png>\n", argv[0]);
        return 1;
    }
    read_fp = fopen(argv[1], "rb");
    if (!read_fp) {
        printf("Can't open %s\n", argv[1]);
        return 2;
    }
    write_fp = fopen(argv[2], "wb");
    if (!write_fp) {
        printf("Can't open %s for writing\n", argv[2]);
        return 3;
    }
    fread(header, 1, number, read_fp);
    is_png = !png_sig_cmp(header, 0, number);
    if (!is_png) {
        printf("%s is not a png image\n", argv[1]);
        return 3;
    }
    free(header);

    png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, 
       png_voidp_NULL, png_error_ptr_NULL, png_error_ptr_NULL);
    if (!png_ptr) return 3;

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr,
                (png_infopp)NULL, (png_infopp)NULL);
        return (3);
    }

    png_init_io(png_ptr, read_fp);
    png_set_sig_bytes(png_ptr, number);
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    width  = png_get_image_width (png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    printf("w: %i h: %i d: %i b: %i\n", width, height, bit_depth, rowbytes);
 
    row_pointers = png_get_rows(png_ptr, info_ptr);
    for (i=0; i<height; i++) {
        row = row_pointers[i];
        for (j=0; j<width; j++) {
            pixel = &row[j*2+1];
            if (*pixel) {
                *pixel = 127;
                if (i==0        || !row_pointers[i-1][  j  *2+1]) { *pixel = 223; }
                if (i==height-1 || !row_pointers[i+1][  j  *2+1]) { *pixel = 223; }
                if (j==0        || !row_pointers[ i ][(j-1)*2+1]) { *pixel = 223; }
                if (j==width-1  || !row_pointers[ i ][(j+1)*2+1]) { *pixel = 223; }
            }
        }
    }

    for (k=64; k>0; k>>=1) {
        a = 127 + k;
        for (i=0; i<height; i++) {
            row = row_pointers[i];
            for (j=0; j<width; j++) {
                pixel = &row[j*2+1];
                if (*pixel && *pixel < a) {
                    if (row_pointers[i-1][  j  *2+1] > a) { *pixel = a; }
                    if (row_pointers[i+1][  j  *2+1] > a) { *pixel = a; }
                    if (row_pointers[ i ][(j-1)*2+1] > a) { *pixel = a; }
                    if (row_pointers[ i ][(j+1)*2+1] > a) { *pixel = a; }
                }
            }
        }
    }

    write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, png_voidp_NULL,
            png_error_ptr_NULL, png_error_ptr_NULL);
    if (!write_ptr) return 3;

    png_init_io(write_ptr, write_fp);
    png_write_png(write_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    fclose(read_fp);
    fclose(write_fp);
    return 0;
}
