#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <cstring>

int width, height;

// запись в файл
bool new_file(int type_p, int maxColorValue, unsigned char* k1, unsigned char* k2, unsigned char* k3, char* name_file, int count_output) {
	// std::cout << "ffff ";
	// std::cout << count_output;
	if (count_output == 1) {
		// std::cout << "gg ";
		FILE* rFile;
		rFile = fopen(name_file, "wb");

		if (rFile == NULL) {
			std::cerr << "File not found!"; // файл не открылс€
			return false;
		}

		// формируем динамический массив дл€ записи в файл
		unsigned char* k;
		k = (unsigned char*)malloc(sizeof(unsigned char) * height * width * 3);
		for (int i = 0; i < height * width; ++i) {
			k[3 * i] = k1[i];
			k[3 * i + 1] = k2[i];
			k[3 * i + 2] = k3[i];
		}


		fprintf(rFile, "P%i\n%i %i\n%i\n", 6, width, height, maxColorValue); // запись данных в файл
		fwrite(k, sizeof(unsigned char), width * height * 3, rFile);

		free(k);
		fclose(rFile);
	}

	if (count_output == 3) {
		FILE* output_file_first;
		FILE* output_file_second;
		FILE* output_file_third;

		std::string only_name;
		std::string after_name;
		for (int i = 0; i < (int)strlen(name_file); ++i) {
			if (name_file[i] == '.') {
				only_name = ((std::string)name_file).substr(0, i);
				after_name = ((std::string)name_file).substr(i, strlen(name_file));
			}
		}

		if (only_name.empty()) {
			std::cerr << "Undefined problem with name of input file ";
			return false;
		}

		output_file_first = fopen((char*)(only_name + "_1" + after_name).c_str(), "wb");
		output_file_second = fopen((char*)(only_name + "_2" + after_name).c_str(), "wb");
		output_file_third = fopen((char*)(only_name + "_3" + after_name).c_str(), "wb");

		fprintf(output_file_first, "P%i\n%i %i\n%i\n", 5, width, height, maxColorValue);
		fprintf(output_file_second, "P%i\n%i %i\n%i\n", 5, width, height, maxColorValue);
		fprintf(output_file_third, "P%i\n%i %i\n%i\n", 5, width, height, maxColorValue);

		fwrite(k1, sizeof(unsigned char), width * height, output_file_first);
		fwrite(k2, sizeof(unsigned char), width * height, output_file_second);
		fwrite(k3, sizeof(unsigned char), width * height, output_file_third);

		fclose(output_file_first);
		fclose(output_file_second);
		fclose(output_file_third);
	}

	return true;
}

void RGB_to_HSL(unsigned char* k1, unsigned char* k2, unsigned char* k3) {
	for (int i = 0; i < width * height; ++i) {
		double R = k1[i] / 255.0;
		double G = k2[i] / 255.0;
		double B = k3[i] / 255.0;

		double max = ((R > B) && (R > G)) ? R : ((B > G) ? B : G);
		double min = ((R < B) && (R < G)) ? R : ((B < G) ? B : G);

		double H = 0;

		if (max != min)
			if (max == R)
				if (G >= B)
					H = 60 * (G - B) / (max - min);
				else
					H = 60 * (G - B) / (max - min) + 360;
			else if (max == G)
				H = 60 * (B - R) / (max - min) + 120;
			else if (max == B)
				H = 60 * (R - G) / (max - min) + 240;

		double S = (max - min) / (1 - abs(1 - (max + min)));
		double L = (max + min) / 2;

		k1[i] = round(H / 366.0 * 255);
		k2[i] = round(S * 255);
		k3[i] = round(L * 255);
	}
}

