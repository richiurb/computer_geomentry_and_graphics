#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdlib>

using namespace std;

int width, height;
bool check_srgb;

// запись в файл
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

void no_dithering(unsigned char* k, double& gamma, int& bitt) {
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			double plot_was = k[i * width + j] / 255.0;

			double srgb_was = check_srgb ?
				((plot_was <= 0.04045) ? (25 * plot_was / 323) : (pow((200 * plot_was + 11) / 211, gamma))) :
				(pow(plot_was, gamma));

			double srgb_final = srgb_was;
			srgb_final = srgb_final < 0 ? 0 : srgb_final;
			srgb_final = srgb_final > 1 ? 1 : srgb_final;
			srgb_final = floor(srgb_final * (bitt + 1)) / bitt;
			srgb_final = srgb_final > 1 ? 1 : srgb_final;

			double plot_final = check_srgb ?
				((srgb_final <= 0.0031308) ? (323 * srgb_final / 25) : ((211 * pow(srgb_final, 1 / gamma) - 11) / 200)) :
				pow(srgb_final, 1 / gamma);

			k[i * width + j] = round(255 * plot_final);
		}
	}
}

void ordered_dithering(unsigned char* k, double& gamma, int& bitt) {
	// genarate thershold map 8x8
	double M[8][8];
	M[0][0] = 0;
	M[4][4] = 1;
	M[4][0] = 2;
	M[0][4] = 3;
	M[2][2] = 4;
	M[6][6] = 5;
	M[6][2] = 6;
	M[2][6] = 7;
	M[2][0] = 8;
	M[6][4] = 9;
	M[6][0] = 10;
	M[2][4] = 11;
	M[0][2] = 12;
	M[4][6] = 13;
	M[4][2] = 14;
	M[0][6] = 15;
	for (int i = 0; i < 8; i += 2) {
		for (int j = 0; j < 8; j += 2) {
			M[i + 1][j + 1] = M[i][j] + 16;
			M[i][j + 1] = M[i][j] + 32;
			M[i + 1][j] = M[i][j] + 48;
		}
	}

	// pre-calculated threshold map 8x8
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			M[i][j] = (M[i][j] - 31) / (64.0 * bitt);
		}
	}

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			double plot_was = k[i * width + j] / 255.0;

			double srgb_was = check_srgb ?
				((plot_was <= 0.04045) ? (25 * plot_was / 323) : (pow((200 * plot_was + 11) / 211, gamma))) :
				(pow(plot_was, gamma));

			double srgb_final = srgb_was + M[i % 8][j % 8];
			srgb_final = srgb_final < 0 ? 0 : srgb_final;
			srgb_final = srgb_final > 1 ? 1 : srgb_final;
			srgb_final = round(srgb_final * bitt) / bitt;

			double plot_final = check_srgb ?
				((srgb_final <= 0.0031308) ? (323 * srgb_final / 25) : ((211 * pow(srgb_final, 1 / gamma) - 11) / 200)) :
				pow(srgb_final, 1 / gamma);

			k[i * width + j] = round(255 * plot_final);
		}
	}
}

void random_ordered_dithering(unsigned char* k, double& gamma, int& bitt) {
	srand(3732);

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			double plot_was = k[i * width + j] / 255.0;

			double srgb_was = check_srgb ?
				((plot_was <= 0.04045) ? (25 * plot_was / 323) : (pow((200 * plot_was + 11) / 211, gamma))) :
				(pow(plot_was, gamma));

			double srgb_final = srgb_was + ((rand() % 64) - 31) / (64.0 * bitt);
			srgb_final = srgb_final < 0 ? 0 : srgb_final;
			srgb_final = srgb_final > 1 ? 1 : srgb_final;
			srgb_final = round(srgb_final * bitt) / bitt;

			double plot_final = check_srgb ?
				((srgb_final <= 0.0031308) ? (323 * srgb_final / 25) : ((211 * pow(srgb_final, 1 / gamma) - 11) / 200)) :
				pow(srgb_final, 1 / gamma);

			k[i * width + j] = round(255 * plot_final);
		}
	}
}

