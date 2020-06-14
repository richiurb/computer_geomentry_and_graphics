#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

//#pragma warning(disable : 4996)

bool new_file(int type_p, int width, int height, int maxColorValue, unsigned char* k, char* name_file, int i) {
	FILE* rFile;
	rFile = fopen(name_file, "wb");

	if (rFile == NULL) {
		fputs("File not found!", stderr);
		return false;
	}
	else {
		fprintf(rFile, "P%i\n%i %i\n%i\n", type_p, width, height, maxColorValue);
		fwrite(k, sizeof(unsigned char), width * height * i, rFile);
		fclose(rFile);
		return true;
	}
}

int main(int argc, char* argv[]) {
	FILE* pFile;

	char* input_file_name = argv[1];
	char* output_file_name = argv[2];

	if ((input_file_name == NULL) || (output_file_name == NULL)) {
		fputs("Problem with comands from console", stderr);
		exit(9);
	}
	pFile = fopen(input_file_name, "rb");

	if (pFile == NULL) {
		fputs("File not found!", stderr);
		exit(1);
	}

	int width, height, maxColorValue;
	int type_p;
	char c_type;

	fscanf(pFile, "%c%i%i%i%i\n", &c_type, &type_p, &width, &height, &maxColorValue);

	if (c_type != 'P') {
		fputs("Problem with type of file!", stderr);
		fclose(pFile);
		exit(2);
	}

	if (maxColorValue != 255) {
		fputs("Value of maxColor is not correct!", stderr);
		fclose(pFile);
		exit(3);
	}

	if ((width <= 0) || (height <= 0)) {
		fputs("Problem with width or height!", stderr);
		fclose(pFile);
		exit(7);
	}

	long l_size = ftell(pFile);
	fseek(pFile, 0, SEEK_END);
	long l_size_all = ftell(pFile);
	fseek(pFile, l_size, SEEK_SET);

	if (!((type_p == 5) || (type_p == 6))) {
		fputs("Type of file not correct!", stderr);
		fclose(pFile);
		exit(4);
	}

	if (type_p == 5) {
		if (width * height != l_size_all - l_size) {
			fputs("Input error", stderr);
			fclose(pFile);
			exit(6);
		}

		unsigned char* k;
		k = (unsigned char*)malloc(sizeof(unsigned char) * height * width);
		int check_size = fread(k, sizeof(unsigned char), width * height, pFile);

		fclose(pFile);

		int ttype;
		if (argv[3] != NULL) {
			ttype = atoi(argv[3]);
		}
		else {
			fputs("Problem with coomands from console", stderr);
			free(k);
			exit(9);
		}

		if (ttype == 0) {
			for (int i = 0; i < height * width; i++) {
				k[i] = maxColorValue - k[i];
			}

			if (!new_file(type_p, width, height, maxColorValue, k, output_file_name, 1)) {
				free(k);
				exit(10);
			}
			free(k);
		}
		else if (ttype == 1) {
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width / 2; j++) {
					unsigned char t = k[i * width + j];
					k[i * width + j] = k[(i + 1) * width - (j + 1)];
					k[(i + 1) * width - (j + 1)] = t;
				}
			}

			if (!new_file(type_p, width, height, maxColorValue, k, output_file_name, 1)) {
				free(k);
				exit(10);
			}
			free(k);
		}
		else if (ttype == 2) {
			for (int i = 0; i < height / 2; i++) {
				for (int j = 0; j < width; j++) {
					unsigned char t = k[i * width + j];
					k[i * width + j] = k[(height - i - 1) * width + j];
					k[(height - i - 1) * width + j] = t;
				}
			}

			if (!new_file(type_p, width, height, maxColorValue, k, output_file_name, 1)) {
				free(k);
				exit(10);
			}
			free(k);
		}
		else if (ttype == 3) {
			unsigned char* k_new;
			k_new = (unsigned char*)malloc(sizeof(unsigned char) * height * width);

			for (int i = 0; i < height * width; i++) {
				k_new[i] = 0;
			}

			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					k_new[height - (i + 1) + j * height] = k[i * width + j];
				}
			}

			if (!new_file(type_p, height, width, maxColorValue, k_new, output_file_name, 1)) {
				free(k_new);
				free(k);
				exit(10);
			}
			free(k_new);
			free(k);
		}
		else if (ttype == 4) {
			unsigned char* k_new;
			k_new = (unsigned char*)malloc(sizeof(unsigned char) * height * width);

			for (int i = 0; i < height * width; i++) {
				k_new[i] = 0;
			}

			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					k_new[i + (width - j - 1) * height] = k[i * width + j];
				}
			}

			if (!new_file(type_p, height, width, maxColorValue, k_new, output_file_name, 1)) {
				free(k_new);
				free(k);
				exit(10);
			}
			free(k_new);
			free(k);
		}
		else {
			fputs("Wrong type of transformation", stderr);
			free(k);
			exit(8);
		}
	}

	if (type_p == 6) {
		if (width * height * 3 != l_size_all - l_size) {
			fputs("Input error", stderr);
			fclose(pFile);
			exit(6);
		}

		unsigned char* k;
		k = (unsigned char*)malloc(sizeof(unsigned char) * height * width * 3);
		int check_size = fread(k, sizeof(unsigned char), width * height * 3, pFile);

		fclose(pFile);

		int ttype;
		if (argv[3] != NULL) {
			ttype = atoi(argv[3]);
		}

		if (ttype == 0) {
			for (int i = 0; i < 3 * height * width; i++) {
				k[i] = maxColorValue - k[i];
			}

			if (!new_file(type_p, width, height, maxColorValue, k, output_file_name, 3)) {
				free(k);
				exit(10);
			}
			free(k);
		}
		else if (ttype == 1) {
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width / 2; j++) {
					unsigned char t_0 = k[i * width * 3 + j * 3];
					unsigned char t_1 = k[i * width * 3 + j * 3 + 1];
					unsigned char t_2 = k[i * width * 3 + j * 3 + 2];
					k[i * width * 3 + j * 3] = k[(i + 1) * width * 3 - (j + 1) * 3];
					k[i * width * 3 + j * 3 + 1] = k[(i + 1) * width * 3 - (j + 1) * 3 + 1];
					k[i * width * 3 + j * 3 + 2] = k[(i + 1) * width * 3 - (j + 1) * 3 + 2];
					k[(i + 1) * width * 3 - (j + 1) * 3] = t_0;
					k[(i + 1) * width * 3 - (j + 1) * 3 + 1] = t_1;
					k[(i + 1) * width * 3 - (j + 1) * 3 + 2] = t_2;
				}
			}

			if (!new_file(type_p, width, height, maxColorValue, k, output_file_name, 3)) {
				free(k);
				exit(10);
			}
			free(k);
		}
		else if (ttype == 2) {
			for (int i = 0; i < height / 2; i++) {
				for (int j = 0; j < width; j++) {
					unsigned char t_0 = k[i * width * 3 + j * 3];
					unsigned char t_1 = k[i * width * 3 + j * 3 + 1];
					unsigned char t_2 = k[i * width * 3 + j * 3 + 2];
					k[i * width * 3 + j * 3] = k[(height - i - 1) * width * 3 + j * 3];
					k[i * width * 3 + j * 3 + 1] = k[(height - i - 1) * width * 3 + j * 3 + 1];
					k[i * width * 3 + j * 3 + 2] = k[(height - i - 1) * width * 3 + j * 3 + 2];
					k[(height - i - 1) * width * 3 + j * 3] = t_0;
					k[(height - i - 1) * width * 3 + j * 3 + 1] = t_1;
					k[(height - i - 1) * width * 3 + j * 3 + 2] = t_2;
				}
			}

			if (!new_file(type_p, width, height, maxColorValue, k, output_file_name, 3)) {
				free(k);
				exit(10);
			}
			free(k);
		}
		else if (ttype == 3) {
			unsigned char* k_new;
			k_new = (unsigned char*)malloc(sizeof(unsigned char) * height * width * 3);

			for (int i = 0; i < 3 * height * width; i++) {
				k_new[i] = 0;
			}

			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					k_new[height * 3 - (i + 1) * 3 + j * height * 3] = k[i * width * 3 + j * 3];
					k_new[height * 3 - (i + 1) * 3 + j * height * 3 + 1] = k[i * width * 3 + j * 3 + 1];
					k_new[height * 3 - (i + 1) * 3 + j * height * 3 + 2] = k[i * width * 3 + j * 3 + 2];
				}
			}

			if (!new_file(type_p, height, width, maxColorValue, k_new, output_file_name, 3)) {
				free(k_new);
				free(k);
				exit(10);
			}
			free(k_new);
			free(k);
		}
		else if (ttype == 4) {
			unsigned char* k_new;
			k_new = (unsigned char*)malloc(sizeof(unsigned char) * height * width * 3);

			for (int i = 0; i < 3 * height * width; i++) {
				k_new[i] = 0;
			}

			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					k_new[i * 3 + (width - j - 1) * height * 3] = k[i * width * 3 + j * 3];
					k_new[i * 3 + (width - j - 1) * height * 3 + 1] = k[i * width * 3 + j * 3 + 1];
					k_new[i * 3 + (width - j - 1) * height * 3 + 2] = k[i * width * 3 + j * 3 + 2];
				}
			}

			if (!new_file(type_p, height, width, maxColorValue, k_new, output_file_name, 3)) {
				free(k_new);
				free(k);
				exit(10);
			}
			free(k_new);
			free(k);
		}
		else {
			fputs("Wrong type of transformation", stderr);
			free(k);
			exit(8);
		}
	}
}