void HSL_to_RGB(unsigned char* k1, unsigned char* k2, unsigned char* k3) {
	for (int i = 0; i < width * height; ++i) {
		double H = k1[i] / 255.0;
		double S = k2[i] / 255.0;
		double L = k3[i] / 255.0;

		double Q = (L >= 0.5) ? (L + S - (L * S)) : (L * (1.0 + S));
		double P = 2.0 * L - Q;
		double T_r = H + 1 / 3.0;
		double T_g = H;
		double T_b = H - 1 / 3.0;

		T_r = (T_r > 1) ? T_r - 1 : T_r;
		T_b = (T_b < 0) ? T_b + 1 : T_b;

		double T[3];
		T[0] = T_r;
		T[1] = T_g;
		T[2] = T_b;

		for (int j = 0; j < 3; ++j) {
			if (T[j] < 1 / 6.0) {
				T[j] = P + ((Q - P) * 6.0 * T[j]);
			}
			else if (T[j] < 0.5) {
				T[j] = Q;
			}
			else if (T[j] < 2 / 3.0) {
				T[j] = P + ((Q - P) * (2 / 3.0 - T[j]) * 6.0);
			}
			else {
				T[j] = P;
			}
		}

		k1[i] = round(T[0] * 255);
		k2[i] = round(T[1] * 255);
		k3[i] = round(T[2] * 255);
	}
}

void RGB_to_HSV(unsigned char* k1, unsigned char* k2, unsigned char* k3) {
	for (int i = 0; i < width * height; ++i) {
		double R = k1[i] / 255.0;
		double G = k2[i] / 255.0;
		double B = k3[i] / 255.0;

		double max = ((R > B) && (R > G)) ? R : ((B > G) ? B : G);
		double min = ((R < B) && (R < G)) ? R : ((B < G) ? B : G);

		double H = 0;
		if (max != min)
			if (max == R) {
				if (G >= B)
					H = 60 * (G - B) / (max - min);
				else
					H = 60 * (G - B) / (max - min) + 360;
			}
			else if (max == G) {
				H = 60 * (B - R) / (max - min) + 120;
			}
			else if (max == B) {
				H = 60 * (R - G) / (max - min) + 240;
			}


		double S = (max == 0) ? 0 : (1 - min / max);
		double V = max;

		k1[i] = round(H / 360.0 * 255);
		k2[i] = round(S * 255);
		k3[i] = round(V * 255);
	}
}

void HSV_to_RGB(unsigned char* k1, unsigned char* k2, unsigned char* k3) {
	for (int i = 0; i < width * height; ++i) {
		double H = k1[i] / 255.0 * 360;
		double S = k2[i] / 255.0 * 100;
		double V = k3[i] / 255.0 * 100;

		double V_min = (100 - S) * V / 100.0;
		double a = (V - V_min) * ((int)round(H) % 60) / 60.0;
		double V_inc = V_min + a;
		double V_dec = V - a;
		int H_i = (int)floor(H / 60.0) % 6;

		switch (H_i) {
		case 0:
			k1[i] = round(V * 255 / 100.0);
			k2[i] = round(V_inc * 255 / 100.0);
			k3[i] = round(V_min * 255 / 100.0);
			break;

		case 1:
			k1[i] = round(V_dec * 255 / 100.0);
			k2[i] = round(V * 255 / 100.0);
			k3[i] = round(V_min * 255 / 100.0);
			break;

		case 2:
			k1[i] = round(V_min * 255 / 100.0);
			k2[i] = round(V * 255 / 100.0);
			k3[i] = round(V_inc * 255 / 100.0);
			break;

		case 3:
			k1[i] = round(V_min * 255 / 100.0);
			k2[i] = round(V_dec * 255 / 100.0);
			k3[i] = round(V * 255 / 100.0);
			break;

		case 4:
			k1[i] = round(V_inc * 255 / 100.0);
			k2[i] = round(V_min * 255 / 100.0);
			k3[i] = round(V * 255 / 100.0);
			break;

		case 5:
			k1[i] = round(V * 255 / 100.0);
			k2[i] = round(V_min * 255 / 100.0);
			k3[i] = round(V_dec * 255 / 100.0);
			break;
		}
	}
}