void Floyd_Steinberg(unsigned char* k, double& gamma, int& bitt) {
	vector<vector<double>> add_plot(height);
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			add_plot[i].push_back(0);
		}
	}

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			double plot_was = k[i * width + j] / 255.0;

			double srgb_was = check_srgb ?
				((plot_was <= 0.04045) ? (25 * plot_was / 323) : (pow((200 * plot_was + 11) / 211, gamma))) :
				(pow(plot_was, gamma));

			double srgb_final = srgb_was + add_plot[i][j];
			double eq_final = srgb_final;
			srgb_final = srgb_final < 0 ? 0 : srgb_final;
			srgb_final = srgb_final > 1 ? 1 : srgb_final;
			srgb_final = round(srgb_final * bitt) / bitt;

			double diff = (srgb_was - srgb_final + add_plot[i][j]) / 16.0;
			if ((i < height - 1) && (j < width - 1))
				add_plot[i + 1][j + 1] += diff;
			if ((i < height - 1) && (j > 0))
				add_plot[i + 1][j - 1] += 3 * diff;
			if (i < height - 1)
				add_plot[i + 1][j] += 5 * diff;
			if (j < width - 1)
				add_plot[i][j + 1] += 7 * diff;

			double plot_final = check_srgb ?
				((srgb_final <= 0.0031308) ? (323 * srgb_final / 25) : ((211 * pow(srgb_final, 1 / gamma) - 11) / 200)) :
				pow(srgb_final, 1 / gamma);

			k[i * width + j] = round(255 * plot_final);
		}
	}
}

void Jarvis_Judice_Ninke(unsigned char* k, double& gamma, int& bitt) {
	vector<vector<double>> add_plot(height);
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			add_plot[i].push_back(0);
		}
	}

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			double plot_was = k[i * width + j] / 255.0;

			double srgb_was = check_srgb ?
				((plot_was <= 0.04045) ? (25 * plot_was / 323) : (pow((200 * plot_was + 11) / 211, gamma))) :
				(pow(plot_was, gamma));

			double srgb_final = srgb_was + add_plot[i][j];
			double eq_final = srgb_final;
			srgb_final = srgb_final < 0 ? 0 : srgb_final;
			srgb_final = srgb_final > 1 ? 1 : srgb_final;
			srgb_final = round(srgb_final * bitt) / bitt;

			double diff = (srgb_was - srgb_final + add_plot[i][j]) / 48.0;
			if (j < width - 1)
				add_plot[i][j + 1] += 7 * diff;
			if (j < width - 2)
				add_plot[i][j + 2] += 5 * diff;
			if ((i < height - 1) && (j > 1))
				add_plot[i + 1][j - 2] += 3 * diff;
			if ((i < height - 1) && (j > 0))
				add_plot[i + 1][j - 1] += 5 * diff;
			if (i < height - 1)
				add_plot[i + 1][j] += 7 * diff;
			if ((i < height - 1) && (j < width - 1))
				add_plot[i + 1][j + 1] += 5 * diff;
			if ((i < height - 1) && (j < width - 2))
				add_plot[i + 1][j + 2] += 3 * diff;
			if ((i < height - 2) && (j > 1))
				add_plot[i + 2][j - 2] += diff;
			if ((i < height - 2) && (j > 0))
				add_plot[i + 2][j - 1] += 3 * diff;
			if (i < height - 2)
				add_plot[i + 2][j] += 5 * diff;
			if ((i < height - 2) && (j < width - 1))
				add_plot[i + 2][j + 1] += 3 * diff;
			if ((i < height - 2) && (j < width - 2))
				add_plot[i + 2][j + 2] += diff;

			double plot_final = check_srgb ?
				((srgb_final <= 0.0031308) ? (323 * srgb_final / 25) : ((211 * pow(srgb_final, 1 / gamma) - 11) / 200)) :
				pow(srgb_final, 1 / gamma);

			k[i * width + j] = round(255 * plot_final);
		}
	}
}

