#include <ctype.h>
#include <stdint.h>
#include "output.h"
#include "utils.h"

static const char alphabet[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char *base64_encode(const char *input, uint32_t len, uint32_t * setme_len) {
  int i = 0;
  int j = 0;
  unsigned char char_3[3];
  unsigned char char_4[4];
	int count = 0;
	uint32_t enc_size = ((len + 13) / 3) * 4;
	char *ret = am_malloc(enc_size);
	char *out = ret;

	dbg_printf(P_DBG, "Calculated encoded size: %d", (enc_size));
  while (len--) {
    char_3[i++] = *(input++);
    if (i == 3) {
      char_4[0] = (char_3[0] & 0xfc) >> 2;
      char_4[1] = ((char_3[0] & 0x03) << 4) + ((char_3[1] & 0xf0) >> 4);
      char_4[2] = ((char_3[1] & 0x0f) << 2) + ((char_3[2] & 0xc0) >> 6);
      char_4[3] = char_3[2] & 0x3f;

      for(i = 0; i < 4; i++) {
				*out = alphabet[char_4[i]];
				out++;
				count++;
			}
      i = 0;
    }
  }

  if (i != 0)  {
    for(j = i; j < 3; j++) {
      char_3[j] = '\0';
		}

    char_4[0] = (char_3[0] & 0xfc) >> 2;
    char_4[1] = ((char_3[0] & 0x03) << 4) + ((char_3[1] & 0xf0) >> 4);
    char_4[2] = ((char_3[1] & 0x0f) << 2) + ((char_3[2] & 0xc0) >> 6);
    char_4[3] = char_3[2] & 0x3f;

    for (j = 0; j < i + 1; j++) {
      *out = alphabet[char_4[j]];
			out++;
			count++;
		}
    while(i++ < 3) {
      *out = '=';
			out++;
			count++;
		}
  }
	*out = '\0';
	dbg_printf(P_DBG, "Actual written characters: %d", count);
	if(setme_len) {
		*setme_len = count;
	}
  return ret;
}

static int is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

static char find(char c) {
	int i;
	for(i = 0; i < 64; i++) {
		if(alphabet[i] == c) {
			return i;
		}
	}
	return -1;
}

char *base64_decode(const char *encoded_string, uint32_t in_len, uint32_t * setme_len) {
  int i = 0;
  int j = 0;
  int in_ = 0;
	int count = 0;
  unsigned char char_array_4[4], char_array_3[3];
  char *ret = am_malloc(3 * (in_len / 4) + 2);
	char *out = ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i < 4; i++) {
        char_array_4[i] = find(char_array_4[i]);
			}

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++) {
        *out = char_array_3[i];
				out++;
				count++;
			}
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) {
			*out = char_array_3[j];
			out++;
			count++;
		}
  }
	*out = '\0';

	if(setme_len) {
		*setme_len = count;
	}

  return ret;
}


/*
int main(int argc, char **argv) {
	FILE *in, *out;
	char *raw_data, *encoded, *decoded;
	struct stat fs;
	int enc_len = 0, dec_len = 0;

	if(stat("test.torrent", &fs) == -1)  {
		return -1;
	}

	if ((raw_data = am_malloc(fs.st_size + 1)) == NULL) {
		return -1;
	}
	printf("file size: %d\n", (uint32_t)fs.st_size);

	if ((in = fopen("test.torrent", "rb")) == NULL) {
		perror("fopen");
		am_free(raw_data);
		return -1;
	}

	if(fread(raw_data, fs.st_size, 1, in) != 1) {
		perror("fread");
		fclose(in);
		am_free(raw_data);
		return -1;
	}
	if(in)
		fclose(in);

	encoded = base64_encode(raw_data, fs.st_size, &enc_len);
	printf("encode successful: %d\n", enc_len);
	if ((out = fopen("test_torrent.b64", "wb")) == NULL) {
		perror("fopen");
		am_free(raw_data);
		return -1;
	}
	if(fwrite(encoded, strlen(encoded), 1, out) != 1) {
		perror("fwrite");
		fclose(out);
		am_free(raw_data);
		return -1;
	}
	fclose(out);
	printf("written encoded data\n");
	decoded = base64_decode(encoded, &dec_len);
	printf("decode successful: %d\n", dec_len);
	if ((out = fopen("decoded.torrent", "wb")) == NULL) {
		perror("fopen");
		return -1;
	}
	if(fwrite(decoded, dec_len, 1, out) != 1) {
		perror("fwrite");
		fclose(out);
		am_free(raw_data);
		return -1;
	}
	printf("written decoded data\n");
	fclose(out);
	am_free(raw_data);
	am_free(encoded);
	am_free(decoded);

	return 0;
}
*/