void RGB_to_YCbCr601(unsigned char* k1, unsigned char* k2, unsigned char* k3) {
	for (int i = 0; i < width * height; ++i) {
		double R = k1[i];
		double G = k2[i];
		double B = k3[i];

		double K_r = 0.299;
		double K_g = 0.587;
		double K_b = 0.114;
		double Y = K_r * R + K_g * G + K_b * B;
		double Cb = (B - Y) / (2 * (1 - K_b)) + 0.5 * 255;
		double Cr = (R - Y) / (2 * (1 - K_r)) + 0.5 * 255;

		Y = (Y > 255) ? 255 : Y;
		Y = (Y < 0) ? 0 : Y;
		Cb = (Cb > 255) ? 255 : Cb;
		Cb = (Cb < 0) ? 0 : Cb;
		Cr = (Cr > 255) ? 255 : Cr;
		Cr = (Cr < 0) ? 0 : Cr;

		k1[i] = round(Y);
		k2[i] = round(Cb);
		k3[i] = round(Cr);
	}
}

void YCbCr601_to_RGB(unsigned char* k1, unsigned char* k2, unsigned char* k3) {
	for (int i = 0; i < width * height; ++i) {
		double Y = k1[i];
		double Cb = k2[i];
		double Cr = k3[i];

		double K_r = 0.299;
		double K_g = 0.587;
		double K_b = 0.114;
		double R = Y + 2 * (1 - K_r) * (Cr - 0.5 * 255);
		double G = Y - 2 * K_b * (1 - K_b) * (Cb - 0.5 * 255) / K_g - 2 * K_r * (1 - K_r) * (Cr - 0.5 * 255) / K_g;
		double B = Y + 2 * (1 - K_b) * (Cb - 0.5 * 255);

		R = (R > 255) ? 255 : R;
		R = (R < 0) ? 0 : R;
		G = (G > 255) ? 255 : G;
		G = (G < 0) ? 0 : G;
		B = (B > 255) ? 255 : B;
		B = (B < 0) ? 0 : B;

		k1[i] = round(R);
		k2[i] = round(G);
		k3[i] = round(B);
	}
}

void RGB_to_YCbCr709(unsigned char* k1, unsigned char* k2, unsigned char* k3) {
	for (int i = 0; i < width * height; ++i) {
		double R = k1[i];
		double G = k2[i];
		double B = k3[i];

		double K_r = 0.2126;
		double K_g = 0.7152;
		double K_b = 0.0722;
		double Y = K_r * R + K_g * G + K_b * B;
		double Cb = (B - Y) / (2 * (1 - K_b)) + 0.5 * 255;
		double Cr = (R - Y) / (2 * (1 - K_r)) + 0.5 * 255;

		Y = (Y > 255) ? 255 : Y;
		Y = (Y < 0) ? 0 : Y;
		Cb = (Cb > 255) ? 255 : Cb;
		Cb = (Cb < 0) ? 0 : Cb;
		Cr = (Cr > 255) ? 255 : Cr;
		Cr = (Cr < 0) ? 0 : Cr;

		k1[i] = round(Y);
		k2[i] = round(Cb);
		k3[i] = round(Cr);
	}
}

void YCbCr709_to_RGB(unsigned char* k1, unsigned char* k2, unsigned char* k3) {
	for (int i = 0; i < width * height; ++i) {
		double Y = k1[i];
		double Cb = k2[i];
		double Cr = k3[i];

		double K_r = 0.2126;
		double K_g = 0.7152;
		double K_b = 0.0722;
		double R = Y + 2 * (1 - K_r) * (Cr - 0.5 * 255);
		double G = Y - 2 * K_b * (1 - K_b) * (Cb - 0.5 * 255) / K_g - 2 * K_r * (1 - K_r) * (Cr - 0.5 * 255) / K_g;
		double B = Y + 2 * (1 - K_b) * (Cb - 0.5 * 255);

		R = (R > 255) ? 255 : R;
		R = (R < 0) ? 0 : R;
		G = (G > 255) ? 255 : G;
		G = (G < 0) ? 0 : G;
		B = (B > 255) ? 255 : B;
		B = (B < 0) ? 0 : B;

		k1[i] = round(R);
		k2[i] = round(G);
		k3[i] = round(B);
	}
}