void Sierra_three(unsigned char* k, double& gamma, int& bitt) {
	vector<vector<double>> add_plot(height);
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			add_plot[i].push_back(0);
		}
	}

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			double plot_was = k[i * width + j] / 255.0;

			double srgb_was = check_srgb ?
				((plot_was <= 0.04045) ? (25 * plot_was / 323) : (pow((200 * plot_was + 11) / 211, gamma))) :
				(pow(plot_was, gamma));

			double srgb_final = srgb_was + add_plot[i][j];
			double eq_final = srgb_final;
			srgb_final = srgb_final < 0 ? 0 : srgb_final;
			srgb_final = srgb_final > 1 ? 1 : srgb_final;
			srgb_final = round(srgb_final * bitt) / bitt;

			double diff = (srgb_was - srgb_final + add_plot[i][j]) / 32.0;
			if (j < width - 1)
				add_plot[i][j + 1] += 5 * diff;
			if (j < width - 2)
				add_plot[i][j + 2] += 3 * diff;
			if ((i < height - 1) && (j > 1))
				add_plot[i + 1][j - 2] += 2 * diff;
			if ((i < height - 1) && (j > 0))
				add_plot[i + 1][j - 1] += 4 * diff;
			if (i < height - 1)
				add_plot[i + 1][j] += 5 * diff;
			if ((i < height - 1) && (j < width - 1))
				add_plot[i + 1][j + 1] += 4 * diff;
			if ((i < height - 1) && (j < width - 2))
				add_plot[i + 1][j + 2] += 2 * diff;
			if ((i < height - 2) && (j > 0))
				add_plot[i + 2][j - 1] += 2 * diff;
			if (i < height - 2)
				add_plot[i + 2][j] += 3 * diff;
			if ((i < height - 2) && (j < width - 1))
				add_plot[i + 2][j + 1] += 2 * diff;

			double plot_final = check_srgb ?
				((srgb_final <= 0.0031308) ? (323 * srgb_final / 25) : ((211 * pow(srgb_final, 1 / gamma) - 11) / 200)) :
				pow(srgb_final, 1 / gamma);

			k[i * width + j] = round(round(plot_final * bitt) * (255 / bitt));
		}
	}
}

void Atkinson(unsigned char* k, double& gamma, int& bitt) {
	vector<vector<double>> add_plot(height);
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			add_plot[i].push_back(0);
		}
	}

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			double plot_was = k[i * width + j] / 255.0;

			double srgb_was = check_srgb ?
				((plot_was <= 0.04045) ? (25 * plot_was / 323) : (pow((200 * plot_was + 11) / 211, gamma))) :
				(pow(plot_was, gamma));

			double srgb_final = srgb_was + add_plot[i][j];
			double eq_final = srgb_final;
			srgb_final = srgb_final < 0 ? 0 : srgb_final;
			srgb_final = srgb_final > 1 ? 1 : srgb_final;
			srgb_final = round(srgb_final * bitt) / bitt;

			double diff = (srgb_was - srgb_final + add_plot[i][j]) / 8.0;
			if (j < width - 1)
				add_plot[i][j + 1] += diff;
			if (j < width - 2)
				add_plot[i][j + 2] += diff;
			if ((i < height - 1) && (j > 0))
				add_plot[i + 1][j - 1] += diff;
			if (i < height - 1)
				add_plot[i + 1][j] += diff;
			if ((i < height - 1) && (j < width - 1))
				add_plot[i + 1][j + 1] += diff;
			if (i < height - 2)
				add_plot[i + 2][j] += diff;

			double plot_final = check_srgb ?
				((srgb_final <= 0.0031308) ? (323 * srgb_final / 25) : ((211 * pow(srgb_final, 1 / gamma) - 11) / 200)) :
				pow(srgb_final, 1 / gamma);

			k[i * width + j] = round(255 * plot_final);
		}
	}
}

void halftone(unsigned char* k, double& gamma, int& bitt) {
	// genarate thershold map 8x8
	double M[4][4];
	M[0][0] = 6;
	M[0][1] = 12;
	M[0][2] = 10;
	M[0][3] = 3;
	M[1][0] = 11;
	M[1][1] = 15;
	M[1][2] = 13;
	M[1][3] = 7;
	M[2][0] = 9;
	M[2][1] = 14;
	M[2][2] = 5;
	M[2][3] = 1;
	M[3][0] = 4;
	M[3][1] = 8;
	M[3][2] = 2;
	M[3][3] = 0;

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			M[i][j] = (M[i][j] - 7.5) / (17.0 * bitt);
		}
	}

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			double plot_was = k[i * width + j] / 255.0;

			double srgb_was = check_srgb ?
				((plot_was <= 0.04045) ? (25 * plot_was / 323) : (pow((200 * plot_was + 11) / 211, gamma))) :
				(pow(plot_was, gamma));

			double srgb_final = srgb_was + M[i % 4][j % 4];
			srgb_final = srgb_final < 0 ? 0 : srgb_final;
			srgb_final = srgb_final > 1 ? 1 : srgb_final;
			srgb_final = round(srgb_final * bitt) / bitt;

			double plot_final = check_srgb ?
				((srgb_final <= 0.0031308) ? (323 * srgb_final / 25) : ((211 * pow(srgb_final, 1 / gamma) - 11) / 200)) :
				pow(srgb_final, 1 / gamma);

			k[i * width + j] = round(255 * plot_final);
		}
	}
}

