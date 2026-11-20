
#define cimg_display 1
#include "lib/CImg.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

using namespace std;
using namespace cimg_library;

typedef vector<vector<unsigned char>> Matrix;

Matrix toGrayscale(CImg<unsigned char>& img){
	CImg<unsigned char> gray = img.get_RGBtoYCbCr().get_channel(0);

	Matrix matrix(gray.height(), vector<unsigned char>(gray.width()));

	for (int y = 0; y < gray.height(); y++)
		for (int x = 0; x < gray.width(); x++)
			matrix[y][x] = gray(x, y);

	return matrix;
}


CImg<unsigned char> toImage(const Matrix& matrix)
{
	int h = matrix.size();
	int w = matrix[0].size();

	CImg<unsigned char> img(w, h, 1, 1, 0);

	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			img(x, y) = matrix[y][x];

	return img;
}

void displayImages(CImg<unsigned char>& original, CImg<unsigned char>& transformed, string caption){
	int w = original.width();
	int h = original.height();

	CImg<unsigned char> canvas(2 * w, h + 45, 1, 1, 30);

	unsigned char white[] = {255};

	canvas.draw_text(10, 5, "Original Image", white, 0, 1, 20);
	canvas.draw_text(w + 10, 5, caption.c_str(), white, 0, 1, 20);

	canvas.draw_image(0, 40, original);
	canvas.draw_image(w, 40, transformed);

	CImgDisplay display(canvas, "Image Transformation");

	while (!display.is_closed())
		display.wait();
}



double getInput(string message){
	double value;

	while (true)
	{
		cout << message;

		if (cin >>value)
			return value;

		cin.clear();
	
	}
}

int main()
{
	try
	{
		string filename;

		cout << "Enter image filename: ";
		cin >> filename;

		CImg<unsigned char> img(filename.c_str());

		// Resize while maintaining aspect ratio
		double factor = min(1.0, 800.0 / max(img.width(), img.height()));

		img.resize(max(1, (int)(img.width() * factor)),
				   max(1, (int)(img.height() * factor)));

		Matrix gray = toGrayscale(img);

		// Save grayscale image
		CImg<unsigned char> original = toImage(gray);
		original.save("grayscale.bmp");

		cout << "\nGrayscale image saved successfully!\n";

		int choice;

		do
		{
			cout << "\nIMAGE TRANSFORMATION MENU\n";
			cout << "1. Rotation\n";
			cout << "2. Scaling\n";
			cout << "3. Translation\n";
			cout << "4. Exit\n";

			cout << "Enter choice: ";

			if (!(cin >> choice))
			{
				cout << "Invalid choice!\n";
				cin.clear();
				continue;
			}

			CImg<unsigned char> result;
			string caption;

			switch (choice)
			{
				case 1:
				{
					double angle = getInput("Enter angle: ");
					result = original.get_rotate(angle);
					caption = "Rotation: " + to_string(angle) + " degrees";
					break;
				}

				case 2:
				{
					double scale;
					do
					{
						scale = getInput("Enter scaling factor (>0): ");

						if (scale <= 0)
							cout << "Factor must be positive!\n";

					} while (scale <= 0);

					result = original;

					result.resize(max(1, (int)(original.width() * scale)),max(1, (int)(original.height() * scale)));

					caption = "Scaling: " + to_string(scale);
					break;
				}

				case 3:
				{
					double x = getInput("Enter X translation: ");
					double y = getInput("Enter Y translation: ");

					if ( abs(x) > 1000000 || abs(y) > 1000000)
					{
						cout << "Enter valid whole-number distances!\n";
						break;
					}

					result.assign(original.width(), original.height(), 1, 1, 0);

					result.draw_image((int)x, (int)y, original);

					caption = "Translation: X=" + to_string((int)x) +
							  " Y=" + to_string((int)y);
					break;
				}

				case 4:
					cout << "Exiting.\n";
					continue;

				default:
					cout << "Invalid menu option!\n";
					continue;
			}

			displayImages(original, result, caption);

		} while (choice != 4);
	}

	catch (CImgException& e)
	{
		cout << "Image error: " << e.what() << endl;
	}

	return 0;
}