void RGB_to_YCoCg(unsigned char* k1, unsigned char* k2, unsigned char* k3) {
	for (int i = 0; i < width * height; ++i) {
		double R = k1[i];
		double G = k2[i];
		double B = k3[i];

		double Y = 0.25 * R + 0.5 * G + 0.25 * B;
		double Co = 0.5 * R - 0.5 * B + 0.5 * 255;
		double Cg = 0.5 * G - 0.25 * B - 0.25 * R + 0.5 * 255;

		Y = (Y > 255) ? 255 : Y;
		Y = (Y < 0) ? 0 : Y;
		Co = (Co > 255) ? 255 : Co;
		Co = (Co < 0) ? 0 : Co;
		Cg = (Cg > 255) ? 255 : Cg;
		Cg = (Cg < 0) ? 0 : Cg;

		k1[i] = round(Y);
		k2[i] = round(Co);
		k3[i] = round(Cg);
	}
}

void YCoCg_to_RGB(unsigned char* k1, unsigned char* k2, unsigned char* k3) {
	for (int i = 0; i < width * height; ++i) {
		double Y = k1[i];
		double Co = k2[i];
		double Cg = k3[i];

		double R = Y + Co - Cg;
		double G = Y + Cg - 0.5 * 255;
		double B = Y - Co - Cg + 255;

		R = (R > 255) ? 255 : R;
		R = (R < 0) ? 0 : R;
		G = (G > 255) ? 255 : G;
		G = (G < 0) ? 0 : G;
		B = (B > 255) ? 255 : B;
		B = (B < 0) ? 0 : B;

		k1[i] = round(R);
		k2[i] = round(G);
		k3[i] = round(B);
	}
}

void RGB_to_CMY(unsigned char* k1, unsigned char* k2, unsigned char* k3) {
	// std::cout << "ch ";
	for (int i = 0; i < width * height; ++i) {
		// if (i < 20) std::cout << (double)k1[i] << " ";
		k1[i] = 255 - k1[i];
		k2[i] = 255 - k2[i];
		k3[i] = 255 - k3[i];
	}
	// std::cout << "\n\n" << "NEXT" << "\n\n";
}

void CMY_to_RGB(unsigned char* k1, unsigned char* k2, unsigned char* k3) {
	for (int i = 0; i < width * height; ++i) {
		k1[i] = 255 - k1[i];
		k2[i] = 255 - k2[i];
		k3[i] = 255 - k3[i];
	}
}