void make_gradient(unsigned char* k, double gamma) {
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			double plot_final = j / (double)width;
			k[i * width + j] = round(plot_final * 255);
		}
	}
}

int main(int argc, char* argv[]) {
	FILE* input_file;

	/// получение имени входного файла
	char* input_file_name;
	if (argv[1] != NULL) {
		input_file_name = argv[1];
	}
	else {
		cerr << "input file name is not defined";
		exit(1);
	}

	/// получение имени выходного файла
	char* output_file_name = argv[2];
	if (argv[2] != NULL) {
		output_file_name = argv[2];
	}
	else {
		cerr << "output file name is not defined";
		exit(1);
	}

	/// получение градиента
	int gradient;
	if (argv[3] != NULL) {
		gradient = atoi(argv[3]);
	}
	else {
		cerr << "gradient is not defined";
		exit(1);
	}

	/// получение дизеринга
	int dither;
	if (argv[4] != NULL) {
		dither = atoi(argv[4]);
	}
	else {
		cerr << "dithering is not defined";
		exit(1);
	}

	/// получение битности
	int bitt;
	if (argv[5] != NULL) {
		bitt = atoi(argv[5]);
		bitt = pow(2, bitt) - 1;
	}
	else {
		cerr << "bit is not defined";
		exit(1);
	}

	/// получение значени€ гаммы
	double gamma;
	if (argv[6] != NULL) {
		gamma = atof(argv[6]);
		check_srgb = false;
		if (gamma == 0) {
			gamma = 2.2;
			check_srgb = true;
		}
	}
	else {
		cerr << "bit is not defined";
		exit(1);
	}

	/// получение данных из файла
	input_file = fopen(input_file_name, "rb");

	// файл не открылс€
	if (input_file == NULL) {
		cerr << "File not found!";
		exit(1);
	}

	int maxColorValue;
	int type_p;
	char c_type;

	// чтение заголовка
	fscanf(input_file, "%c%i%i%i%i\n", &c_type, &type_p, &width, &height, &maxColorValue);

	// проверка первой буквы (должна быть 'P')
	if (c_type != 'P') {
		cerr << "Problem with type of file!";
		fclose(input_file);
		exit(1);
	}

	// проверка значени€ цвета
	if (maxColorValue != 255) {
		cerr << "Value of maxColor is not correct!";
		fclose(input_file);
		exit(1);
	}

	// проверка на тип файла
	if (!(type_p == 5)) {
		cerr << "Type of file not correct!";
		fclose(input_file);
		exit(1);
	}

	// сохран€ем €ркостные данные
	unsigned char* k;
	k = (unsigned char*)malloc(sizeof(unsigned char) * height * width);
	int check_size = fread(k, sizeof(unsigned char), width * height, input_file);

	fclose(input_file);

	if (gradient == 1) {
		make_gradient(k, gamma);
	}

	switch (dither) {
	case 0:
		no_dithering(k, gamma, bitt);
		break;
	case 1:
		ordered_dithering(k, gamma, bitt);
		break;
	case 2:
		random_ordered_dithering(k, gamma, bitt);
		break;
	case 3:
		Floyd_Steinberg(k, gamma, bitt);
		break;
	case 4:
		Jarvis_Judice_Ninke(k, gamma, bitt);
		break;
	case 5:
		Sierra_three(k, gamma, bitt);
		break;
	case 6:
		Atkinson(k, gamma, bitt);
		break;
	case 7:
		halftone(k, gamma, bitt);
		break;
	}

	/* for (int i = 0; i < width * height; ++i) {
		k[i] = 0;
	} */

	new_file(type_p, width, height, maxColorValue, k, output_file_name, 1);
	free(k);
	/// конец программы
	exit(0);
}