int main(int argc, char* argv[]) {
	// проверка на количество аргументов
	if (argc != 11) {
		std::cerr << "quantity of arguments is incorrect ";
		exit(1);
	}

	// чтение аргументов
	char* from_color_space = (char*)"";
	char* to_color_space = (char*)"";
	char* input_file_name = (char*)"";
	char* output_file_name = (char*)"";
	int count_input = 0;
	int count_output = 0;
	for (int i = 1; i < 11; ++i) {
		// std::cout << argv[i]<< " ";
		if (std::string(argv[i]) == "-f")
			if ((i + 1 < 11) && (argv[i + 1] != NULL)) {
				from_color_space = argv[i + 1];
			}
			else {
				std::cerr << "problem with arguments about \"from color space\" ";
				exit(1);
			}

		if (std::string(argv[i]) == "-t")
			if ((i + 1 < 11) && (argv[i + 1] != NULL)) {
				to_color_space = argv[i + 1];
			}
			else {
				std::cerr << "problem with arguments about \"to color space\" ";
				exit(1);
			}

		if (std::string(argv[i]) == "-i")
			if ((i + 2 < 11) && (argv[i + 1] != NULL) && (argv[i + 2] != NULL)) {
				// std::cout << atoi(argv[i + 1]);
				count_input = atoi(argv[i + 1]);
				input_file_name = argv[i + 2];
			}
			else {
				std::cerr << "problem with arguments about \"input file\" ";
				exit(1);
			}

		if (std::string(argv[i]) == "-o")
			if ((i + 2 < 11) && (argv[i + 1] != NULL) && (argv[i + 2] != NULL)) {
				count_output = atoi(argv[i + 1]);
				output_file_name = argv[i + 2];
			}
			else {
				std::cerr << "problem with arguments about \"output file\" ";
				exit(1);
			}
	}

	// std::cout << "f ";

	// открытие файла(ов) на чтение
	int maxColorValue = 0;
	int type_p = 0;
	char c_type = ' ';
	unsigned char* k1 = (unsigned char*)"";
	unsigned char* k2 = (unsigned char*)"";
	unsigned char* k3 = (unsigned char*)"";
	if (count_input == 1) {
		FILE* input_file;
		input_file = fopen(input_file_name, "rb");

		// файл не открылс€
		if (input_file == NULL) {
			std::cerr << "File not found! ";
			exit(1);
		}

		// чтение заголовка
		fscanf(input_file, "%c%i%i%i%i \n", &c_type, &type_p, &width, &height, &maxColorValue);

		// проверка первой буквы (должна быть 'P')
		if (c_type != 'P') {
			std::cerr << "Problem with type of file! ";
			fclose(input_file);
			exit(1);
		}

		// проверка на тип файла
		if ((type_p != 6)) {
			std::cerr << "Type of file not correct! ";
			fclose(input_file);
			exit(1);
		}

		// проверка значени€ цвета
		if (maxColorValue != 255) {
			std::cerr << "Value of maxColor is not correct! ";
			fclose(input_file);
			exit(1);
		}

		if ((width <= 0) || (height <= 0)) {
			std::cerr << "Problem with width or height! ";
			fclose(input_file);
			exit(1);
		}

		// чтение данных
		unsigned char* k;
		k = (unsigned char*)malloc(sizeof(unsigned char) * height * width * 3);
		int check_size = fread(k, sizeof(unsigned char), width * height * 3, input_file);

		fclose(input_file);

		// сохран€ем данные в динамические массивы, удобные дл€ работы
		k1 = (unsigned char*)malloc(sizeof(unsigned char) * height * width);
		k2 = (unsigned char*)malloc(sizeof(unsigned char) * height * width);
		k3 = (unsigned char*)malloc(sizeof(unsigned char) * height * width);
		for (int i = 0; i < width * height; ++i) {
			k1[i] = k[3 * i];
			k2[i] = k[3 * i + 1];
			k3[i] = k[3 * i + 2];
		}

		free(k);
	}
	else if (count_input == 3) {
		FILE* input_file_first;
		FILE* input_file_second;
		FILE* input_file_third;

		std::string only_name;
		std::string after_name;
		for (int i = 0; i < (int)strlen(input_file_name); ++i) {
			if (input_file_name[i] == '.') {
				only_name = ((std::string)input_file_name).substr(0, i);
				after_name = ((std::string)input_file_name).substr(i, strlen(input_file_name));
			}
		}

		if (only_name.empty()) {
			std::cerr << "Undefined problem with name of input file ";
			exit(1);
		}

		input_file_first = fopen((char*)(only_name + "_1" + after_name).c_str(), "rb");
		input_file_second = fopen((char*)(only_name + "_2" + after_name).c_str(), "rb");
		input_file_third = fopen((char*)(only_name + "_3" + after_name).c_str(), "rb");

		// файл не открылс€
		if ((input_file_first == NULL) || (input_file_second == NULL) || (input_file_third == NULL)) {
			std::cerr << "File not found! ";
			exit(1);
		}

		char c_type_2, c_type_3;
		int type_p_2, type_p_3;
		int width_2, width_3;
		int height_2, height_3;
		int maxColorValue_2, maxColorValue_3;

		// чтение заголовка
		fscanf(input_file_first, "%c%i%i%i%i \n", &c_type, &type_p, &width, &height, &maxColorValue);
		fscanf(input_file_second, "%c%i%i%i%i \n", &c_type_2, &type_p_2, &width_2, &height_2, &maxColorValue_2);
		fscanf(input_file_third, "%c%i%i%i%i \n", &c_type_3, &type_p_3, &width_3, &height_3, &maxColorValue_3);

		// проверка первой буквы (должна быть 'P')
		if ((c_type != 'P') || (c_type_2 != c_type) || (c_type_3 != c_type)) {
			std::cerr << "Problem with type of file! ";
			fclose(input_file_first);
			fclose(input_file_second);
			fclose(input_file_third);
			exit(1);
		}

		// проверка на тип файла
		if ((type_p != 5) || (type_p_2 != 5) || (type_p_3 != 5)) {
			std::cerr << "Type of file not correct! ";
			fclose(input_file_first);
			fclose(input_file_second);
			fclose(input_file_third);
			exit(1);
		}

		// проверка значени€ цвета
		if ((maxColorValue != 255) || (maxColorValue_2 != 255) || (maxColorValue_3 != 255)) {
			std::cerr << "Value of maxColor is not correct! ";
			fclose(input_file_first);
			fclose(input_file_second);
			fclose(input_file_third);
			exit(1);
		}

		// проверка ширина и высоты
		if ((width <= 0) || (height <= 0) || (width_2 != width) || (height_2 != height) || (width_3 != width) || (height_3 != height)) {
			std::cerr << "Problem with width or height! ";
			fclose(input_file_first);
			fclose(input_file_second);
			fclose(input_file_third);
			exit(1);
		}

		// сохран€ем данные в динамические массивы, удобные дл€ работы
		k1 = (unsigned char*)malloc(sizeof(unsigned char) * height * width);
		k2 = (unsigned char*)malloc(sizeof(unsigned char) * height * width);
		k3 = (unsigned char*)malloc(sizeof(unsigned char) * height * width);
		fread(k1, sizeof(unsigned char), width * height, input_file_first);
		fread(k2, sizeof(unsigned char), width * height, input_file_second);
		fread(k3, sizeof(unsigned char), width * height, input_file_third);

		fclose(input_file_first);
		fclose(input_file_second);
		fclose(input_file_third);
	}

	// перевод файла в RGB; если файл уже в RGB, то этот шаг пропускаетс€
	// std::cout << from_color_space;
	if (std::string(from_color_space) == "HSL") {
		HSL_to_RGB(k1, k2, k3);
	}
	else if (std::string(from_color_space) == "HSV") {
		HSV_to_RGB(k1, k2, k3);
	}
	else if (std::string(from_color_space) == "YCbCr.601") {
		YCbCr601_to_RGB(k1, k2, k3);
	}
	else if (std::string(from_color_space) == "YCbCr.709") {
		YCbCr709_to_RGB(k1, k2, k3);
	}
	else if (std::string(from_color_space) == "YCoCg") {
		YCoCg_to_RGB(k1, k2, k3);
	}
	else if (std::string(from_color_space) == "CMY") {
		CMY_to_RGB(k1, k2, k3);
	}

	// перевод файла из RGB; если нужно было перевести файл в RGB, то этот шаг пропускаетс€
	// std::cout << to_color_space;
	if (std::string(to_color_space) == "HSL") {
		RGB_to_HSL(k1, k2, k3);
	}
	else if (std::string(to_color_space) == "HSV") {
		RGB_to_HSV(k1, k2, k3);
	}
	else if (std::string(to_color_space) == "YCbCr.601") {
		RGB_to_YCbCr601(k1, k2, k3);
	}
	else if (std::string(to_color_space) == "YCbCr.709") {
		RGB_to_YCbCr709(k1, k2, k3);
	}
	else if (std::string(to_color_space) == "YCoCg") {
		RGB_to_YCoCg(k1, k2, k3);
	}
	else if (std::string(to_color_space) == "CMY") {
		RGB_to_CMY(k1, k2, k3);
	}

	/* for (int i = 0; i <= 20; ++i) {
		std::cout << (double)k1[i] << " ";
	}*/

	// std::cout << count_output;
	// std::cout << "gf ";

	if (!(new_file(type_p, maxColorValue, k1, k2, k3, output_file_name, count_output))) {
		free(k1);
		free(k2);
		free(k3);
		exit(1);
	}

	free(k1);
	free(k2);
	free(k3);

	exit(0);
}
